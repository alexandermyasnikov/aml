#include "stmt.h"

#include <filesystem>
#include "logger.h"

namespace aml::stmt_n {

  syntax_error_t::syntax_error_t(const token_n::token_t& token, token_n::type_t type)
    : utils_n::fatal_error_t("synax error at or near '" + token.lexeme + "' at '" + token.pos.show() + "'"
        + (token_n::token_t{.type = type}.is_primary() ? ", expected '" + token_n::token_t{.type = type}.show() + "'" : "")) { }



  std::shared_ptr<stmt_t> stmt_t::factory(type_t type) {
    switch (type) {
      case type_t::stmt_stub:    return std::make_shared<stmt_stub_t>();    break;
      case type_t::stmt_program: return std::make_shared<stmt_program_t>(); break;
      case type_t::stmt_arg:     return std::make_shared<stmt_arg_t>();     break;
      case type_t::stmt_call:    return std::make_shared<stmt_call_t>();    break;
      // case type_t::stmt_block:   return std::make_shared<stmt_block_t>();   break;
      case type_t::stmt_defn:    return std::make_shared<stmt_defn_t>();    break;
      // case type_t::stmt_defvar:  return std::make_shared<stmt_defvar_t>();  break;
      case type_t::stmt_func:    return std::make_shared<stmt_func_t>();    break;
      case type_t::stmt_if:      return std::make_shared<stmt_if_t>();      break;
      case type_t::stmt_include: return std::make_shared<stmt_include_t>(); break;
      case type_t::stmt_int:     return std::make_shared<stmt_int_t>();     break;
      case type_t::stmt_syscall: return std::make_shared<stmt_syscall_t>(); break;
      // case type_t::stmt_var:     return std::make_shared<stmt_var_t>();     break;
      default:                   throw utils_n::fatal_error_t("unknown type_t " + std::to_string(static_cast<size_t>(type)));
    }
  }

  std::shared_ptr<stmt_t> stmt_t::parse(const lisp_tree_n::lisp_tree_t& tree,
      env_n::env_sptr_t env, const types_t& types, options_t& options) {

    for (const auto type : types) {
      auto stmt = factory(type);
      if (stmt->parse_v(tree, env, options))
        return stmt;
    }

    throw syntax_error_t(tree.node);
  }



  bool stmt_stub_t::parse_v(const lisp_tree_n::lisp_tree_t& /*tree*/, env_n::env_sptr_t /*env*/, options_t& /*options*/) {
    return false;
  }

  std::string stmt_stub_t::show(size_t /*deep*/) const {
    return "";
  }

  void stmt_stub_t::intermediate_code(code_n::code_ctx_t& /*code_ctx*/) const {
  }



