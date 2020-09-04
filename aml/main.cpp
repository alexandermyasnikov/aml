#include <iostream>
#include <variant>
#include <iomanip>
#include <regex>

#include "utils.h"
#include "lisp_tree.h"
#include "token.h"

#include "debug_logger.h"

#define DEBUG_LOGGER_TRACE_LA            DEBUG_LOGGER("la   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_LA(...)             DEBUG_LOG("la   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_SA            // DEBUG_LOGGER("sa   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_SA(...)             DEBUG_LOG("sa   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_ICG           // DEBUG_LOGGER("icg  ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_ICG(...)            DEBUG_LOG("icg  ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_CG            // DEBUG_LOGGER("cg   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_CG(...)             // DEBUG_LOG("cg   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_EXEC          DEBUG_LOGGER("exec ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_EXEC(...)           DEBUG_LOG("exec ", logger_indent_aml_t::indent, __VA_ARGS__)

template <typename T>
struct logger_indent_t { static inline int indent = 0; };

struct logger_indent_aml_t : logger_indent_t<logger_indent_aml_t> { };



namespace aml_n {

  namespace lexical_analyzer_n {
    auto process(const std::string& code) {
      return aml::token_n::process(code);
    }
  }

  namespace syntax_lisp_analyzer_n {
    auto process(const aml::token_n::tokens_t& tokens) {
      return aml::lisp_tree_n::process(tokens);
    }
  }



#if 0
  namespace syntax_analyzer_n {

    using namespace aml::utils_n;
    using namespace syntax_lisp_analyzer_n;

    // GRAMMAR
    // program: func+   expr
    // expr:    ARG     <digit>
    // expr:    BLOCK   expr+
    // expr:    CALL    expr    expr*
    // expr:    DEFVAR  <name>  expr
    // expr:    IF      expr    expr expr
    // expr:    INT     <digit>
    // expr:    SYSCALL expr+
    // expr:    VAR     <name>
    // func:    DEFN    expr    expr

    struct syntax_error_t : fatal_error_t {
      syntax_error_t(const token_t& token, token_t::type_t type = token_t::type_t::unknown)
        : fatal_error_t("synax error at or near '" + token.lexeme + "' at '" + token.pos.show() + "'"
            + (token_t{.type = type}.is_primary() ? ", expected '" + token_t{.type = type}.show() + "'" : "")) { }
    };

    void check_not_leaf(const syntax_lisp_tree_t& tree) {
      if (tree.is_leaf())
        throw syntax_error_t(tree.node);
    }

    void check_size_eq(const syntax_lisp_tree_t& tree, size_t count) {
      if (tree.nodes.size() != count)
        throw syntax_error_t(tree.node);
    }

    void check_size_gt(const syntax_lisp_tree_t& tree, size_t count) {
      if (tree.nodes.size() < count)
        throw syntax_error_t(tree.node);
    }

    void check_type(const syntax_lisp_tree_t& tree, token_t::type_t type) {
      if (!tree.is_leaf() || tree.node.type != type)
        throw syntax_error_t(tree.node, type);
    }

















    struct stmt_program_t;
    struct stmt_arg_t;
    struct stmt_block_t;
    struct stmt_call_t;
    struct stmt_defn_t;
    struct stmt_defvar_t;
    struct stmt_expr_t;
    struct stmt_func_t;
    struct stmt_if_t;
    struct stmt_int_t;
    struct stmt_syscall_t;
    struct stmt_var_t;

    struct stmt_t {
      virtual ~stmt_t() { }
      virtual std::string show(size_t deep) const = 0;
      virtual void intermediate_code(code_ctx_t& code_ctx) const = 0;
    };

    struct stmt_program_t : stmt_t {
      using funcs_t = std::deque<std::shared_ptr<stmt_defn_t>>;
      using body_t  = std::shared_ptr<stmt_expr_t>;

      env_sptr_t env;
      funcs_t    funcs;
      body_t     body;

