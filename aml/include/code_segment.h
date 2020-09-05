#pragma once

#include <vector>
#include "utils.h"

namespace aml::code_n {
  namespace utils_n = aml::utils_n;



  enum class instruction_rpn_t : uint8_t {
    unknown,
    arg,
    call,
    exit,
    jmp,
    pop_jif,
    push8,
    ret,
    syscall,
  };

  size_t instruction_size(instruction_rpn_t cmd);
  std::string show_instruction(instruction_rpn_t cmd);



  struct code_t {
    std::vector<uint8_t> buffer;

    void write(const void* data, size_t size, size_t pos = std::string::npos);
    void write_u8(uint8_t data);
    void write_i64(int64_t data, size_t pos = std::string::npos);

    void read(void* data, size_t size, size_t& pos) const;
    uint8_t read_u8(size_t& pos) const;
    int64_t read_i64(size_t& pos) const;

    std::string show() const;
  };



  struct code_ctx_t {
    code_t  code;
    size_t  rip = {};
    size_t  rsp = {};

    std::string save() const;
    void load(const std::string&);
  };

}
