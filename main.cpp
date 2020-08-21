
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

    using data_t = std::vector<uint8_t>;

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
        return std::to_string(line) + ":" + std::to_string(column) + ":" + std::to_string(length);
      }
    };

    struct attr_nl_t {
      static inline const std::string regex = R"(\n)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "\n"; }
    };

    struct attr_empty_t {
      static inline const std::string regex = R"(\s+|;[^\n]*)";

      void init(const std::string& lexeme) { }
      std::string show() const { return ""; }
    };

    struct attr_lp_t {
      static inline const std::string regex = R"(\()";

      void init(const std::string& lexeme) { }
      std::string show() const { return "("; }
    };

    struct attr_rp_t {
      static inline const std::string regex = R"(\))";

      void init(const std::string& lexeme) { }
      std::string show() const { return ")"; }
    };

    struct attr_func_t {
      static inline const std::string regex = R"(func)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "func"; }
    };

    struct attr_return_t {
      static inline const std::string regex = R"(return)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "return"; }
    };

    struct attr_block_t {
      static inline const std::string regex = R"(block)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "block"; }
    };

    struct attr_if_t {
      static inline const std::string regex = R"(if)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "if"; }
    };

    struct attr_set_t {
      static inline const std::string regex = R"(set)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "set"; }
    };

    struct attr_call_t {
      static inline const std::string regex = R"(call)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "call"; }
    };

    struct attr_var_t {
      static inline const std::string regex = R"(var)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "var"; }
    };

    struct attr_arg_t {
      static inline const std::string regex = R"(arg)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "arg"; }
    };

    struct attr_int_t {
      static inline const std::string regex = R"(int)";

      void init(const std::string& lexeme) { }
      std::string show() const { return "int"; }
    };

    struct attr_integer_t {
      static inline const std::string regex = R"([-+]?\d+)";

      int64_t value;

      void init(const std::string& lexeme) { value = stol(lexeme); }
      std::string show() const { return std::to_string(value); }
    };

    struct attr_ident_t {
      static inline const std::string regex = R"(\w+)";

      std::string value;

      void init(const std::string& lexeme) { value = lexeme; }
      std::string show() const { return value; }
    };

    using attr_t = std::variant<
      attr_nl_t,
      attr_empty_t,
      attr_lp_t,
      attr_rp_t,
      attr_func_t,
      attr_return_t,
      attr_block_t,
      attr_if_t,
      attr_set_t,
      attr_call_t,
      attr_var_t,
      attr_arg_t,
      attr_int_t,
      attr_integer_t,
      attr_ident_t>;

    struct token_t {
      attr_t      attr;
      pos_t       pos;
      std::string lexeme;
    };

    using tokens_t = std::deque<token_t>;

    static inline std::vector<attr_t> attr_rules = {
      attr_nl_t{},
      attr_empty_t{},
      attr_lp_t{},
      attr_rp_t{},
      attr_func_t{},
      attr_return_t{},
      attr_block_t{},
      attr_if_t{},
      attr_set_t{},
      attr_call_t{},
      attr_var_t{},
      attr_arg_t{},
      attr_int_t{},
      attr_integer_t{},
      attr_ident_t{},
    };

    const std::string& get_regex(const attr_t& attr) {
      const std::string* regex;
      std::visit(overloaded{[&regex] (const auto &attr) { regex = &attr.regex; } }, attr);
      return *regex;
    }

    attr_t get_attr(const attr_t& attr, const std::string& lexeme) {
      attr_t attr_new;
      std::visit(overloaded{[&attr_new, lexeme] (const auto& attr) {
          auto attr_tmp = attr; attr_tmp.init(lexeme); attr_new = attr_tmp; } }, attr);
      return attr_new;
    }

    tokens_t process(const std::string& code) {
      tokens_t tokens;
      auto it = code.begin();
      auto ite = code.end();
      std::smatch m;
      std::regex_constants::match_flag_type flags =
          std::regex_constants::match_continuous | std::regex_constants::match_not_null;

      token_t token;

      bool run = true;
      while (run && it != ite) {
        run = false;
        for (const auto& attr_rule : attr_rules) {
          std::string regex = get_regex(attr_rule);

          if (!std::regex_search(it, ite, m, std::regex(regex), flags))
            continue;

          if (m.position())
            throw fatal_error_t("lexical_analyzer: unknown position at " + token.pos.show());

          token.lexeme = m.str();
          token.attr = get_attr(attr_rule, token.lexeme);
          token.pos.length = m.length();
          token.pos.column += m.length();

          DEBUG_LOGGER_LA("pos: %zd:%zd:%zd \t %s", token.pos.line, token.pos.column, token.pos.length, m.str().c_str());

          if (std::get_if<attr_nl_t>(&token.attr)) {
            token.pos.line++;
            token.pos.column = column_start;
          } else if (std::get_if<attr_empty_t>(&token.attr)) {
            ;
          } else {
            tokens.push_back(token);
          }

          it += m.length();
          run = true;
          break;
        }
      }

      if (it != ite)
        throw fatal_error_t("lexical_analyzer: unexpected attr at " + token.pos.show());

      return tokens;
    }

    static std::string show_attr(const attr_t& attr) {
      std::string str;
      std::visit(overloaded{[&str] (const auto &attr) { str = attr.show(); }}, attr);
      return str;
    }

    static std::string show(const tokens_t& tokens) {
      std::string str;
      for (const auto& token : tokens) {
        str += show_attr(token.attr) + " ";
      }
      return str;
    }
  }



  namespace syntax_lisp_analyzer_n {

    using namespace utils_n;
    using namespace lexical_analyzer_n;

    struct syntax_lisp_tree_t {
      using node_t = token_t;
      using nodes_t = std::deque<syntax_lisp_tree_t>;

      node_t  node  = token_t{.attr = attr_empty_t{}};
      nodes_t nodes = { };

      bool is_leaf() const {
        return !std::get_if<attr_empty_t>(&node.attr);
      }
    };

    syntax_lisp_tree_t process(const tokens_t& tokens) {
      std::stack<syntax_lisp_tree_t> stack;
      stack.push(syntax_lisp_tree_t{});
      token_t token_last;
      for (const auto& token : tokens) {
        token_last = token;
        if (std::get_if<attr_lp_t>(&token.attr)) {
          stack.push(syntax_lisp_tree_t{});
          stack.top().node.pos = token.pos;
        } else if (std::get_if<attr_rp_t>(&token.attr)) {
          if (stack.size() < 2)
            throw fatal_error_t("syntax_lisp_analyzer: unexpected ')' at " + token.pos.show() + " " + token.lexeme);
          auto top = stack.top();
          stack.pop();
          stack.top().nodes.push_back(top);
        } else {
          stack.top().nodes.push_back(syntax_lisp_tree_t{.node = token});
        }
      }

      if (stack.size() != 1)
        throw fatal_error_t("syntax_lisp_analyzer: parse error at " + token_last.pos.show() + " " + token_last.lexeme);

      return stack.top();
    }

    static std::string show(const syntax_lisp_tree_t& syntax_lisp_tree) {
      std::string str;

      if (syntax_lisp_tree.is_leaf()) {
        str += show_attr(syntax_lisp_tree.node.attr);
      } else {
        str += attr_lp_t().show() + " ";
        for (const auto& node : syntax_lisp_tree.nodes) {
          str += show(node) + " ";
        }
        str += attr_rp_t().show();
      }

      return str;
    }
  }



  namespace syntax_analyzer_n {

    using namespace utils_n;
    using namespace syntax_lisp_analyzer_n;

    // GRAMMAR
    // program: func+
    // func:    FUNC <name> expr
    // expr:    RETURN expr
    // expr:    BLOCK expr+
    // expr:    IF expr expr expr
    // expr:    SET <name> expr
    // expr:    CALL <name> arg*
    // expr:    INT <digit>
    // expr:    VAR <name>
    // expr:    ARG <digit>

    struct stmt_program_t;
    struct stmt_func_t;
    struct stmt_return_t;
    struct stmt_expr_t;
    struct stmt_block_t;
    struct stmt_if_t;
    struct stmt_set_t;
    struct stmt_call_t;
    struct stmt_var_t;
    struct stmt_arg_t;
    struct stmt_int_t;

    struct stmt_expr_t {
      using expr_t = std::variant<
        std::shared_ptr<stmt_return_t>,
        std::shared_ptr<stmt_block_t>,
        std::shared_ptr<stmt_if_t>,
        std::shared_ptr<stmt_set_t>,
        std::shared_ptr<stmt_call_t>,
        std::shared_ptr<stmt_var_t>,
        std::shared_ptr<stmt_arg_t>,
        std::shared_ptr<stmt_int_t>>;

      expr_t expr;

      std::string show(size_t deep) const;
      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
    };

    struct stmt_block_t {
      std::vector<stmt_expr_t> stmt_exprs;

      std::string show(size_t deep) const;
      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
    };

    struct stmt_if_t {
      stmt_expr_t stmt_expr_if;
      stmt_expr_t stmt_expr_then;
      stmt_expr_t stmt_expr_else;

      std::string show(size_t deep) const;
      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
    };

    struct stmt_set_t {
      std::string name = "<name>";
      stmt_expr_t body;

      std::string show(size_t deep) const;
      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
    };

    struct stmt_call_t {
      std::string name = "<name>";
      std::vector<stmt_expr_t> args;

      std::string show(size_t deep) const;
      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
    };

    struct stmt_var_t {
      std::string value = "<name>";

      std::string show(size_t deep) const;
      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
    };

    struct stmt_arg_t {
      size_t value = { };

      std::string show(size_t deep) const;
      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
    };

    struct stmt_int_t {
      int64_t value = { };

      std::string show(size_t deep) const;
      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
    };

    struct stmt_func_t {
      std::string name = "<name>";
      stmt_expr_t body;

      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
      std::string show(size_t deep) const;
    };

    struct stmt_return_t {
      stmt_expr_t body;

      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
      std::string show(size_t deep) const;
    };

    struct stmt_program_t {
      std::deque<stmt_func_t> funcs;

      void parse(const syntax_lisp_tree_t& syntax_lisp_tree);
      std::string show(size_t deep) const;
    };

    stmt_program_t process(const syntax_lisp_tree_t& syntax_lisp_tree) {
      stmt_program_t stmt_program;
      stmt_program.parse(syntax_lisp_tree);
      return stmt_program;
    }

    static std::string show(const stmt_program_t& stmt_program) {
      std::string str;
      str += stmt_program.show(0);
      return str;
    }

    ////////////////////////////////////////////////////////////////////////////////

    std::string stmt_expr_t::show(size_t deep) const {
      std::string str;
      std::visit(overloaded {
          [&str, deep] (const auto &expr) { str = expr->show(deep); },
          }, expr);
      return str;
    }

    void stmt_expr_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;

      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("expr: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() < 1)
        throw fatal_error_t("expr: expected 1 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf())
        throw fatal_error_t("epxr: expected leaf at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      if (std::get_if<attr_return_t>(&nodes[0].node.attr)) {
        stmt_return_t stmt_return;
        stmt_return.parse(syntax_lisp_tree);
        expr = std::make_shared<stmt_return_t>(stmt_return);

      } else if (std::get_if<attr_block_t>(&nodes[0].node.attr)) {
        stmt_block_t stmt_block;
        stmt_block.parse(syntax_lisp_tree);
        expr = std::make_shared<stmt_block_t>(stmt_block);

      } else if (std::get_if<attr_if_t>(&nodes[0].node.attr)) {
        stmt_if_t stmt_if;
        stmt_if.parse(syntax_lisp_tree);
        expr = std::make_shared<stmt_if_t>(stmt_if);

      } else if (std::get_if<attr_set_t>(&nodes[0].node.attr)) {
        stmt_set_t stmt_set;
        stmt_set.parse(syntax_lisp_tree);
        expr = std::make_shared<stmt_set_t>(stmt_set);

      } else if (std::get_if<attr_call_t>(&nodes[0].node.attr)) {
        stmt_call_t stmt_call;
        stmt_call.parse(syntax_lisp_tree);
        expr = std::make_shared<stmt_call_t>(stmt_call);

      } else if (std::get_if<attr_var_t>(&nodes[0].node.attr)) {
        stmt_var_t stmt_var;
        stmt_var.parse(syntax_lisp_tree);
        expr = std::make_shared<stmt_var_t>(stmt_var);

      } else if (std::get_if<attr_arg_t>(&nodes[0].node.attr)) {
        stmt_arg_t stmt_arg;
        stmt_arg.parse(syntax_lisp_tree);
        expr = std::make_shared<stmt_arg_t>(stmt_arg);

      } else if (std::get_if<attr_int_t>(&nodes[0].node.attr)) {
        stmt_int_t stmt_int;
        stmt_int.parse(syntax_lisp_tree);
        expr = std::make_shared<stmt_int_t>(stmt_int);

      } else {
        throw fatal_error_t("epxr: unknown expr at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);
      }
    }

    std::string stmt_block_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_block_t().show();
      for (const auto& stmt_expr : stmt_exprs) {
        str += "\n";
        str += indent(deep);
        str += stmt_expr.show(deep + 1);
      }
      str += attr_rp_t().show();
      return str;
    }

    void stmt_block_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("block: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() < 2)
        throw fatal_error_t("block: expected 2 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_block_t>(&nodes[0].node.attr))
        throw fatal_error_t("block: expected attr_block_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      for (size_t i = 1; i < nodes.size(); ++i) {
        stmt_expr_t stmt_expr;
        stmt_expr.parse(nodes[i]);
        stmt_exprs.push_back(stmt_expr);
      }
    }

    std::string stmt_if_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_if_t().show();
      str += "\n";
      str += indent(deep);
      str += stmt_expr_if.show(deep + 1);
      str += "\n";
      str += indent(deep);
      str += stmt_expr_then.show(deep + 1);
      str += "\n";
      str += indent(deep);
      str += stmt_expr_else.show(deep + 1);
      str += attr_rp_t().show();
      return str;
    }

    void stmt_if_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("if: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() != 4)
        throw fatal_error_t("if: expected 4 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_if_t>(&nodes[0].node.attr))
        throw fatal_error_t("if: expected attr_if_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      stmt_expr_if.parse(nodes[1]);
      stmt_expr_then.parse(nodes[2]);
      stmt_expr_else.parse(nodes[3]);
    }

    std::string stmt_set_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_set_t().show();
      str += " ";
      str += name;
      str += "\n";
      str += indent(deep);
      str += body.show(deep + 1);
      str += attr_rp_t().show();
      return str;
    }

    void stmt_set_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("set: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() != 3)
        throw fatal_error_t("set: expected 3 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_set_t>(&nodes[0].node.attr))
        throw fatal_error_t("set: expected attr_set_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      if (!nodes[1].is_leaf() || !std::get_if<attr_ident_t>(&nodes[1].node.attr))
        throw fatal_error_t("set: expected attr_ident_t at "
            + syntax_lisp_tree.nodes[1].node.pos.show() + " " + syntax_lisp_tree.nodes[1].node.lexeme);
      name = std::get<attr_ident_t>(nodes[1].node.attr).value;

      body.parse(nodes[2]);
    }

    std::string stmt_call_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_call_t().show();
      str += " ";
      str += name;
      for (const auto& arg : args) {
        str += "\n";
        str += indent(deep);
        str += arg.show(deep + 1);
      }
      str += attr_rp_t().show();
      return str;
    }

    void stmt_call_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("call: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() < 2)
        throw fatal_error_t("call: expected 2 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_call_t>(&nodes[0].node.attr))
        throw fatal_error_t("call: expected attr_call_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      if (!nodes[1].is_leaf() || !std::get_if<attr_ident_t>(&nodes[1].node.attr))
        throw fatal_error_t("call: expected attr_ident_t at "
            + syntax_lisp_tree.nodes[1].node.pos.show() + " " + syntax_lisp_tree.nodes[1].node.lexeme);
      name = std::get<attr_ident_t>(nodes[1].node.attr).value;

      for (size_t i = 2; i < nodes.size(); ++i) {
        stmt_expr_t stmt_expr;
        stmt_expr.parse(nodes[i]);
        args.push_back(stmt_expr);
      }
    }

    std::string stmt_var_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_var_t().show();
      str += " ";
      str += value;
      str += attr_rp_t().show();
      return str;
    }

    void stmt_var_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("var: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() != 2)
        throw fatal_error_t("var: expected 2 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_var_t>(&nodes[0].node.attr))
        throw fatal_error_t("var: expected attr_var_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      if (!nodes[1].is_leaf() || !std::get_if<attr_ident_t>(&nodes[1].node.attr))
        throw fatal_error_t("var: expected attr_ident_t at "
            + syntax_lisp_tree.nodes[1].node.pos.show() + " " + syntax_lisp_tree.nodes[1].node.lexeme);
      value = std::get<attr_ident_t>(nodes[1].node.attr).value;
    }

    std::string stmt_arg_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_arg_t().show();
      str += " ";
      str += std::to_string(value);
      str += attr_rp_t().show();
      return str;
    }

    void stmt_arg_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("arg: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() != 2)
        throw fatal_error_t("arg: expected 2 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_arg_t>(&nodes[0].node.attr))
        throw fatal_error_t("arg: expected attr_arg_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      if (!nodes[1].is_leaf() || !std::get_if<attr_integer_t>(&nodes[1].node.attr))
        throw fatal_error_t("arg: expected attr_integer_t at "
            + syntax_lisp_tree.nodes[1].node.pos.show() + " " + syntax_lisp_tree.nodes[1].node.lexeme);
      value = std::get<attr_integer_t>(nodes[1].node.attr).value;
    }

    std::string stmt_int_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_int_t().show();
      str += " ";
      str += std::to_string(value);
      str += attr_rp_t().show();
      return str;
    }

    void stmt_int_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("int: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() != 2)
        throw fatal_error_t("int: expected 2 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_int_t>(&nodes[0].node.attr))
        throw fatal_error_t("int: expected attr_int_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      if (!nodes[1].is_leaf() || !std::get_if<attr_integer_t>(&nodes[1].node.attr))
        throw fatal_error_t("int: expected attr_integer_t at "
            + syntax_lisp_tree.nodes[1].node.pos.show() + " " + syntax_lisp_tree.nodes[1].node.lexeme);
      value = std::get<attr_integer_t>(nodes[1].node.attr).value;
    }

    void stmt_func_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("func: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() != 3)
        throw fatal_error_t("func: expected 3 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_func_t>(&nodes[0].node.attr))
        throw fatal_error_t("func: expected attr_func_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      if (!nodes[1].is_leaf() || !std::get_if<attr_ident_t>(&nodes[1].node.attr))
        throw fatal_error_t("func: expected attr_ident_t at "
            + syntax_lisp_tree.nodes[1].node.pos.show() + " " + syntax_lisp_tree.nodes[1].node.lexeme);
      name = std::get<attr_ident_t>(nodes[1].node.attr).value;

      body.parse(nodes[2]);
    }

    std::string stmt_return_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_return_t().show();
      str += "\n";
      str += indent(deep);
      str += body.show(deep + 1);
      str += attr_rp_t().show();
      return str;
    }

    void stmt_return_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("return: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      const auto& nodes = syntax_lisp_tree.nodes;

      if (nodes.size() != 2)
        throw fatal_error_t("return: expected 2 nodes at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      if (!nodes[0].is_leaf() || !std::get_if<attr_return_t>(&nodes[0].node.attr))
        throw fatal_error_t("return: expected attr_return_t at "
            + syntax_lisp_tree.nodes[0].node.pos.show() + " " + syntax_lisp_tree.nodes[0].node.lexeme);

      body.parse(nodes[1]);
    }

    std::string stmt_func_t::show(size_t deep) const {
      std::string str;
      str += attr_lp_t().show();
      str += attr_func_t().show();
      str += " ";
      str += name;
      str += "\n";
      str += indent(deep);
      str += body.show(deep + 1);
      str += attr_rp_t().show();
      return str;
    }

    void stmt_program_t::parse(const syntax_lisp_tree_t& syntax_lisp_tree) {
      DEBUG_LOGGER_TRACE_SA;
      if (syntax_lisp_tree.is_leaf())
        throw fatal_error_t("program: unexpected leaf at "
            + syntax_lisp_tree.node.pos.show() + " " + syntax_lisp_tree.node.lexeme);

      for (const auto& node : syntax_lisp_tree.nodes) {
        stmt_func_t stmt_func;
        stmt_func.parse(node);
        funcs.push_back(stmt_func);
      }
    }

    std::string stmt_program_t::show(size_t deep) const {
      std::string str;
      for (const auto& func : funcs) {
        str += func.show(deep + 1);
        str += "\n";
        str += "\n";
      }
      return str;
    }
  }



  namespace semantic_analyzer_n {

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



  namespace code_optimizer_n {
  }



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
    DEBUG_LOGGER_LA("tokens: \n%s", lexical_analyzer_n::show(tokens).c_str());

    auto syntax_lisp_tree = syntax_lisp_analyzer_n::process(tokens);
    DEBUG_LOGGER_SA("syntax_lisp_tree: \n%s", syntax_lisp_analyzer_n::show(syntax_lisp_tree).c_str());

    auto stmt_program = syntax_analyzer_n::process(syntax_lisp_tree);
    DEBUG_LOGGER_SA("syntax_tree: \n%s", syntax_analyzer_n::show(stmt_program).c_str());

    // TODO
    // intermediate_code_generator_n::instructions_t instructions;
    // intermediate_code_generator_n::functions_t functions;
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

