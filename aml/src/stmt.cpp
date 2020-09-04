#include "stmt.h"

namespace aml::stmt_n {
  namespace utils_n = aml::utils_n;
  namespace token_n = aml::token_n;
  namespace code_n = aml::code_n;
  namespace env_n = aml::env_n;



  syntax_error_t::syntax_error_t(const token_n::token_t& token, token_n::type_t type)
    : utils_n::fatal_error_t("synax error at or near '" + token.lexeme + "' at '" + token.pos.show() + "'"
        + (token_n::token_t{.type = type}.is_primary() ? ", expected '" + token_n::token_t{.type = type}.show() + "'" : "")) { }



  stmt_program_t::stmt_program_t(const lisp_tree_n::lisp_tree_t& tree) {
    check_not_leaf(tree);
    check_size_gt(tree, 1);

    env = std::make_shared<env_n::env_t>();

    for (size_t i{}; i < tree.nodes.size() - 1; ++i) {
      funcs.push_back(std::make_shared<stmt_defn_t>(tree.nodes[i], env));
    }
    body = std::make_shared<stmt_expr_t>(tree.nodes.back(), env);

    // DEBUG_LOGGER_SA("env: \n%s", env->show().c_str());
  }

  std::string stmt_program_t::show(size_t deep) const {
    std::string str;
    for (const auto& func : funcs) {
      str += func->show(deep + 1);
      str += token_n::token_t{.type = token_n::type_t::new_line}.show();
      str += token_n::token_t{.type = token_n::type_t::new_line}.show();
    }
    str += body->show(deep + 1);
    str += token_n::token_t{.type = token_n::type_t::new_line}.show();
    str += token_n::token_t{.type = token_n::type_t::new_line}.show();
    return str;
  }

