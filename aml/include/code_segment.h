#pragma once

#include <deque>
#include <string>
#include <vector>

namespace aml::code_n {

  enum class cmd_id_t : uint8_t {
    arg,
    call,
    exit,
    jmp,
    pop,
    pop_jif,
    push,
    ret,
    syscall,
  };



  struct cmd_t {
    union {
      struct {
        uint8_t id:4,
                ext:1,
                len:3;
      } bits;
      uint8_t cmd;
    };
    int64_t val;

    cmd_t() = default;
    cmd_t(cmd_id_t cmd_id, int64_t value = {});

    void encode();
    void decode();
    std::string show() const;
  };



  int64_t zigzag_encode(int64_t value);
  int64_t zigzag_decode(int64_t value);
  uint8_t zigzag_size(int64_t value);



  struct code_t {
    std::vector<uint8_t> buffer;

    void write(const void* data, size_t size, size_t pos = std::string::npos);
    void write_u8(uint8_t data);
    void write_i64(int64_t data, size_t pos = std::string::npos);
    void write_int(int64_t data, size_t size);
    void write_cmd(cmd_t cmd);

    void read(void* data, size_t size, size_t& pos) const;
    uint8_t read_u8(size_t& pos) const;
    int64_t read_i64(size_t& pos) const;
    int64_t read_int(size_t& pos, size_t size) const;
    cmd_t   read_cmd(size_t& pos) const;

    std::string show() const;
  };



  struct code_ctx_t {
    code_t  code;
    size_t  rip = {};
    size_t  rsp = {};

    std::string save() const;
    void load(const std::string&);
  };



  struct stack_t {
    std::deque<int64_t> buffer;
    int64_t rbp = {};
    size_t  rip = {};
    size_t  limit = 0xFFFF;

    bool step(const code_t& code);
    size_t size() const;
    int64_t& get(size_t pos);
    int64_t& back();
    void push_back(int64_t value);
    void pop_back();
    std::string show() const;
  };
}
