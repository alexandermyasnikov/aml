
#include <iostream>
#include <variant>
#include <iomanip>
#include <regex>

#include "debug_logger.h"

#define DEBUG_LOGGER_TRACE_LA            // DEBUG_LOGGER("la   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_LA(...)             // DEBUG_LOG("la   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_SA            DEBUG_LOGGER("sa   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_SA(...)             DEBUG_LOG("sa   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_ICG           DEBUG_LOGGER("icg  ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_ICG(...)            DEBUG_LOG("icg  ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_CG            DEBUG_LOGGER("cg   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_CG(...)             DEBUG_LOG("cg   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_EXEC          DEBUG_LOGGER("exec ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_EXEC(...)           DEBUG_LOG("exec ", logger_indent_aml_t::indent, __VA_ARGS__)

template <typename T>
struct logger_indent_t { static inline int indent = 0; };

struct logger_indent_aml_t : logger_indent_t<logger_indent_aml_t> { };



namespace aml_n {

  namespace utils_n {

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    static inline size_t column_start = 1;
    static inline size_t line_start = 1;

    static std::string indent(size_t count) {
      static size_t tab_size = 2;
      return std::string(count * tab_size, ' ');
    }

    struct fatal_error_t : std::runtime_error {
      fatal_error_t(const std::string& msg = "unknown error") : std::runtime_error(msg) { }
    };
  }



  namespace lexical_analyzer_n {

    using namespace utils_n;

    struct pos_t {
      size_t line   = line_start;
      size_t column = column_start;
      size_t length = 0;

      std::string show() const {
        return std::to_string(line)
          + ":" + std::to_string(column)
          + ":" + std::to_string(length);
      }
    };

    struct token_t {
      enum class type_t {
        unknown,
        new_line,
        whitespace,
        lp,
        rp,
        key_arg,
        key_block,
        key_call,
        key_defn,
        key_defvar,
        key_func,
        key_if,
        key_int,
        key_syscall,
        key_var,
        integer,
        ident,
        eof,
      };

      using value_t = std::variant<
        int64_t,
        std::string>;

      type_t      type = type_t::unknown;
      pos_t       pos  = { };
      std::string lexeme;
      value_t     value;

      bool is_primary() const {
        switch (type) {
          case type_t::unknown:
          case type_t::new_line:
          case type_t::whitespace:
          case type_t::eof:
            return false;
          default:
            return true;
        }
      }

      std::string show() const {
#if 0
        return lexeme;
#else
        switch (type) {
          case type_t::new_line:    return "\n";
          case type_t::whitespace:  return " ";
          case type_t::lp:          return "(";
          case type_t::rp:          return ")";
          case type_t::key_arg:     return "arg";
          case type_t::key_block:   return "block";
          case type_t::key_call:    return "call";
          case type_t::key_defn:    return "defn";
          case type_t::key_defvar:  return "defvar";
          case type_t::key_func:    return "func";
          case type_t::key_if:      return "if";
          case type_t::key_int:     return "int";
          case type_t::key_syscall: return "syscall";
          case type_t::key_var:     return "var";
          case type_t::integer:
            { const auto* p = std::get_if<int64_t>(&value);     return p ? std::to_string(*p) : "<integer>"; }
          case type_t::ident:
            { const auto* p = std::get_if<std::string>(&value); return p ? *p : "<ident>"; }
          case type_t::eof:         return "\0";
          default:                  return "(unknown)";
        }
#endif
      }

      struct rule_t {
        type_t        type;
        std::regex    regex;
        std::function<value_t(const std::string&)> get_value;
      };

