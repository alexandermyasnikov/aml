#include "code_segment.h"
#include "utils.h"

namespace aml::code_n {
  namespace utils_n = aml::utils_n;



  size_t instruction_size(instruction_rpn_t cmd) {
    switch (cmd) {
      case instruction_rpn_t::arg:     return 8;
      case instruction_rpn_t::call:    return 0;
      case instruction_rpn_t::exit:    return 0;
      case instruction_rpn_t::jmp:     return 8;
      case instruction_rpn_t::pop_jif: return 8;
      case instruction_rpn_t::push8:   return 8;
      case instruction_rpn_t::ret:     return 0;
      case instruction_rpn_t::syscall: return 0;
      default: return 0;
    }
  }

  std::string show_instruction(instruction_rpn_t cmd) {
    std::string str;
    switch (cmd) {
      case instruction_rpn_t::arg:     str += "arg";     break;
      case instruction_rpn_t::call:    str += "call";    break;
      case instruction_rpn_t::exit:    str += "exit";    break;
      case instruction_rpn_t::jmp:     str += "jmp";     break;
      case instruction_rpn_t::pop_jif: str += "pop_jif"; break;
      case instruction_rpn_t::push8:   str += "push8";   break;
      case instruction_rpn_t::ret:     str += "ret";     break;
      case instruction_rpn_t::syscall: str += "syscall"; break;
      default: str += "(unknown)";
    }
    return str;
  }



  void code_t::write(const void* data, size_t size, size_t pos) {
#if 1
    pos = std::min(pos, buffer.size());
    for (size_t i{}; i < size; ++i, ++pos) {
      if (pos < buffer.size()) {
        buffer[pos] = static_cast<const uint8_t*>(data)[i];
      } else {
        buffer.push_back(static_cast<const uint8_t*>(data)[i]);
      }
    }
#else
    if (pos == std::string::npos) {
      for (size_t i{}; i < size; ++i) {
        buffer.push_back(static_cast<const uint8_t*>(data)[i]);
      }
    } else if (pos + size < buffer.size()) {
      for (size_t i{}; i < size; ++i) {
        buffer[pos + i] = static_cast<const uint8_t*>(data)[i];
      }
    } else {
      throw fatal_error_t("invalid pos");
    }
#endif
  }

  void code_t::write_u8(uint8_t data) {
    write(&data, sizeof(data));
  }

  void code_t::write_i64(uint64_t data, size_t pos) {
    write(&data, sizeof(data), pos);
  }

  void code_t::read(void* data, size_t size, size_t& pos) const {
    if (pos + size > buffer.size())
      throw utils_n::fatal_error_t("invalid pos");
    for (size_t i{}; i < size; ++i) {
      static_cast<uint8_t*>(data)[i] = buffer[pos + i];
    }
    pos += size;
  }

  uint8_t code_t::read_u8(size_t& pos) const {
    uint8_t ret;
    read(&ret, sizeof(ret), pos);
    return ret;
  }

  int64_t code_t::read_i64(size_t& pos) const {
    int64_t ret;
    read(&ret, sizeof(ret), pos);
    return ret;
  }

  std::string code_t::show() const {
    std::string str;
    size_t rip = {};
    while (rip < buffer.size()) {
      str += std::to_string(rip) + "\t\t";
      auto cmd = static_cast<instruction_rpn_t>(read_u8(rip));
      str += show_instruction(cmd);
      switch (instruction_size(cmd)) {
        case 8: str += " " + std::to_string(read_i64(rip)); break;
        case 0: break;
        default: str += " (unknown)";
      }
      str += "\n";
    }
    return str;
  }
}