      stmt_program_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_arg_t : stmt_t {
      int64_t value = {};

      stmt_arg_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_block_t : stmt_t {
      std::deque<std::shared_ptr<stmt_expr_t>> exprs;

      stmt_block_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_call_t : stmt_t {
      std::shared_ptr<stmt_expr_t>              name;
      std::vector<std::shared_ptr<stmt_expr_t>> args;

      stmt_call_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_defn_t : stmt_t {
      std::shared_ptr<var_info_t>  var;
      std::shared_ptr<stmt_expr_t> body;
      std::shared_ptr<env_t>       env;

      stmt_defn_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_defvar_t : stmt_t {
      std::shared_ptr<var_info_t>  var;
      std::shared_ptr<stmt_expr_t> body;
      std::shared_ptr<env_t>       env;

      stmt_defvar_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_expr_t : stmt_t {
      using expr_t = std::variant<
        std::shared_ptr<stmt_arg_t>,
        std::shared_ptr<stmt_block_t>,
        std::shared_ptr<stmt_call_t>,
        std::shared_ptr<stmt_defvar_t>,
        std::shared_ptr<stmt_func_t>,
        std::shared_ptr<stmt_if_t>,
        std::shared_ptr<stmt_int_t>,
        std::shared_ptr<stmt_syscall_t>,
        std::shared_ptr<stmt_var_t>>;

      expr_t expr;

      stmt_expr_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_func_t : stmt_t {
      std::shared_ptr<var_info_t>  var;
      std::shared_ptr<env_t>       env;

      stmt_func_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_if_t : stmt_t {
      std::shared_ptr<stmt_expr_t> expr_if;
      std::shared_ptr<stmt_expr_t> expr_then;
      std::shared_ptr<stmt_expr_t> expr_else;

      stmt_if_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_int_t : stmt_t {
      int64_t value = {};

      stmt_int_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_syscall_t : stmt_t {
      std::vector<std::shared_ptr<stmt_expr_t>> args;

      stmt_syscall_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    struct stmt_var_t : stmt_t {
      std::shared_ptr<var_info_t> var;

      stmt_var_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
      void intermediate_code(code_ctx_t& code_ctx) const override;
    };

    ////////////////////////////////////////////////////////////////////////////////

    stmt_program_t::stmt_program_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 1);

      env = std::make_shared<env_t>();

      for (size_t i{}; i < tree.nodes.size() - 1; ++i) {
        funcs.push_back(std::make_shared<stmt_defn_t>(tree.nodes[i], env));
      }
      body = std::make_shared<stmt_expr_t>(tree.nodes.back(), env);

      DEBUG_LOGGER_SA("env: \n%s", env->show().c_str());
    }

    std::string stmt_program_t::show(size_t deep) const {
      std::string str;
      for (const auto& func : funcs) {
        str += func->show(deep + 1);
        str += token_t{.type = token_t::type_t::new_line}.show();
        str += token_t{.type = token_t::type_t::new_line}.show();
      }
      str += body->show(deep + 1);
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += token_t{.type = token_t::type_t::new_line}.show();
      return str;
    }

    void stmt_program_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      for (const auto& func : funcs) {
        func->intermediate_code(code_ctx);
      }
      code_ctx.rip = code_ctx.code.buffer.size();
      DEBUG_LOGGER_ICG("rip start: %d", code_ctx.rip);
      body->intermediate_code(code_ctx);
      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::exit);
    }

    stmt_arg_t::stmt_arg_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_arg);
      check_type(tree.nodes[1], token_t::type_t::integer);

      value = std::get<int64_t>(tree.nodes[1].node.value);
    }

    std::string stmt_arg_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_arg}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += std::to_string(value);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_arg_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::arg);
      code_ctx.code.write_i64(code_ctx.rsp + value + 1/*rbp*/ + 1/*rip*/ + 1);
      code_ctx.rsp++;
    }

    stmt_block_t::stmt_block_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_block);

      auto env_block = std::make_shared<env_t>(env);

      for (const auto& node : tree.nodes) {
        exprs.push_back(std::make_shared<stmt_expr_t>(node, env_block));
      }
    }

