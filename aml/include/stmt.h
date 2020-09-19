#pragma once

#include <deque>
#include <memory>
#include <set>

#include "code_segment.h"
#include "env.h"
#include "lisp_tree.h"
#include "utils.h"

namespace aml::stmt_n {
  namespace utils_n = aml::utils_n;
  namespace code_n = aml::code_n;
  namespace env_n = aml::env_n;
  namespace lisp_tree_n = aml::lisp_tree_n;



  struct syntax_error_t : utils_n::fatal_error_t {
    syntax_error_t(const token_n::token_t& token, token_n::type_t type = token_n::type_t::unknown);
  };



  static inline bool check_type(const lisp_tree_n::lisp_tree_t& tree, token_n::type_t type) {
    return tree.is_leaf() && tree.node.type == type;
  }



  // GRAMMAR
  // program: include*   func*      call
  // expr:    ARG        <digit>
  // expr:    BLOCK      expr+
  // expr:    CALL       expr       expr*
  // expr:    DEFVAR     <name>     expr
  // expr:    IF         expr       expr expr
  // expr:    INT        <digit>
  // expr:    SYSCALL    expr+
  // expr:    VAR        <name>
  // func:    DEFN       expr       expr
  // include: #INCLUDE   <dq_str>



  enum class type_t {
    unknown,
    stmt_stub,
    stmt_program,
    stmt_arg,
    stmt_block,
    stmt_call,
    stmt_defn,
    stmt_defvar,
    stmt_func,
    stmt_if,
    stmt_include,
    stmt_int,
    stmt_syscall,
    stmt_var,
  };

  using types_t = std::vector<type_t>;



  static inline types_t types_program = {
    type_t::stmt_include,
    type_t::stmt_defn,
    type_t::stmt_call,
  };

  static inline types_t types_expr = {
    type_t::stmt_arg,
    type_t::stmt_block,
    type_t::stmt_call,
    // type_t::stmt_defvar,
    type_t::stmt_func,
    type_t::stmt_if,
    type_t::stmt_int,
    type_t::stmt_syscall,
    // type_t::stmt_var,
  };



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



  struct options_t {
    std::string           filename = {};
    std::set<std::string> files    = {};
  };



  struct stmt_t {
    virtual ~stmt_t() { }
    virtual bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) = 0;
    virtual std::string show(size_t deep) const = 0;
    virtual void intermediate_code(code_n::code_ctx_t& code_ctx) const = 0;
    virtual type_t type() const = 0;

    static std::shared_ptr<stmt_t> factory(type_t type);
    static std::shared_ptr<stmt_t> parse(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, const types_t& types, options_t& options);
  };



  struct stmt_stub_t : stmt_t {
    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_stub; }
  };



  struct stmt_program_t : stmt_t {
    using funcs_t = std::deque<std::shared_ptr<stmt_t>>;
    using body_t  = std::shared_ptr<stmt_t>;
    using env_t   = std::shared_ptr<env_n::env_t>;

    env_t   env;
    funcs_t funcs;
    body_t  body;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_program; }
  };



  struct stmt_arg_t : stmt_t {
    int64_t value = {};

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_arg; }
  };



  struct stmt_block_t : stmt_t {
    std::deque<std::shared_ptr<stmt_t>> args;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_block; }
  };



  struct stmt_call_t : stmt_t {
    std::shared_ptr<stmt_t>             name;
    std::deque<std::shared_ptr<stmt_t>> args;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_call; }
  };



  struct stmt_defn_t : stmt_t {
    env_n::var_info_sptr_t  var;
    std::shared_ptr<stmt_t> body;
    env_n::env_sptr_t       env;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_defn; }
  };



#if 0
  struct stmt_defvar_t : stmt_t {
    env_n::var_info_sptr_t  var;
    std::shared_ptr<stmt_t> body;
    env_n::env_sptr_t       env;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_defvar; }
  };
#endif



  struct stmt_func_t : stmt_t {
    env_n::var_info_sptr_t var;
    env_n::env_sptr_t      env;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_func; }
  };



  struct stmt_if_t : stmt_t {
    std::shared_ptr<stmt_t> expr_if;
    std::shared_ptr<stmt_t> expr_then;
    std::shared_ptr<stmt_t> expr_else;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_if; }
  };



  struct stmt_include_t : stmt_t {
    std::string             filename;
    std::shared_ptr<stmt_t> body;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_include; }
    std::shared_ptr<stmt_t> parse_file(env_n::env_sptr_t env, options_t& options, const std::string& filename);
  };



  struct stmt_int_t : stmt_t {
    int64_t value = {};

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_int; }
  };



  struct stmt_syscall_t : stmt_t {
    std::vector<std::shared_ptr<stmt_t>> args;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_syscall; }
  };



#if 0
  struct stmt_var_t : stmt_t {
    env_n::var_info_sptr_t var;

    bool parse_v(const lisp_tree_n::lisp_tree_t& tree, env_n::env_sptr_t env, options_t& options) override;
    std::string show(size_t deep) const override;
    void intermediate_code(code_n::code_ctx_t& code_ctx) const override;
    type_t type() const override { return type_t::stmt_var; }
  };
#endif
}
