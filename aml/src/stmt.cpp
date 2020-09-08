#include "stmt.h"

namespace aml::stmt_n {
  namespace utils_n = aml::utils_n;
  namespace token_n = aml::token_n;
  namespace code_n = aml::code_n;
  namespace env_n = aml::env_n;



  syntax_error_t::syntax_error_t(const token_n::token_t& token, token_n::type_t type)
    : utils_n::fatal_error_t("synax error at or near '" + token.lexeme + "' at '" + token.pos.show() + "'"
        + (token_n::token_t{.type = type}.is_primary() ? ", expected '" + token_n::token_t{.type = type}.show() + "'" : "")) { }



  std::shared_ptr<stmt_t> stmt_t::factory(type_t type) {
    switch (type) {
      case type_t::stmt_program: return std::make_shared<stmt_program_t>(); break;
      case type_t::stmt_arg:     return std::make_shared<stmt_arg_t>();     break;
      case type_t::stmt_call:    return std::make_shared<stmt_call_t>();    break;
      // case type_t::stmt_block:   return std::make_shared<stmt_block_t>();   break;
      case type_t::stmt_defn:    return std::make_shared<stmt_defn_t>();    break;
      // case type_t::stmt_defvar:  return std::make_shared<stmt_defvar_t>();  break;
      case type_t::stmt_func:    return std::make_shared<stmt_func_t>();    break;
      case type_t::stmt_if:      return std::make_shared<stmt_if_t>();      break;
      case type_t::stmt_int:     return std::make_shared<stmt_int_t>();     break;
      case type_t::stmt_syscall: return std::make_shared<stmt_syscall_t>(); break;
      // case type_t::stmt_var:     return std::make_shared<stmt_var_t>();     break;
      default:                   throw utils_n::fatal_error_t("unknown type_t " + std::to_string(static_cast<size_t>(type)));
    }
  }

  std::shared_ptr<stmt_t> stmt_t::parse(const lisp_tree_n::lisp_tree_t& tree,
      env_n::env_sptr_t env, const types_t& types) {

    for (const auto type : types) {
      auto stmt = factory(type);
      if (stmt->parse_v(tree, env))
        return stmt;
    }

    throw syntax_error_t(tree.node);
  }



  bool stmt_program_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t /*env*/) {
    if (tree.is_leaf()) return false;

    env = std::make_shared<env_n::env_t>();

    for (const auto& node : tree.nodes) {
      auto stmt = parse(node, env, types_program);
      switch (stmt->type()) {
        case type_t::stmt_defn:      funcs.push_back(stmt); break;
        case type_t::stmt_call:      body = stmt;           break; // TODO check
        default:                     throw syntax_error_t(node.node);
      }
    }
    return true;
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
    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::exit));
  }



  bool stmt_arg_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t /*env*/) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() < 2) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_arg)) return false;
    if (!check_type(tree.nodes[1], token_n::type_t::integer)) return false;

    value = std::get<int64_t>(tree.nodes[1].node.value);
    return true;
  }

  std::string stmt_arg_t::show(size_t /*deep*/) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_arg}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += std::to_string(value);
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_arg_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::arg));
    code_ctx.code.write_i64(code_ctx.rsp + value + 1/*rbp*/ + 1/*rip*/ + 1);
    code_ctx.rsp++;
  }



#if 0
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

  void stmt_block_t::intermediate_code(code_n::code_ctx_t& /*code_ctx*/) const {
    throw utils_n::fatal_error_t("TODO block");
  }
#endif



  bool stmt_call_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() < 2) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_call)) return false;

    name = parse(tree.nodes[1], env, types_expr);
    for (const auto& node : tree.nodes | std::views::drop(2)) {
      auto stmt = parse(node, env, types_expr);
      args.push_back(stmt);
    }

    return true;
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
    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::push8));
    code_ctx.code.write_i64(args.size() + 1);
    code_ctx.rsp++;

    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::call));
    code_ctx.rsp -= 1/*<count>*/ + (args.size() + 1);
    code_ctx.rsp += 1/*<return>*/;
  }



  bool stmt_defn_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() != 3) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_defn)) return false;
    if (!check_type(tree.nodes[1], token_n::type_t::ident)) return false;

    this->env = env;
    var = env->def_func(std::get<std::string>(tree.nodes[1].node.value));
    body = parse(tree.nodes[2], env, types_expr);
    // DEBUG_LOGGER_SA("env: \n%s", this->env->show().c_str());
    return true;
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
    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::ret));
    code_ctx.rsp = {};
  }