  void stmt_program_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    for (const auto& func : funcs) {
      func->intermediate_code(code_ctx);
    }
    code_ctx.rip = code_ctx.code.buffer.size();
    // DEBUG_LOGGER_ICG("rip start: %d", code_ctx.rip);
    body->intermediate_code(code_ctx);
    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::exit);
  }



  stmt_arg_t::stmt_arg_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_arg);
    check_type(tree.nodes[1], token_n::type_t::integer);

    value = std::get<int64_t>(tree.nodes[1].node.value);
  }

  std::string stmt_arg_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_arg}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += std::to_string(value);
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_arg_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::arg);
    code_ctx.code.write_i64(code_ctx.rsp + value + 1/*rbp*/ + 1/*rip*/ + 1);
    code_ctx.rsp++;
  }



  stmt_block_t::stmt_block_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_block);

    auto env_block = std::make_shared<env_n::env_t>(env);

    for (const auto& node : tree.nodes) {
      exprs.push_back(std::make_shared<stmt_expr_t>(node, env_block));
    }
  }

  std::string stmt_block_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_block}.show();
    for (const auto& expr : exprs) {
      str += token_n::token_t{.type = token_n::type_t::new_line}.show();
      str += utils_n::indent(deep);
      str += expr->show(deep + 1);
    }
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_block_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    throw utils_n::fatal_error_t("TODO block");
  }



  stmt_call_t::stmt_call_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_call);

    name = std::make_shared<stmt_expr_t>(tree.nodes[1], env);
    for (size_t i = 2; i < tree.nodes.size(); ++i) {
      args.push_back(std::make_shared<stmt_expr_t>(tree.nodes[i], env));
    }
  }

  std::string stmt_call_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_call}.show();
    str += token_n::token_t{.type = token_n::type_t::new_line}.show();
    str += utils_n::indent(deep);
    str += name->show(deep + 1);
    for (const auto& arg : args) {
      str += token_n::token_t{.type = token_n::type_t::new_line}.show();
      str += utils_n::indent(deep);
      str += arg->show(deep + 1);
    }
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_call_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    name->intermediate_code(code_ctx);

    for (const auto& arg : args | std::views::reverse) {
      arg->intermediate_code(code_ctx);
    }
    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::push8);
    code_ctx.code.write_i64(args.size() + 1);
    code_ctx.rsp++;

    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::call);
    code_ctx.rsp -= 1/*<count>*/ + (args.size() + 1);
    code_ctx.rsp += 1/*<return>*/;
  }



  stmt_defvar_t::stmt_defvar_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_var);
    check_type(tree.nodes[1], token_n::type_t::ident);

    var = env->def_var(std::get<std::string>(tree.nodes[1].node.value));
    body = std::make_shared<stmt_expr_t>(tree.nodes[2], env);
    // env TODO
  }

  std::string stmt_defvar_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_var}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += var->name;
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_defvar_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    throw utils_n::fatal_error_t("TODO defvar");
  }



  stmt_expr_t::stmt_expr_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 1);

    switch (tree.nodes[0].node.type) {
      case token_n::type_t::key_arg:     expr = std::make_shared<stmt_arg_t>(tree, env);     break;
      case token_n::type_t::key_block:   expr = std::make_shared<stmt_block_t>(tree, env);   break;
      case token_n::type_t::key_call:    expr = std::make_shared<stmt_call_t>(tree, env);    break;
      case token_n::type_t::key_defvar:  expr = std::make_shared<stmt_defvar_t>(tree, env);  break;
      case token_n::type_t::key_func:    expr = std::make_shared<stmt_func_t>(tree, env);    break;
      case token_n::type_t::key_if:      expr = std::make_shared<stmt_if_t>(tree, env);      break;
      case token_n::type_t::key_int:     expr = std::make_shared<stmt_int_t>(tree, env);     break;
      case token_n::type_t::key_syscall: expr = std::make_shared<stmt_syscall_t>(tree, env); break;
      case token_n::type_t::key_var:     expr = std::make_shared<stmt_var_t>(tree, env);     break;
      default: throw syntax_error_t(tree.nodes[0].node);
    }
  }

  std::string stmt_expr_t::show(size_t deep) const {
    std::string str;
    std::visit(utils_n::overloaded{[&str, deep] (const auto &expr) { str = expr->show(deep); } }, expr);
    return str;
  }

  void stmt_expr_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    std::visit(utils_n::overloaded{[&code_ctx] (const auto &expr) { expr->intermediate_code(code_ctx); } }, expr);
  }



  stmt_func_t::stmt_func_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_eq(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_func);
    check_type(tree.nodes[1], token_n::type_t::ident);

    var = env->get_func(std::get<std::string>(tree.nodes[1].node.value));
  }

  std::string stmt_func_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_func}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += var->name;
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_func_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::push8);
    code_ctx.code.write_i64(var->offset);
    code_ctx.rsp++;
  }



  stmt_if_t::stmt_if_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_eq(tree, 4);
    check_type(tree.nodes[0], token_n::type_t::key_if);

    expr_if   = std::make_shared<stmt_expr_t>(tree.nodes[1], env);
    expr_then = std::make_shared<stmt_expr_t>(tree.nodes[2], env);
    expr_else = std::make_shared<stmt_expr_t>(tree.nodes[3], env);
  }

  std::string stmt_if_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_if}.show();
    str += token_n::token_t{.type = token_n::type_t::new_line}.show();
    str += utils_n::indent(deep);
    str += expr_if->show(deep + 1);
    str += token_n::token_t{.type = token_n::type_t::new_line}.show();
    str += utils_n::indent(deep);
    str += expr_then->show(deep + 1);
    str += token_n::token_t{.type = token_n::type_t::new_line}.show();
    str += utils_n::indent(deep);
    str += expr_else->show(deep + 1);
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_if_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    expr_if->intermediate_code(code_ctx);

    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::pop_jif);
    code_ctx.rsp--;
    size_t m1 = code_ctx.code.buffer.size();
    code_ctx.code.write_i64(-1);

    expr_then->intermediate_code(code_ctx);

    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::jmp);
    code_ctx.rsp--;
    size_t m2 = code_ctx.code.buffer.size();
    code_ctx.code.write_i64(-1);

    code_ctx.code.write_i64(code_ctx.code.buffer.size(), m1);

    expr_else->intermediate_code(code_ctx);

    code_ctx.code.write_i64(code_ctx.code.buffer.size(), m2);
  }



  stmt_int_t::stmt_int_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_int);
    check_type(tree.nodes[1], token_n::type_t::integer);

    value = std::get<int64_t>(tree.nodes[1].node.value);
  }

  std::string stmt_int_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_int}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += std::to_string(value);
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_int_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::push8);
    code_ctx.code.write_i64(value);
    code_ctx.rsp++;
  }



  stmt_defn_t::stmt_defn_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_eq(tree, 3);
    check_type(tree.nodes[0], token_n::type_t::key_defn);
    check_type(tree.nodes[1], token_n::type_t::ident);

    this->env = env;
    var = env->def_func(std::get<std::string>(tree.nodes[1].node.value));
    body = std::make_shared<stmt_expr_t>(tree.nodes[2], env);
    // DEBUG_LOGGER_SA("env: \n%s", this->env->show().c_str());
  }

  std::string stmt_defn_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_defn}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += var->name;
    str += token_n::token_t{.type = token_n::type_t::new_line}.show();
    str += utils_n::indent(deep);
    str += body->show(deep + 1);
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_defn_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    var->offset = code_ctx.code.buffer.size();
    // DEBUG_LOGGER_ICG("name: %s \t %d", var->name.c_str(), var->offset);
    body->intermediate_code(code_ctx);
    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::ret);
    code_ctx.rsp = {};
  }



  stmt_syscall_t::stmt_syscall_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_syscall);

    for (const auto& node : tree.nodes | std::views::drop(1)) {
      args.push_back(std::make_shared<stmt_expr_t>(node, env));
    }
  }

  std::string stmt_syscall_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_syscall}.show();
    for (const auto& arg : args) {
      str += token_n::token_t{.type = token_n::type_t::new_line}.show();
      str += utils_n::indent(deep);
      str += arg->show(deep + 1);
    }
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_syscall_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    for (const auto& arg : args | std::views::reverse) {
      arg->intermediate_code(code_ctx);
    }
    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::push8);
    code_ctx.code.write_i64(args.size());
    code_ctx.rsp++;

    code_ctx.code.write_u8((uint8_t) code_n::instruction_rpn_t::syscall);
    code_ctx.rsp -= 1/*<count>*/ + args.size();
    code_ctx.rsp += 1/*<return>*/;
    code_ctx.rsp += 1/*<rbp>*/ + 1/*<rip>*/;
  }



  stmt_var_t::stmt_var_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_var);
    check_type(tree.nodes[1], token_n::type_t::ident);

    var = env->def_var(std::get<std::string>(tree.nodes[1].node.value));
  }

  std::string stmt_var_t::show(size_t deep) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_var}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += var->name;
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_var_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    throw utils_n::fatal_error_t("TODO var");
  }
}