    std::string stmt_block_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_block}.show();
      for (const auto& expr : exprs) {
        str += token_t{.type = token_t::type_t::new_line}.show();
        str += indent(deep);
        str += expr->show(deep + 1);
      }
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_block_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      throw fatal_error_t("TODO");
    }

    stmt_call_t::stmt_call_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_call);

      name = std::make_shared<stmt_expr_t>(tree.nodes[1], env);
      for (size_t i = 2; i < tree.nodes.size(); ++i) {
        args.push_back(std::make_shared<stmt_expr_t>(tree.nodes[i], env));
      }
    }

    std::string stmt_call_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_call}.show();
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += name->show(deep + 1);
      for (const auto& arg : args) {
        str += token_t{.type = token_t::type_t::new_line}.show();
        str += indent(deep);
        str += arg->show(deep + 1);
      }
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_call_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      name->intermediate_code(code_ctx);

      for (const auto& arg : args | std::views::reverse) {
        arg->intermediate_code(code_ctx);
      }
      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::push8);
      code_ctx.code.write_i64(args.size() + 1);
      code_ctx.rsp++;

      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::call);
      code_ctx.rsp -= 1/*<count>*/ + (args.size() + 1);
      code_ctx.rsp += 1/*<return>*/;
    }

    stmt_defvar_t::stmt_defvar_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_var);
      check_type(tree.nodes[1], token_t::type_t::ident);

      var = env->def_var(std::get<std::string>(tree.nodes[1].node.value));
      body = std::make_shared<stmt_expr_t>(tree.nodes[2], env);
      // env TODO
    }

    std::string stmt_defvar_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_var}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += var->name;
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_defvar_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      throw fatal_error_t("TODO");
    }

    stmt_expr_t::stmt_expr_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 1);

      switch (tree.nodes[0].node.type) {
        case token_t::type_t::key_arg:     expr = std::make_shared<stmt_arg_t>(tree);          break;
        case token_t::type_t::key_block:   expr = std::make_shared<stmt_block_t>(tree, env);   break;
        case token_t::type_t::key_call:    expr = std::make_shared<stmt_call_t>(tree, env);    break;
        case token_t::type_t::key_defvar:  expr = std::make_shared<stmt_defvar_t>(tree, env);  break;
        case token_t::type_t::key_func:    expr = std::make_shared<stmt_func_t>(tree, env);    break;
        case token_t::type_t::key_if:      expr = std::make_shared<stmt_if_t>(tree, env);      break;
        case token_t::type_t::key_int:     expr = std::make_shared<stmt_int_t>(tree);          break;
        case token_t::type_t::key_syscall: expr = std::make_shared<stmt_syscall_t>(tree, env); break;
        case token_t::type_t::key_var:     expr = std::make_shared<stmt_var_t>(tree, env);     break;
        default: throw syntax_error_t(tree.nodes[0].node);
      }
    }

    std::string stmt_expr_t::show(size_t deep) const {
      std::string str;
      std::visit(overloaded{[&str, deep] (const auto &expr) { str = expr->show(deep); } }, expr);
      return str;
    }

    void stmt_expr_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      std::visit(overloaded{[&code_ctx] (const auto &expr) { expr->intermediate_code(code_ctx); } }, expr);
    }

    stmt_func_t::stmt_func_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_eq(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_func);
      check_type(tree.nodes[1], token_t::type_t::ident);

      var = env->get_func(std::get<std::string>(tree.nodes[1].node.value));
    }

    std::string stmt_func_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_func}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += var->name;
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_func_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::push8);
      code_ctx.code.write_i64(var->offset);
      code_ctx.rsp++;
    }

    stmt_if_t::stmt_if_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_eq(tree, 4);
      check_type(tree.nodes[0], token_t::type_t::key_if);

      expr_if   = std::make_shared<stmt_expr_t>(tree.nodes[1], env);
      expr_then = std::make_shared<stmt_expr_t>(tree.nodes[2], env);
      expr_else = std::make_shared<stmt_expr_t>(tree.nodes[3], env);
    }

    std::string stmt_if_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_if}.show();
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += expr_if->show(deep + 1);
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += expr_then->show(deep + 1);
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += expr_else->show(deep + 1);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_if_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;

      expr_if->intermediate_code(code_ctx);

      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::pop_jif);
      code_ctx.rsp--;
      size_t m1 = code_ctx.code.buffer.size();
      code_ctx.code.write_i64(-1);

      expr_then->intermediate_code(code_ctx);

      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::jmp);
      code_ctx.rsp--;
      size_t m2 = code_ctx.code.buffer.size();
      code_ctx.code.write_i64(-1);

      code_ctx.code.write_i64(code_ctx.code.buffer.size(), m1);

      expr_else->intermediate_code(code_ctx);

      code_ctx.code.write_i64(code_ctx.code.buffer.size(), m2);
    }

    stmt_int_t::stmt_int_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_int);
      check_type(tree.nodes[1], token_t::type_t::integer);

      value = std::get<int64_t>(tree.nodes[1].node.value);
    }

    std::string stmt_int_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_int}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += std::to_string(value);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_int_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::push8);
      code_ctx.code.write_i64(value);
      code_ctx.rsp++;
    }

    stmt_defn_t::stmt_defn_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_eq(tree, 3);
      check_type(tree.nodes[0], token_t::type_t::key_defn);
      check_type(tree.nodes[1], token_t::type_t::ident);

      this->env = env;
      var = env->def_func(std::get<std::string>(tree.nodes[1].node.value));
      body = std::make_shared<stmt_expr_t>(tree.nodes[2], env);
      // DEBUG_LOGGER_SA("env: \n%s", this->env->show().c_str());
    }

    std::string stmt_defn_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_defn}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += var->name;
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += body->show(deep + 1);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_defn_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      var->offset = code_ctx.code.buffer.size();
      DEBUG_LOGGER_ICG("name: %s \t %d", var->name.c_str(), var->offset);
      body->intermediate_code(code_ctx);
      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::ret);
      code_ctx.rsp = {};
    }

    stmt_syscall_t::stmt_syscall_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_syscall);

      for (const auto& node : tree.nodes | std::views::drop(1)) {
        args.push_back(std::make_shared<stmt_expr_t>(node, env));
      }
    }

    std::string stmt_syscall_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_syscall}.show();
      for (const auto& arg : args) {
        str += token_t{.type = token_t::type_t::new_line}.show();
        str += indent(deep);
        str += arg->show(deep + 1);
      }
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_syscall_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      for (const auto& arg : args | std::views::reverse) {
        arg->intermediate_code(code_ctx);
      }
      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::push8);
      code_ctx.code.write_i64(args.size());
      code_ctx.rsp++;

      code_ctx.code.write_u8((uint8_t) instruction_rpn_t::syscall);
      code_ctx.rsp -= 1/*<count>*/ + args.size();
      code_ctx.rsp += 1/*<return>*/;
      code_ctx.rsp += 1/*<rbp>*/ + 1/*<rip>*/;
    }

    stmt_var_t::stmt_var_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_var);
      check_type(tree.nodes[1], token_t::type_t::ident);

      var = env->def_var(std::get<std::string>(tree.nodes[1].node.value));
    }

    std::string stmt_var_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_var}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += var->name;
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    void stmt_var_t::intermediate_code(code_ctx_t& code_ctx) const {
      DEBUG_LOGGER_TRACE_ICG;
      throw fatal_error_t("TODO");
    }

    std::shared_ptr<stmt_t> process(const syntax_lisp_tree_t& tree) {
      return std::make_shared<stmt_program_t>(tree);
    }
  }



  namespace semantic_analyzer_n {

    using namespace aml::utils_n;
    using namespace syntax_analyzer_n;
  }



  namespace intermediate_code_generator_n {

    using namespace aml::utils_n;
    using namespace syntax_analyzer_n;

    code_ctx_t process(std::shared_ptr<stmt_t> stmt) {
      code_ctx_t code_ctx;
      stmt->intermediate_code(code_ctx);
      return code_ctx;
    }
  }



  namespace code_optimizer_n {
  }