  bool stmt_program_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) {
    if (tree.is_leaf()) return false;

    env = env ? env : std::make_shared<env_n::env_t>(env);
    this->env = env;
    body = factory(type_t::stmt_stub);

    for (const auto& node : tree.nodes) {
      auto stmt = parse(node, env, types_program, options);
      switch (stmt->type()) {
        case type_t::stmt_defn:      funcs.push_back(stmt); break;
        case type_t::stmt_call:      body = stmt;           break; // TODO check
        case type_t::stmt_include:
        {
          auto stmt_include = std::dynamic_pointer_cast<stmt_include_t>(stmt);
          if (!stmt_include) break;
          auto stmt_program = std::dynamic_pointer_cast<stmt_program_t>(stmt_include->body);
          if (!stmt_program) break;
          funcs.insert(funcs.end(), stmt_program->funcs.begin(), stmt_program->funcs.end());
          break;
        }
        default: throw syntax_error_t(node.node);
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
    code_ctx.code.write_cmd(code_n::instruction_rpn_t::exit);
  }



  bool stmt_arg_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t /*env*/, options_t& /*options*/) {
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
    code_ctx.code.write_cmd(code_n::instruction_rpn_t::arg, code_ctx.rsp + value + 1/*rbp*/ + 1/*rip*/ + 1);
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



  bool stmt_call_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() < 2) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_call)) return false;

    name = parse(tree.nodes[1], env, types_expr, options);
    for (const auto& node : tree.nodes | std::views::drop(2)) {
      auto stmt = parse(node, env, types_expr, options);
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
    code_ctx.code.write_cmd(code_n::instruction_rpn_t::push, args.size() + 1);
    code_ctx.rsp++;

    code_ctx.code.write_cmd(code_n::instruction_rpn_t::call);
    code_ctx.rsp -= 1/*<count>*/ + (args.size() + 1);
    code_ctx.rsp += 1/*<return>*/;
  }



  bool stmt_defn_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() != 3) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_defn)) return false;
    if (!check_type(tree.nodes[1], token_n::type_t::ident)) return false;

    this->env = env;
    var = env->def_func(std::get<std::string>(tree.nodes[1].node.value));
    body = parse(tree.nodes[2], env, types_expr, options);
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
    code_ctx.code.write_cmd(code_n::instruction_rpn_t::ret);
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



  bool stmt_func_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& /*options*/) {
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
    code_ctx.code.write_cmd(code_n::instruction_rpn_t::push, var->offset);
    code_ctx.rsp++;
  }



  bool stmt_if_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() != 4) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_if)) return false;

    expr_if   = parse(tree.nodes[1], env, types_expr, options);
    expr_then = parse(tree.nodes[2], env, types_expr, options);
    expr_else = parse(tree.nodes[3], env, types_expr, options);
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

    size_t m1 = code_ctx.code.buffer.size() + sizeof(uint8_t);
    code_ctx.code.write_cmd((code_n::instruction_rpn_t::pop_jif), {});
    code_ctx.rsp--;

    expr_then->intermediate_code(code_ctx);

    size_t m2 = code_ctx.code.buffer.size() + sizeof(uint8_t);
    code_ctx.code.write_cmd(code_n::instruction_rpn_t::jmp, {});
    code_ctx.rsp--;

    code_ctx.code.write_i64(code_ctx.code.buffer.size(), m1);

    expr_else->intermediate_code(code_ctx);

    code_ctx.code.write_i64(code_ctx.code.buffer.size(), m2);
  }



  bool stmt_include_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() != 2) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_include)) return false;
    if (!check_type(tree.nodes[1], token_n::type_t::dq_string)) return false;

    filename = std::get<std::string>(tree.nodes[1].node.value);
    body = factory(type_t::stmt_stub);


    auto filename_abs = std::filesystem::path(options.wd) / filename;
    AML_LOGGER(debug, "filename_abs: {}", filename_abs.string());

    if (!options.files.contains(filename_abs)) {
      options.files.insert(filename_abs);
      auto wd = filename_abs.parent_path().string();

      std::swap(wd, options.wd);
      body = parse_file(env, options, filename_abs);
      std::swap(wd, options.wd);
    }

    return true;
  }

  std::string stmt_include_t::show(size_t /*deep*/) const {
    std::string str;
    str += token_n::token_t{.type = token_n::type_t::lp}.show();
    str += token_n::token_t{.type = token_n::type_t::key_include}.show();
    str += token_n::token_t{.type = token_n::type_t::whitespace}.show();
    str += '"' + filename + '"';
    str += token_n::token_t{.type = token_n::type_t::rp}.show();
    return str;
  }

  void stmt_include_t::intermediate_code(code_n::code_ctx_t& /*code_ctx*/) const {
  }

  std::shared_ptr<stmt_t> stmt_include_t::parse_file(env_n::env_sptr_t env, options_t& options, const std::string& filename) {
    std::string code = utils_n::str_from_file(filename);
    AML_LOGGER(debug, "filename: {}", filename);
    AML_LOGGER(debug, "code: \n{}", code);
    if (code.empty())
      return factory(type_t::stmt_stub);

    auto tokens  = token_n::process(code);
    auto tree    = lisp_tree_n::process(tokens);
    auto stmt    = parse(tree, env, {type_t::stmt_program}, options);
    return stmt;
  }



  bool stmt_int_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t /*env*/, options_t& /*options*/) {
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
    code_ctx.code.write_cmd(code_n::instruction_rpn_t::push, value);
    code_ctx.rsp++;
  }



  bool stmt_syscall_t::parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) {
    if (tree.is_leaf()) return false;
    if (tree.nodes.size() < 3) return false;

    if (!check_type(tree.nodes[0], token_n::type_t::key_syscall)) return false;

    for (const auto& node : tree.nodes | std::views::drop(1)) {
      auto stmt = parse(node, env, types_expr, options);
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
    code_ctx.code.write_cmd(code_n::instruction_rpn_t::push, static_cast<int64_t>(args.size()));
    code_ctx.rsp++;

    code_ctx.code.write_cmd(code_n::instruction_rpn_t::syscall);
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
