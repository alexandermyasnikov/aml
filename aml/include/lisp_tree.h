#pragma once

#include "token.h"

namespace aml::lisp_tree_n {
  namespace token_n = aml::token_n;



  struct lisp_tree_t {
    using node_t  = token_n::token_t;
    using nodes_t = std::deque<lisp_tree_t>;

    node_t  node  = node_t{};
    nodes_t nodes = {};

    bool is_leaf() const;
    std::string show(size_t deep) const;
  };



  lisp_tree_t process(const token_n::tokens_t& tokens);
}
