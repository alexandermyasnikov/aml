#pragma once

#include <deque>
#include "utils.h"
#include "code_segment.h"

namespace aml::exec_n {
  namespace utils_n = aml::utils_n;
  namespace code_n = aml::code_n;



  struct machine_t {
    std::deque<int64_t> stack;
    int64_t rbp = {};
    size_t rip = {};

    bool step(const code_n::code_t& code);
    std::string show() const;
  };
}