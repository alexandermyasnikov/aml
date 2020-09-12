#pragma once

#include <vector>
#include <string>

namespace aml::code_n {

  enum class instruction_rpn_t : uint8_t {
    unknown,

    arg, // deprecated
    arg_1b,
    arg_2b,
    arg_3b,
    arg_4b,
    arg_5b,
    arg_6b,
    arg_7b,
    arg_8b,

    jmp,
    // jmp_1b,
    // jmp_2b,
    // jmp_3b,
    // jmp_4b,
    // jmp_5b,
    // jmp_6b,
    // jmp_7b,
    // jmp_8b,

    pop_jif,
    // pop_jif_1b,
    // pop_jif_2b,
    // pop_jif_3b,
    // pop_jif_4b,
    // pop_jif_5b,
    // pop_jif_6b,
    // pop_jif_7b,
    // pop_jif_8b,

    push, // deprecated
    push_1b,
    push_2b,
    push_3b,
    push_4b,
    push_5b,
    push_6b,
    push_7b,
    push_8b,

    call,
    exit,
    ret,
    syscall,
  };

  size_t instruction_size(instruction_rpn_t cmd);
  std::string show_instruction(instruction_rpn_t cmd);

  int64_t zigzag_encode(int64_t value);
  int64_t zigzag_decode(int64_t value);
  uint8_t zigzag_size(int64_t value);



  struct code_t {
    std::vector<uint8_t> buffer;

    void write(const void* data, size_t size, size_t pos = std::string::npos);
    void write_u8(uint8_t data);
    void write_i64(int64_t data, size_t pos = std::string::npos);
    void write_int(int64_t data, size_t size);
    void write_cmd(instruction_rpn_t cmd, int64_t value = {});

    void read(void* data, size_t size, size_t& pos) const;
    uint8_t read_u8(size_t& pos) const;
    int64_t read_i64(size_t& pos) const;
    int64_t read_int(size_t& pos, size_t size) const;

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