#if 0
  stmt_defvar_t::stmt_defvar_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_var);
    check_type(tree.nodes[1], token_n::type_t::ident);

    var = env->def_var(std::get<std::string>(tree.nodes[1].node.value));
    body = std::make_shared<stmt_expr_t>(tree.nodes[2], env);
    // env TODO
  }

  std::string stmt_defvar_t::show(size_t /*deep*/) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_var}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += var->name;
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_defvar_t::intermediate_code(code_n::code_ctx_t& /*code_ctx*/) const {
    throw utils_n::fatal_error_t("TODO defvar");
  }
#endif



#if 0
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
#endif



  bool stmt_func_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() != 2) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_func)) return false;
    if (!check_type(tree.nodes[1], token_n::type_t::ident)) return false;

    var = env->get_func(std::get<std::string>(tree.nodes[1].node.value));

    return true;
  }

  std::string stmt_func_t::show(size_t /*deep*/) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_func}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += var->name;
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_func_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::push8));
    code_ctx.code.write_i64(var->offset);
    code_ctx.rsp++;
  }



  bool stmt_if_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() != 4) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_if)) return false;

    expr_if   = parse(tree.nodes[1], env, types_expr);
    expr_then = parse(tree.nodes[2], env, types_expr);
    expr_else = parse(tree.nodes[3], env, types_expr);
    return true;
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

    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::pop_jif));
    code_ctx.rsp--;
    size_t m1 = code_ctx.code.buffer.size();
    code_ctx.code.write_i64({});

    expr_then->intermediate_code(code_ctx);

    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::jmp));
    code_ctx.rsp--;
    size_t m2 = code_ctx.code.buffer.size();
    code_ctx.code.write_i64({});

    code_ctx.code.write_i64(code_ctx.code.buffer.size(), m1);

    expr_else->intermediate_code(code_ctx);

    code_ctx.code.write_i64(code_ctx.code.buffer.size(), m2);
  }



  bool stmt_int_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t /*env*/) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() != 2) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_int)) return false;
    if (!check_type(tree.nodes[1], token_n::type_t::integer)) return false;

    value = std::get<int64_t>(tree.nodes[1].node.value);
    return true;
  }

  std::string stmt_int_t::show(size_t /*deep*/) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_int}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += std::to_string(value);
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_int_t::intermediate_code(code_n::code_ctx_t& code_ctx) const {
    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::push8));
    code_ctx.code.write_i64(value);
    code_ctx.rsp++;
  }



  bool stmt_syscall_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() < 3) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_syscall)) return false;

    for (const auto& node : tree.nodes | std::views::drop(1)) {
      auto stmt = parse(node, env, types_expr);
      args.push_back(stmt);
    }

    return true;
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
    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::push8));
    code_ctx.code.write_i64(static_cast<int64_t>(args.size()));
    code_ctx.rsp++;

    code_ctx.code.write_u8(static_cast<uint8_t>(code_n::instruction_rpn_t::syscall));
    code_ctx.rsp -= 1/*<count>*/ + args.size();
    code_ctx.rsp += 1/*<return>*/;
    code_ctx.rsp += 1/*<rbp>*/ + 1/*<rip>*/;
  }



#if 0
  stmt_var_t::stmt_var_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env) {
    check_not_leaf(tree);
    check_size_gt(tree, 2);
    check_type(tree.nodes[0], token_n::type_t::key_var);
    check_type(tree.nodes[1], token_n::type_t::ident);

    var = env->def_var(std::get<std::string>(tree.nodes[1].node.value));
  }

  std::string stmt_var_t::show(size_t /*deep*/) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_var}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += var->name;
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_var_t::intermediate_code(code_n::code_ctx_t& /*code_ctx*/) const {
    throw utils_n::fatal_error_t("TODO var");
  }
#endif
}