      static inline std::vector<rule_t> rules = {
        {
          type_t::new_line,
          std::regex(R"(\n)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::whitespace,
          std::regex(R"([ \t]+)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::whitespace,
          std::regex(R"(;[^\n]*)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::lp,
          std::regex(R"(\()"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::rp,
          std::regex(R"(\))"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_arg,
          std::regex(R"(arg)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_block,
          std::regex(R"(block)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_call,
          std::regex(R"(call)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_defn,
          std::regex(R"(defn)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_defvar,
          std::regex(R"(defvar)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_func,
          std::regex(R"(func)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_if,
          std::regex(R"(if)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_int,
          std::regex(R"(int)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_syscall,
          std::regex(R"(syscall)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_var,
          std::regex(R"(var)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::integer,
          std::regex(R"([-+]?\d+)"),
          [](const std::string& lexeme) { return stol(lexeme); },
        }, {
          type_t::ident,
          std::regex(R"([\w<=>&|+\-\!:*^\/]+)"),
          [](const std::string& lexeme) { return lexeme; },
        },
      };

      static bool next(auto& it, auto ite, token_t& token) {
        if (it == ite) {
          token.pos.column += token.pos.length;
          token.pos.length  = 0;
          token.lexeme      = "\0";
          token.value       = {};
          token.type        = type_t::eof;
          return false;
        }

        std::smatch m;
        std::regex_constants::match_flag_type flags =
            std::regex_constants::match_continuous | std::regex_constants::match_not_null;

        const rule_t* match_rule = nullptr;
        std::smatch match_best;

        for (const auto& rule : rules) {
          if (!std::regex_search(it, ite, m, rule.regex, flags))
            continue;

          if (m.position())
            throw fatal_error_t("lexical_analyzer: unknown position at " + token.pos.show());

          if (m.length() > (!match_best.empty() ? match_best.length() : 0)) {
            match_best = m;
            match_rule = &rule;
          }
        }

        if (!match_rule) {
          return false;
        }

        it               += match_best.length();
        token.pos.column += token.pos.length;
        token.pos.length  = match_best.length();
        token.lexeme      = match_best.str();
        token.value       = match_rule->get_value(token.lexeme);
        token.type        = match_rule->type;

        if (match_rule->type == type_t::new_line) {
          token.pos.line++;
          token.pos.length = 0;
          token.pos.column = column_start;
        }

        return true;
      }
    };

    using tokens_t = std::deque<token_t>;

    tokens_t process(const std::string& code) {
      tokens_t tokens;

      auto it = code.begin();
      auto ite = code.end();
      token_t token;

      while (token_t::next(it, ite, token)) {
        DEBUG_LOGGER_LA("token: %zd \t '%s' \t '%s'", (size_t) token.type, token.pos.show().c_str(), token.lexeme.c_str());
        if (token.is_primary())
          tokens.push_back(token);
      }

      if (token.type != token_t::type_t::eof) {
        throw fatal_error_t("lexical_analyzer: expected eof at " + token.pos.show());
      }

      return tokens;
    }

    static std::string show_tokens(const tokens_t& tokens) {
      std::string str;
      for (const auto& token : tokens) {
        str += token.show() + " ";
      }
      return str;
    }
  }



  namespace syntax_lisp_analyzer_n {

    using namespace utils_n;
    using namespace lexical_analyzer_n;

    struct syntax_lisp_tree_t {
      using node_t  = token_t;
      using nodes_t = std::deque<syntax_lisp_tree_t>;

      node_t  node  = token_t{};
      nodes_t nodes = {};

      bool is_leaf() const {
        return node.type != token_t::type_t::unknown
          && node.type != token_t::type_t::lp
          && node.type != token_t::type_t::rp;
      }

      std::string show(size_t deep) const {
        std::string str;

        if (is_leaf()) {
          str += node.show();
        } else {
          str += node_t{.type = token_t::type_t::lp}.show();
          for (size_t i{}; i < nodes.size(); ++i) {
            const auto& node = nodes[i];
            if (i) {
              str += node_t{.type = token_t::type_t::new_line}.show();
              str += indent(deep);
            }
            str += node.show(deep + 1);
          }
          str += node_t{.type = token_t::type_t::rp}.show();
        }

        return str;
      }
    };

    syntax_lisp_tree_t process(const tokens_t& tokens) {
      std::stack<syntax_lisp_tree_t> stack;
      stack.push(syntax_lisp_tree_t{});
      token_t token_last;
      for (const auto& token : tokens) {
        token_last = token;

        if (token.type == token_t::type_t::lp) {
          stack.push(syntax_lisp_tree_t{.node = token});

        } else if (token.type == token_t::type_t::rp) {
          if (stack.size() < 2)
            throw fatal_error_t("syntax_lisp_analyzer: unexpected ')' at " + token.pos.show() + " " + token.lexeme);
          auto top = stack.top();
          stack.pop();
          stack.top().nodes.push_back(top);

        } else if (token.is_primary()) {
          stack.top().nodes.push_back(syntax_lisp_tree_t{.node = token});

        } else {
          throw fatal_error_t("syntax_lisp_analyzer: expected primary token");
        }
      }

      if (stack.size() != 1)
        throw fatal_error_t("syntax_lisp_analyzer: parse error at " + token_last.pos.show() + " " + token_last.lexeme);

      return stack.top();
    }
  }



  namespace syntax_analyzer_n {

    using namespace utils_n;
    using namespace syntax_lisp_analyzer_n;

    // GRAMMAR
    // program: func+
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

    struct var_info_t {
      size_t      id = {};
      std::string name;
      size_t      offset = {};

      std::string show() const {
        return std::to_string(id) + "\t'" + name + "'\t" + std::to_string(offset);
      }
    };

    struct env_t;
    using env_sptr_t = std::shared_ptr<env_t>;

    struct env_t : std::enable_shared_from_this<env_t> {
      using key_t = std::string;
      using val_t = std::shared_ptr<var_info_t>;
      using vars_t = std::deque<val_t>;

      env_sptr_t                parent;
      static inline size_t      id = {};
      vars_t                    funcs;
      vars_t                    vars;

      env_t(env_sptr_t parent = nullptr) : parent(parent) { }

      val_t def(const auto& key, auto pvars) {
        auto it = std::find_if((this->*pvars).begin(), (this->*pvars).end(),
            [&key](auto var) { return key == var->name; });
        if (it != (this->*pvars).end())
          throw fatal_error_t("env_t: '" + key + "' is exists");

        (this->*pvars).push_back(std::make_shared<var_info_t>());
        (this->*pvars).back()->id = id++;
        (this->*pvars).back()->name = key;
        return (this->*pvars).back();
      }

      val_t get(const auto& key, auto pvar, auto pvars) const {
        auto env = this->shared_from_this();
        while (env) {
          auto it = std::find_if((*env.*pvars).begin(), (*env.*pvars).end(),
              [&key, pvar](auto var) { return key == (*var).*pvar; });
          if (it != (*env.*pvars).end()) {
            return *it;
          }
          env = env->parent;
        }

        std::string key_str;
        if constexpr (std::is_arithmetic<decltype(key)>::value) {
          key_str = std::to_string(key);
        } else {
          key_str = key;
        }
        throw fatal_error_t("env_t: '" + key_str + "' is not exists");
      }

      val_t def_func(const key_t& key) {
        return def(key, &env_t::funcs);
      }

      val_t get_func(const key_t& key) const {
        return get(key, &var_info_t::name, &env_t::funcs);
      }

      val_t get_func(size_t key) const {
        return get(key, &var_info_t::id, &env_t::funcs);
      }

      val_t def_var(const key_t& key) {
        return def(key, &env_t::vars);
      }

      val_t get_var(const key_t& key) const {
        return get(key, &var_info_t::name, &env_t::vars);
      }

#if 0
      void save_funcs() {
        if (!parent)
          return;

        for (const auto& func : funcs) {
          parent->funcs.push_back(func);
        }
      }

      void save_vars() {
        // TODO
      }
#endif

      std::string show() const {
        std::string str;
        auto env = this->shared_from_this();
        size_t deep = {};
        while (env) {
          for (const auto& func : env->funcs) {
            str += "; func: " + indent(deep) + func->show() + "\n";
          }
          for (const auto& var : env->vars) {
            str += "; var:  " + indent(deep) + var->show() + "\n";
          }
          env = env->parent;
          deep++;
        }
        return str;
      }
    };

#if 0
    struct symbols_t {
      struct func_info_t {
        std::string name;
        std::deque<std::string> vars;
      };

      std::deque<func_info_t> funcs;

      std::string show() {
        std::string str;
        for (size_t i{}; i < funcs.size(); ++i) {
          const auto& func = funcs[i];
          str += std::to_string(i) + ": \t" + func.name + "\n";
          for (size_t i{}; i < func.vars.size(); ++i) {
            const auto& var = func.vars[i];
            str += "\t" + std::to_string(i) + ": \t" + var + "\n";
          }
        }
        return str;
      }
    };
#endif

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
    };

    struct stmt_program_t : stmt_t {
      using funcs_t = std::deque<std::shared_ptr<stmt_defn_t>>;

      env_sptr_t env;
      funcs_t    funcs;

      stmt_program_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_arg_t : stmt_t {
      int64_t value = {};

      stmt_arg_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_block_t : stmt_t {
      std::deque<std::shared_ptr<stmt_expr_t>> exprs;

      stmt_block_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
    };

    struct stmt_call_t : stmt_t {
      std::shared_ptr<stmt_expr_t>              name;
      std::vector<std::shared_ptr<stmt_expr_t>> args;

      stmt_call_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
    };

    struct stmt_defn_t : stmt_t {
      std::shared_ptr<var_info_t>  var;
      std::shared_ptr<stmt_expr_t> body;
      std::shared_ptr<env_t>       env;

      stmt_defn_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
    };

    struct stmt_defvar_t : stmt_t {
      std::shared_ptr<var_info_t>  var;
      std::shared_ptr<stmt_expr_t> body;
      std::shared_ptr<env_t>       env;

      stmt_defvar_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
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
    };

    struct stmt_func_t : stmt_t {
      std::shared_ptr<var_info_t>  var;
      std::shared_ptr<env_t>       env;

      stmt_func_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
    };

    struct stmt_if_t : stmt_t {
      std::shared_ptr<stmt_expr_t> stmt_expr_if;
      std::shared_ptr<stmt_expr_t> stmt_expr_then;
      std::shared_ptr<stmt_expr_t> stmt_expr_else;

      stmt_if_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
    };

    struct stmt_int_t : stmt_t {
      int64_t value = {};

      stmt_int_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_syscall_t : stmt_t {
      std::vector<std::shared_ptr<stmt_expr_t>> args;

      stmt_syscall_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
    };

    struct stmt_var_t : stmt_t {
      std::shared_ptr<var_info_t> var;

      stmt_var_t(const syntax_lisp_tree_t& tree, env_sptr_t env);
      std::string show(size_t deep) const override;
    };

    ////////////////////////////////////////////////////////////////////////////////

    stmt_program_t::stmt_program_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);

      env = std::make_shared<env_t>();

      for (const auto& node : tree.nodes) {
        funcs.push_back(std::make_shared<stmt_defn_t>(node, env));
      }
      DEBUG_LOGGER_SA("env: \n%s", env->show().c_str());
    }

    std::string stmt_program_t::show(size_t deep) const {
      std::string str;
      for (const auto& func : funcs) {
        str += func->show(deep + 1);
        str += token_t{.type = token_t::type_t::new_line}.show();
        str += token_t{.type = token_t::type_t::new_line}.show();
      }
      return str;
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

    stmt_block_t::stmt_block_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_block);

      auto env_block = std::make_shared<env_t>(env);

      for (const auto& node : tree.nodes) {
        exprs.push_back(std::make_shared<stmt_expr_t>(node, env_block));
      }

      // env_block->save_vars();
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

    stmt_if_t::stmt_if_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_eq(tree, 4);
      check_type(tree.nodes[0], token_t::type_t::key_if);

      stmt_expr_if   = std::make_shared<stmt_expr_t>(tree.nodes[1], env);
      stmt_expr_then = std::make_shared<stmt_expr_t>(tree.nodes[2], env);
      stmt_expr_else = std::make_shared<stmt_expr_t>(tree.nodes[3], env);
    }

    std::string stmt_if_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_if}.show();
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += stmt_expr_if->show(deep + 1);
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += stmt_expr_then->show(deep + 1);
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += stmt_expr_else->show(deep + 1);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
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
      str += token_t{.type = token_t::type_t::key_func}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += var->name;
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += body->show(deep + 1);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    stmt_syscall_t::stmt_syscall_t(const syntax_lisp_tree_t& tree, env_sptr_t env) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_syscall);

      for (size_t i = 1; i < tree.nodes.size(); ++i) {
        args.push_back(std::make_shared<stmt_expr_t>(tree.nodes[i], env));
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

    std::shared_ptr<stmt_t> process(const syntax_lisp_tree_t& tree) {
      return std::make_shared<stmt_program_t>(tree);
    }
  }



  namespace semantic_analyzer_n {

    using namespace utils_n;
    using namespace syntax_analyzer_n;
  }



#if 0
  namespace intermediate_code_generator_n {

    using namespace utils_n;
    using namespace syntax_analyzer_n;

    void process(instructions_t& instructions, functions_t& functions, const cmds_str_t& cmds_str) {
    }
  }
#endif



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



#if 0
  namespace executor_n {

    using namespace intermediate_code_generator_n;

    void process(const data_t& text, const functions_t& functions) {
      DEBUG_LOGGER_TRACE_EXEC;

      if (functions.find("__start") == functions.end())
        throw fatal_error_t("__start not exists");
    }
  }
#endif
}


struct interpreter_t {

  void exec(const std::string code) {
    using namespace aml_n;

    auto tokens = lexical_analyzer_n::process(code);
    DEBUG_LOGGER_LA("tokens: \n%s", lexical_analyzer_n::show_tokens(tokens).c_str());

    auto syntax_lisp_tree = syntax_lisp_analyzer_n::process(tokens);
    DEBUG_LOGGER_SA("syntax_lisp_tree: \n%s", syntax_lisp_tree.show({}).c_str());

    auto stmt = syntax_analyzer_n::process(syntax_lisp_tree);
    DEBUG_LOGGER_SA("syntax_tree: \n%s", stmt->show({}).c_str());

    // intermediate_code_generator_n::code_t code;
    // intermediate_code_generator_n::process(instructions, functions, cmds_str);

    // utils_n::data_t text;
    // code_generator_n::process(text, instructions);

    // executor_n::process(text, functions);
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

