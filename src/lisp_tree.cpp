#include "lisp_tree.h"

namespace aml::lisp_tree_n {
  namespace utils_n = aml::utils_n;



  bool lisp_tree_t::is_leaf() const {
    return node.type != token_n::type_t::unknown
      && node.type != token_n::type_t::lp
      && node.type != token_n::type_t::rp;
  }

  std::string lisp_tree_t::show(size_t deep) const {
    std::string str;

    if (is_leaf()) {
      str += node.show();
    } else {
      str += node_t{.type = token_n::type_t::lp}.show();
      for (size_t i{}; i < nodes.size(); ++i) {
        const auto& node = nodes[i];
        if (i) {
          str += node_t{.type = token_n::type_t::new_line}.show();
          str += utils_n::indent(deep);
        }
        str += node.show(deep + 1);
      }
      str += node_t{.type = token_n::type_t::rp}.show();
    }

    return str;
  }



  lisp_tree_t process(const token_n::tokens_t& tokens) {
    std::stack<lisp_tree_t> stack;
    stack.push(lisp_tree_t{});
    token_n::token_t token_last;
    for (const auto& token : tokens) {
      token_last = token;

      if (token.type == token_n::type_t::lp) {
        stack.push(lisp_tree_t{.node = token});

      } else if (token.type == token_n::type_t::rp) {
        if (stack.size() < 2)
          throw utils_n::fatal_error_t("lisp_tree: unexpected ')' at " + token.pos.show() + " " + token.lexeme);
        auto top = stack.top();
        stack.pop();
        stack.top().nodes.push_back(top);

      } else if (token.is_primary()) {
        stack.top().nodes.push_back(lisp_tree_t{.node = token});

      } else {
        throw utils_n::fatal_error_t("lisp_tree: expected primary token");
      }
    }

    if (stack.size() != 1)
      throw utils_n::fatal_error_t("lisp_tree: parse error at " + token_last.pos.show() + " " + token_last.lexeme);

    return stack.top();
  }
}