#if 0
  namespace code_generator_n {
    using namespace intermediate_code_generator_n;

    void process(data_t& text, const instructions_t& instructions) {
      text.assign(sizeof(instruction_t) * instructions.size(), 0);
      for (size_t i = 0; i < instructions.size(); ++i) {
        memcpy(text.data() + i * sizeof(instruction_t), &instructions[i].value, sizeof(instruction_t));
      }

      for (size_t i = 0; i < text.size(); i += sizeof(instruction_t)) {
        DEBUG_LOGGER_CG("text: '%02hhx%02hhx'", text.at(i), text.at(i + 1));
      }
    }
  }
#endif



  namespace executor_n {

    using namespace intermediate_code_generator_n;

    struct machine_t {
      std::deque<int64_t> stack;
      int64_t rbp = {};
      size_t rip = {};

      std::string show() {
        std::string str;
        str += "size: " + std::to_string(stack.size()) + "\t";
        str += "rbp: " + std::to_string(rbp) + "\t";
        str += "rip: " + std::to_string(rip) + "\t";
        str += "stack: ";
        for (const auto& v : stack) {
          str += std::to_string(v) + " ";
        }
        return str;
      }
    };

    bool step(machine_t& machine, const code_t& code) {
      DEBUG_LOGGER_EXEC("\t%s", machine.show().c_str());
      auto cmd = static_cast<instruction_rpn_t>(code.read_u8(machine.rip));
      DEBUG_LOGGER_EXEC("\tcmd: %s", code_t::show_instruction(cmd).c_str());

      if (machine.stack.size() > 1000)
        throw fatal_error_t("too big stack");

      switch (cmd) {
        case instruction_rpn_t::exit: {
          return false;

        } case instruction_rpn_t::arg: {
          int64_t value = code.read_i64(machine.rip);
          DEBUG_LOGGER_EXEC("value: %d", value);
          machine.stack.push_back(machine.stack.at(machine.stack.size() - value));
          DEBUG_LOGGER_EXEC("arg: %d", machine.stack.back());
          break;

        } case instruction_rpn_t::call: {
          if (machine.stack.empty())
            throw fatal_error_t("invalid stack");

          int64_t arg_count = machine.stack.back();
          DEBUG_LOGGER_EXEC("arg_count: %d", arg_count);
          if (arg_count < 1 || arg_count > machine.stack.size())
            throw fatal_error_t("invalid stack");

          for (size_t i{}; i < arg_count - 1; ++i) {
            int64_t arg = machine.stack.at(machine.stack.size() - 1 - 1/*arg_count*/ - i);
            DEBUG_LOGGER_EXEC("arg: %d", arg);
          }

          int64_t rip = machine.stack.at(machine.stack.size() - 1 - arg_count);
          DEBUG_LOGGER_EXEC("rip: %d", rip);

          machine.stack.push_back(machine.rbp);
          machine.stack.push_back(machine.rip);
          machine.rbp = machine.stack.size() - 1/*rbp*/ - 1/*rip*/;
          machine.rip = rip;
          break;

         } case instruction_rpn_t::jmp: {
          int64_t rip = code.read_i64(machine.rip);
          DEBUG_LOGGER_EXEC("rip: %d", rip);
          machine.rip = rip;
          break;

        } case instruction_rpn_t::pop_jif: {
          if (machine.stack.empty())
            throw fatal_error_t("invalid stack");

          int64_t ret = machine.stack.back();
          machine.stack.pop_back();
          DEBUG_LOGGER_EXEC("ret: %d", ret);

          int64_t rip = code.read_i64(machine.rip);
          DEBUG_LOGGER_EXEC("rip: %d", rip);

          if (!ret) {
            machine.rip = rip;
            DEBUG_LOGGER_EXEC("rip: %d", rip);
          }

          break;

        } case instruction_rpn_t::push8: {
          int64_t value = code.read_i64(machine.rip);
          machine.stack.push_back(value);
          DEBUG_LOGGER_EXEC("value: %d", value);
          break;

        } case instruction_rpn_t::ret: {
          if (machine.stack.size() < 4)
            throw fatal_error_t("invalid stack");

          int64_t ret = machine.stack.back();
          machine.stack.pop_back();

          machine.rip = machine.stack.back();
          machine.stack.pop_back();

          machine.rbp = machine.stack.back();
          machine.stack.pop_back();

          int64_t arg_count = machine.stack.back();
          machine.stack.pop_back();

          if (machine.stack.size() < arg_count)
            throw fatal_error_t("invalid stack");

          for (size_t i{}; i < arg_count; ++i) {
            machine.stack.pop_back();
          }

          machine.stack.push_back(ret);
          break;

        } case instruction_rpn_t::syscall: {
          if (machine.stack.empty())
            throw fatal_error_t("invalid stack");

          int64_t arg_count = machine.stack.back();
          machine.stack.pop_back();
          DEBUG_LOGGER_EXEC("arg_count: %d", arg_count);
          if (arg_count > machine.stack.size())
            throw fatal_error_t("invalid stack");




          int64_t ret = -1;
          int64_t op  = -1;
          if (arg_count > 0) {
            op = machine.stack.back();
            machine.stack.pop_back();
            arg_count--;

            static std::vector<std::function<int64_t(int64_t, int64_t)>> ops2 = {
              /*200*/ [](int64_t a, int64_t b) { return a + b; },
              /*201*/ [](int64_t a, int64_t b) { return a - b; },
              /*202*/ [](int64_t a, int64_t b) { return a * b; },
              /*203*/ [](int64_t a, int64_t b) { return !b ? 0 : a / b; },
              /*204*/ [](int64_t a, int64_t b) { return a == b; },
              /*205*/ [](int64_t a, int64_t b) { return a < b; },
              /*206*/ [](int64_t a, int64_t b) { return a && b; },
              /*207*/ [](int64_t a, int64_t b) { return a || b; },
            };

            if (arg_count == 1 && op == 100) {
              int64_t opnd1 = machine.stack.back();
              machine.stack.pop_back();
              arg_count--;
              ret = !opnd1;
              DEBUG_LOGGER_EXEC("%d: %d => %d", op, opnd1, ret);

            } else if (arg_count == 2 && op >= 200 && op < 200 + ops2.size()) {
              int64_t opnd1 = machine.stack.back();
              machine.stack.pop_back();
              arg_count--;
              int64_t opnd2 = machine.stack.back();
              machine.stack.pop_back();
              arg_count--;

              ret = ops2[op - 200](opnd1, opnd2);
              DEBUG_LOGGER_EXEC("%d: %d %d => %d", op, opnd1, opnd2, ret);
            }
          }

          if (arg_count) {
            DEBUG_LOGGER_EXEC("invalid syscall: %d : %d", op, arg_count);
            for (size_t i{}; i < arg_count; ++i) {
              int64_t arg = machine.stack.back();
              machine.stack.pop_back();
              DEBUG_LOGGER_EXEC("arg: %d", arg);
            }
          }

          machine.stack.push_back(ret);
          break;

        } default: throw fatal_error_t("unknown cmd");
      }
      return true;
    }

    void process(const code_ctx_t& code_ctx) {
      DEBUG_LOGGER_TRACE_EXEC;

      machine_t machine;
      machine.rip = code_ctx.rip;
      machine.rbp = {};

      while (step(machine, code_ctx.code)) {
      }

      if (machine.stack.size() == 1) {
        std::cout << machine.stack.front() << std::endl;
      }
    }
  }
