
#include <iostream>
#include <variant>
#include <iomanip>
#include <regex>

#include "debug_logger.h"

#define DEBUG_LOGGER_TRACE_LA            DEBUG_LOGGER("la   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_LA(...)             DEBUG_LOG("la   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_SA            // DEBUG_LOGGER("sa   ", logger_indent_aml_t::indent)
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


/*
struct segment_t {
  uint64_t type;
  std::deque<uint64_t> data;
};

struct segment_text_t {
  std::deque<uint64_t> data;
};

struct library_file_t {
  std::deque<segment_t> segments;
};
*/

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
        key_func,
        key_block,
        key_if,
        key_set,
        key_call,
        key_syscall,
        key_var,
        key_arg,
        key_int,
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
          case type_t::key_func:    return "func";
          case type_t::key_block:   return "block";
          case type_t::key_if:      return "if";
          case type_t::key_set:     return "set";
          case type_t::key_call:    return "call";
          case type_t::key_syscall: return "syscall";
          case type_t::key_var:     return "var";
          case type_t::key_arg:     return "arg";
          case type_t::key_int:     return "int";
          case type_t::integer:     { const auto* p = std::get_if<int64_t>(&value);     return p ? std::to_string(*p) : lexeme; }
          case type_t::ident:       { const auto* p = std::get_if<std::string>(&value); return p ? *p : lexeme; }
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
          std::regex(R"(\s+|;[^\n]*)"),
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
          type_t::key_func,
          std::regex(R"(func)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_block,
          std::regex(R"(block)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_if,
          std::regex(R"(if)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_set,
          std::regex(R"(set)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_call,
          std::regex(R"(call)"),
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
          type_t::key_arg,
          std::regex(R"(arg)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::key_int,
          std::regex(R"(int)"),
          [](const std::string& lexeme) { return value_t{}; },
        }, {
          type_t::integer,
          std::regex(R"([-+]?\d+)"),
          [](const std::string& lexeme) { return stol(lexeme); },
        }, {
          type_t::ident,
          std::regex(R"(\w+)"),
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

        for (const auto& rule : rules) {
          if (!std::regex_search(it, ite, m, rule.regex, flags))
            continue;

          if (m.position())
            throw fatal_error_t("lexical_analyzer: unknown position at " + token.pos.show());

          it               += m.length();
          token.pos.column += token.pos.length;
          token.pos.length  = m.length();
          token.lexeme      = m.str();
          token.value       = rule.get_value(token.lexeme);
          token.type        = rule.type;

          if (rule.type == type_t::new_line) {
            token.pos.line++;
            token.pos.length = 0;
            token.pos.column = column_start;
          }

          return true;
        }

        return false;
      }
    };

    using tokens_t = std::deque<token_t>;

    tokens_t process(const std::string& code) {
      tokens_t tokens;

      auto it = code.begin();
      auto ite = code.end();
      token_t token;

      while (token_t::next(it, ite, token)) {
        // DEBUG_LOGGER_LA("token: %zd \t %s \t %s", (size_t) token.type, token.pos.show().c_str(), token.lexeme.c_str());
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
      nodes_t nodes = { };

      bool is_leaf() const {
        return node.type != token_t::type_t::unknown
          && node.type != token_t::type_t::lp
          && node.type != token_t::type_t::rp;
      }

      std::string show(size_t deep = 0) const {
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
    // func:    FUNC    <name>  expr
    // expr:    BLOCK   set*    expr
    // expr:    IF      expr    expr expr
    // expr:    CALL    expr    expr*
    // expr:    SYSCALL expr+
    // expr:    INT     <digit>
    // expr:    VAR     <name>
    // expr:    ARG     <digit>
    // set:     SET     <name>  expr

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
    struct stmt_func_t;
    struct stmt_expr_t;
    struct stmt_block_t;
    struct stmt_if_t;
    struct stmt_set_t;
    struct stmt_call_t;
    struct stmt_syscall_t;
    struct stmt_var_t;
    struct stmt_arg_t;
    struct stmt_int_t;

    struct stmt_t {
      virtual ~stmt_t() { }
      virtual std::string show(size_t deep) const = 0;
    };

    struct stmt_program_t : stmt_t {
      std::deque<std::shared_ptr<stmt_func_t>> funcs;

      stmt_program_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_func_t : stmt_t {
      std::string name = "<name>";
      std::shared_ptr<stmt_expr_t> body;

      stmt_func_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_expr_t : stmt_t {
      using expr_t = std::variant<
        std::shared_ptr<stmt_block_t>,
        std::shared_ptr<stmt_if_t>,
        std::shared_ptr<stmt_call_t>,
        std::shared_ptr<stmt_syscall_t>,
        std::shared_ptr<stmt_var_t>,
        std::shared_ptr<stmt_arg_t>,
        std::shared_ptr<stmt_int_t>>;

      expr_t expr;

      stmt_expr_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_block_t : stmt_t {
      std::deque<std::shared_ptr<stmt_set_t>> stmt_sets;
      std::shared_ptr<stmt_expr_t>            stmt_expr;

      stmt_block_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_if_t : stmt_t {
      std::shared_ptr<stmt_expr_t> stmt_expr_if;
      std::shared_ptr<stmt_expr_t> stmt_expr_then;
      std::shared_ptr<stmt_expr_t> stmt_expr_else;

      stmt_if_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_set_t : stmt_t {
      std::string name = "<name>";
      std::shared_ptr<stmt_expr_t> body;

      stmt_set_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_call_t : stmt_t {
      std::shared_ptr<stmt_expr_t>              name;
      std::vector<std::shared_ptr<stmt_expr_t>> args;

      stmt_call_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_syscall_t : stmt_t {
      std::vector<std::shared_ptr<stmt_expr_t>> args;

      stmt_syscall_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_var_t : stmt_t {
      std::string value = "<name>";

      stmt_var_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_arg_t : stmt_t {
      int64_t value = { };

      stmt_arg_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    struct stmt_int_t : stmt_t {
      int64_t value = { };

      stmt_int_t(const syntax_lisp_tree_t& tree);
      std::string show(size_t deep) const override;
    };

    ////////////////////////////////////////////////////////////////////////////////

    stmt_program_t::stmt_program_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);

      for (const auto& node : tree.nodes) {
        funcs.push_back(std::make_shared<stmt_func_t>(node));
      }
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

    stmt_func_t::stmt_func_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_eq(tree, 3);
      check_type(tree.nodes[0], token_t::type_t::key_func);
      check_type(tree.nodes[1], token_t::type_t::ident);

      name = std::get<std::string>(tree.nodes[1].node.value);
      body = std::make_shared<stmt_expr_t>(tree.nodes[2]);
    }

    std::string stmt_func_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_func}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += name;
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += body->show(deep + 1);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    stmt_expr_t::stmt_expr_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 1);

      switch (tree.nodes[0].node.type) {
        case token_t::type_t::key_block:     expr = std::make_shared<stmt_block_t>(tree);  break;
        case token_t::type_t::key_if:        expr = std::make_shared<stmt_if_t>(tree);     break;
        case token_t::type_t::key_call:      expr = std::make_shared<stmt_call_t>(tree);   break;
        // case token_t::type_t::key_syscall:   expr = std::make_shared<stmt_syscall_t>(tree); break;
        case token_t::type_t::key_var:       expr = std::make_shared<stmt_var_t>(tree);    break;
        case token_t::type_t::key_arg:       expr = std::make_shared<stmt_arg_t>(tree);    break;
        case token_t::type_t::key_int:       expr = std::make_shared<stmt_int_t>(tree);    break;
        default: throw syntax_error_t(tree.nodes[0].node);
      }
    }

    std::string stmt_expr_t::show(size_t deep = 0) const {
      std::string str;
      std::visit(overloaded{[&str, deep] (const auto &expr) { str = expr->show(deep); } }, expr);
      return str;
    }

    stmt_block_t::stmt_block_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_block);

      for (size_t i = 1; i < tree.nodes.size() - 1; ++i) {
        stmt_sets.push_back(std::make_shared<stmt_set_t>(tree.nodes[i]));
      }

      stmt_expr = std::make_shared<stmt_expr_t>(tree.nodes[tree.nodes.size() - 1]);
    }

    std::string stmt_block_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_block}.show();
      for (const auto& stmt_set : stmt_sets) {
        str += token_t{.type = token_t::type_t::new_line}.show();
        str += indent(deep);
        str += stmt_set->show(deep + 1);
      }
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += stmt_expr->show(deep + 1);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    stmt_if_t::stmt_if_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_eq(tree, 4);
      check_type(tree.nodes[0], token_t::type_t::key_if);

      stmt_expr_if   = std::make_shared<stmt_expr_t>(tree.nodes[1]);
      stmt_expr_then = std::make_shared<stmt_expr_t>(tree.nodes[2]);
      stmt_expr_else = std::make_shared<stmt_expr_t>(tree.nodes[3]);
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

    stmt_set_t::stmt_set_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_eq(tree, 3);
      check_type(tree.nodes[0], token_t::type_t::key_set);
      check_type(tree.nodes[1], token_t::type_t::ident);

      name = std::get<std::string>(tree.nodes[1].node.value);
      body = std::make_shared<stmt_expr_t>(tree.nodes[2]);
    }

    std::string stmt_set_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_set}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += name;
      str += token_t{.type = token_t::type_t::new_line}.show();
      str += indent(deep);
      str += body->show(deep + 1);
      str += token_t{.type = token_t::type_t::rp}.show();
      return str;
    }

    stmt_call_t::stmt_call_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_call);

      name = std::make_shared<stmt_expr_t>(tree.nodes[1]);
      for (size_t i = 2; i < tree.nodes.size(); ++i) {
        args.push_back(std::make_shared<stmt_expr_t>(tree.nodes[i]));
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

    // syscall

    stmt_var_t::stmt_var_t(const syntax_lisp_tree_t& tree) {
      DEBUG_LOGGER_TRACE_SA;

      check_not_leaf(tree);
      check_size_gt(tree, 2);
      check_type(tree.nodes[0], token_t::type_t::key_var);
      check_type(tree.nodes[1], token_t::type_t::ident);

      value = std::get<std::string>(tree.nodes[1].node.value);
    }

    std::string stmt_var_t::show(size_t deep) const {
      std::string str;
      str += token_t{.type = token_t::type_t::lp}.show();
      str += token_t{.type = token_t::type_t::key_var}.show();
      str += token_t{.type = token_t::type_t::whitespace}.show();
      str += value;
      str += token_t{.type = token_t::type_t::rp}.show();
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

    using stmt_sptr_t = std::shared_ptr<stmt_t>;
    stmt_sptr_t process(const syntax_lisp_tree_t& tree) {
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

    union instruction_t {
      uint16_t value;
      struct {
        uint8_t  op  : 4;
        uint8_t  rd  : 4;
        uint8_t  rs1 : 4;
        uint8_t  rs2 : 4;
      } __attribute__((packed)) cmd;
      struct {
        uint8_t  op : 4;
        uint8_t  rd : 4;
        uint8_t  val;
      } __attribute__((packed)) cmd_set;
    };

    void process(instructions_t& instructions, functions_t& functions, const cmds_str_t& cmds_str) {
      for (auto cmd_str : cmds_str) {
        if (cmd_str.at(0) == "SET" && cmd_str.size() == 3) {
          auto op = opcode_index(0, cmd_str.at(0));
          auto rd = reg_index(cmd_str.at(1));
          reg_value_t value = strtol(cmd_str.at(2).c_str(), nullptr, 0);
          macro_set(instructions, rd, value);

        } else if (cmd_str.at(0) == "FUNCTION" && cmd_str.size() == 2) {
          auto name = cmd_str.at(1);

          if (functions.find(name) != functions.end())
            throw fatal_error_t("function exists");

          functions[name] = instructions.size() * sizeof(instruction_t);

        } else if (cmd_str.at(0) == "LABEL") {
          throw fatal_error_t("LABEL TODO");

        } else if (cmd_str.at(0) == "ADDRESS" && cmd_str.size() == 3) {
          auto rd   = reg_index(cmd_str.at(1));
          auto name = cmd_str.at(2);

          if (functions.find(name) == functions.end())
            throw fatal_error_t("function not exists");

          macro_set(instructions, rd, functions[name]);

        } else if (cmd_str.size() == 4) {
          auto op  = opcode_index(0, cmd_str.at(0));
          auto rd  = reg_index(cmd_str.at(1));
          auto rs1 = reg_index(cmd_str.at(2));
          auto rs2 = reg_index(cmd_str.at(3));
          instructions.push_back({ .cmd  = { op, rd, rs1, rs2 } });

        } else if (cmd_str.size() == 3) {
          auto op1 = opcode_index(0, "OTH0");
          auto op2 = opcode_index(1, cmd_str.at(0));
          auto rd  = reg_index(cmd_str.at(1));
          auto rs  = reg_index(cmd_str.at(2));
          instructions.push_back({ .cmd  = { op1, op2, rd, rs } });

        } else if (cmd_str.size() == 2) {
          auto op1 = opcode_index(0, "OTH0");
          auto op2 = opcode_index(1, "OTH1");
          auto op3 = opcode_index(2, cmd_str.at(0));
          auto rd  = reg_index(cmd_str.at(1));
          instructions.push_back({ .cmd  = { op1, op2, op3, rd } });

        } else if (cmd_str.size() == 1) {
          auto op1 = opcode_index(0, "OTH0");
          auto op2 = opcode_index(1, "OTH1");
          auto op3 = opcode_index(2, "OTH2");
          auto op4 = opcode_index(3, cmd_str.at(0));
          instructions.push_back({ .cmd  = { op1, op2, op3, op4 } });

        } else {
          throw fatal_error_t("unknown cmd format");
        }
      }

      for (size_t i = 0; i < instructions.size(); ++i) {
        DEBUG_LOGGER_ICG("instruction: %08x '%s'", i * sizeof(instruction_t), print_instruction(instructions[i]).c_str());
      }
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

      data_t stack(0xFFFF, 0);

      registers_set_t* registers_set = reinterpret_cast<registers_set_t*>(stack.data());
      (*registers_set)[reg_index("RP")] = 0;
      (*registers_set)[reg_index("RI")] = functions.at("__start");
      (*registers_set)[reg_index("RB")] = sizeof(registers_set_t);
      (*registers_set)[reg_index("RS")] = (*registers_set)[reg_index("RB")];

      DEBUG_LOGGER_EXEC("stack frame: '%s'", print_stack(stack, registers_set).c_str());

      while (true) {
        instruction_t instruction = *reinterpret_cast<const instruction_t*>(text.data() + (*registers_set)[reg_index("RI")]);
        exec_cmd0(text, stack, registers_set, instruction);

        DEBUG_LOGGER_EXEC("instruction: '%s'", print_instruction(instruction).c_str());
        DEBUG_LOGGER_EXEC("stack frame: '%s'", print_stack(stack, registers_set).c_str());

        (*registers_set)[reg_index("RI")] += sizeof(instruction_t);
      }
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
    DEBUG_LOGGER_SA("syntax_lisp_tree: \n%s", syntax_lisp_tree.show().c_str());

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

  // std::cout << code << std::endl;

  interpreter_t interpreter;
  interpreter.exec(code);

  return 0;
}

