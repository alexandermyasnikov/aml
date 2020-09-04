#pragma once

#include <deque>
#include <memory>
#include "utils.h"
#include "code_segment.h"
#include "lisp_tree.h"
#include "env.h"

namespace aml::stmt_n {
  namespace utils_n = aml::utils_n;
  namespace code_n = aml::code_n;
  namespace env_n = aml::env_n;
  namespace lisp_tree_n = aml::lisp_tree_n;



  struct syntax_error_t : utils_n::fatal_error_t {
    syntax_error_t(const token_n::token_t& token, token_n::type_t type = token_n::type_t::unknown);
  };



  void check_not_leaf(const lisp_tree_n::lisp_tree_t& tree) {
    if (tree.is_leaf())
      throw syntax_error_t(tree.node);
  }

  void check_size_eq(const lisp_tree_n::lisp_tree_t& tree, size_t count) {
    if (tree.nodes.size() != count)
      throw syntax_error_t(tree.node);
  }

  void check_size_gt(const lisp_tree_n::lisp_tree_t& tree, size_t count) {
    if (tree.nodes.size() < count)
      throw syntax_error_t(tree.node);
  }

  void check_type(const lisp_tree_n::lisp_tree_t& tree, token_n::type_t type) {
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
    virtual void intermediate_code(code_n::code_ctx_t& code_ctx) const = 0;
  };



  struct stmt_program_t : stmt_t {
    using funcs_t = std::deque<std::shared_ptr<stmt_defn_t>>;
    using body_t  = std::shared_ptr<stmt_expr_t>;
    using env_t   = std::shared_ptr<env_n::env_t>;

    env_t   env;
    funcs_t funcs;
    body_t  body;

    stmt_program_t(const lisp_tree_n::lisp_tree_t& tree);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_arg_t : stmt_t {
    int64_t value = {};

    stmt_arg_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_block_t : stmt_t {
    std::deque<std::shared_ptr<stmt_expr_t>> exprs;

    stmt_block_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_call_t : stmt_t {
    std::shared_ptr<stmt_expr_t>              name;
    std::vector<std::shared_ptr<stmt_expr_t>> args;

    stmt_call_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_defn_t : stmt_t {
    env_n::var_info_sptr_t var;
    std::shared_ptr<stmt_expr_t> body;
    env_n::env_sptr_t            env;

    stmt_defn_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_defvar_t : stmt_t {
    env_n::var_info_sptr_t var;
    std::shared_ptr<stmt_expr_t> body;
    env_n::env_sptr_t            env;

    stmt_defvar_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
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

    stmt_expr_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_func_t : stmt_t {
    env_n::var_info_sptr_t var;
    env_n::env_sptr_t      env;

    stmt_func_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_if_t : stmt_t {
    std::shared_ptr<stmt_expr_t> expr_if;
    std::shared_ptr<stmt_expr_t> expr_then;
    std::shared_ptr<stmt_expr_t> expr_else;

    stmt_if_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_int_t : stmt_t {
    int64_t value = {};

    stmt_int_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_syscall_t : stmt_t {
    std::vector<std::shared_ptr<stmt_expr_t>> args;

    stmt_syscall_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };



  struct stmt_var_t : stmt_t {
    env_n::var_info_sptr_t var;

    stmt_var_t(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env);
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
  };
}