#endif
}

struct interpreter_t {

  void exec(const std::string code) {
    using namespace aml_n;

    auto tokens = lexical_analyzer_n::process(code);
    DEBUG_LOGGER_LA("tokens: \n%s", aml::token_n::show_tokens(tokens).c_str());

    // auto syntax_lisp_tree = syntax_lisp_analyzer_n::process(tokens);
    // DEBUG_LOGGER_SA("syntax_lisp_tree: \n%s", syntax_lisp_tree.show({}).c_str());

    // auto stmt = syntax_analyzer_n::process(syntax_lisp_tree);
    // DEBUG_LOGGER_SA("syntax_tree: \n%s", stmt->show({}).c_str());

    // auto code_ctx = intermediate_code_generator_n::process(stmt);
    // DEBUG_LOGGER_ICG("intermediate_code: \n%s", code_ctx.code.show().c_str());

    // aml::utils_n::data_t text;
    // code_generator_n::process(text, instructions);

    // executor_n::process(code_ctx);
  }
};



int main(int argc, char* argv[]) {
  std::string code;

  if (argc < 2) {
    std::cerr << "usage: ./<program> <code>" << std::endl;
    return 1;
  }

  if (argc > 1) {
    code = argv[1];
  }

  // std::cout << "code: '\n" << code << "'" << std::endl;

  interpreter_t interpreter;
  interpreter.exec(code);

  return 0;
}

