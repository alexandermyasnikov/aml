#include "code_segment.h"
#include "utils.h"

namespace aml::code_n {
  namespace utils_n = aml::utils_n;



  size_t instruction_size(instruction_rpn_t cmd) {
    switch (cmd) {
      case instruction_rpn_t::arg:        return 8;
      case instruction_rpn_t::arg_1b:     return 1;
      case instruction_rpn_t::arg_2b:     return 2;
      case instruction_rpn_t::arg_3b:     return 3;
      case instruction_rpn_t::arg_4b:     return 4;
      case instruction_rpn_t::arg_5b:     return 5;
      case instruction_rpn_t::arg_6b:     return 6;
      case instruction_rpn_t::arg_7b:     return 7;
      case instruction_rpn_t::arg_8b:     return 8;

      case instruction_rpn_t::jmp:        return 8;
      // case instruction_rpn_t::jmp_1b:     return 1;
      // case instruction_rpn_t::jmp_2b:     return 2;
      // case instruction_rpn_t::jmp_3b:     return 3;
      // case instruction_rpn_t::jmp_4b:     return 4;
      // case instruction_rpn_t::jmp_5b:     return 5;
      // case instruction_rpn_t::jmp_6b:     return 6;
      // case instruction_rpn_t::jmp_7b:     return 7;
      // case instruction_rpn_t::jmp_8b:     return 8;

      case instruction_rpn_t::pop_jif:    return 8;
      // case instruction_rpn_t::pop_jif_1b: return 1;
      // case instruction_rpn_t::pop_jif_2b: return 2;
      // case instruction_rpn_t::pop_jif_3b: return 3;
      // case instruction_rpn_t::pop_jif_4b: return 4;
      // case instruction_rpn_t::pop_jif_5b: return 5;
      // case instruction_rpn_t::pop_jif_6b: return 6;
      // case instruction_rpn_t::pop_jif_7b: return 7;
      // case instruction_rpn_t::pop_jif_8b: return 8;

      case instruction_rpn_t::push:       return 8;
      case instruction_rpn_t::push_1b:    return 1;
      case instruction_rpn_t::push_2b:    return 2;
      case instruction_rpn_t::push_3b:    return 3;
      case instruction_rpn_t::push_4b:    return 4;
      case instruction_rpn_t::push_5b:    return 5;
      case instruction_rpn_t::push_6b:    return 6;
      case instruction_rpn_t::push_7b:    return 7;
      case instruction_rpn_t::push_8b:    return 8;

      case instruction_rpn_t::call:       return 0;
      case instruction_rpn_t::exit:       return 0;
      case instruction_rpn_t::ret:        return 0;
      case instruction_rpn_t::syscall:    return 0;
      default: return 0;
    }
  }

  std::string show_instruction(instruction_rpn_t cmd) {
    std::string str;
    switch (cmd) {
      case instruction_rpn_t::arg:     str += "arg";     break;
      case instruction_rpn_t::arg_1b:  str += "arg_1b";  break;
      case instruction_rpn_t::arg_2b:  str += "arg_2b";  break;
      case instruction_rpn_t::arg_3b:  str += "arg_3b";  break;
      case instruction_rpn_t::arg_4b:  str += "arg_4b";  break;
      case instruction_rpn_t::arg_5b:  str += "arg_5b";  break;
      case instruction_rpn_t::arg_6b:  str += "arg_6b";  break;
      case instruction_rpn_t::arg_7b:  str += "arg_7b";  break;
      case instruction_rpn_t::arg_8b:  str += "arg_8b";  break;

      case instruction_rpn_t::push:     str += "push";     break;
      case instruction_rpn_t::push_1b:  str += "push_1b";  break;
      case instruction_rpn_t::push_2b:  str += "push_2b";  break;
      case instruction_rpn_t::push_3b:  str += "push_3b";  break;
      case instruction_rpn_t::push_4b:  str += "push_4b";  break;
      case instruction_rpn_t::push_5b:  str += "push_5b";  break;
      case instruction_rpn_t::push_6b:  str += "push_6b";  break;
      case instruction_rpn_t::push_7b:  str += "push_7b";  break;
      case instruction_rpn_t::push_8b:  str += "push_8b";  break;

      case instruction_rpn_t::pop_jif: str += "pop_jif";         break;
      // case instruction_rpn_t::pop_jif_1b:  str += "pop_jif_1b";  break;
      // case instruction_rpn_t::pop_jif_2b:  str += "pop_jif_2b";  break;
      // case instruction_rpn_t::pop_jif_3b:  str += "pop_jif_3b";  break;
      // case instruction_rpn_t::pop_jif_4b:  str += "pop_jif_4b";  break;
      // case instruction_rpn_t::pop_jif_5b:  str += "pop_jif_5b";  break;
      // case instruction_rpn_t::pop_jif_6b:  str += "pop_jif_6b";  break;
      // case instruction_rpn_t::pop_jif_7b:  str += "pop_jif_7b";  break;
      // case instruction_rpn_t::pop_jif_8b:  str += "pop_jif_8b";  break;

      case instruction_rpn_t::jmp:     str += "jmp";     break;
      // case instruction_rpn_t::jmp_1b:  str += "jmp_1b";  break;
      // case instruction_rpn_t::jmp_2b:  str += "jmp_2b";  break;
      // case instruction_rpn_t::jmp_3b:  str += "jmp_3b";  break;
      // case instruction_rpn_t::jmp_4b:  str += "jmp_4b";  break;
      // case instruction_rpn_t::jmp_5b:  str += "jmp_5b";  break;
      // case instruction_rpn_t::jmp_6b:  str += "jmp_6b";  break;
      // case instruction_rpn_t::jmp_7b:  str += "jmp_7b";  break;
      // case instruction_rpn_t::jmp_8b:  str += "jmp_8b";  break;

      case instruction_rpn_t::call:    str += "call";    break;
      case instruction_rpn_t::exit:    str += "exit";    break;
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

  void code_t::write_i64(int64_t data, size_t pos) {
    write(&data, sizeof(data), pos);
  }

  void code_t::write_int(int64_t data, size_t size) {
    for (size_t i{}; i < size; ++i) {
      write_u8(static_cast<uint8_t>(data & 0xFF));
      data >>= 8;
    }
  }

  void code_t::write_cmd(instruction_rpn_t cmd, int64_t value) {
    switch (cmd) {
      case instruction_rpn_t::arg:
      case instruction_rpn_t::push: {
        auto value_new = zigzag_encode(value);
        auto size      = zigzag_size(value_new);
        write_u8(static_cast<uint8_t>(cmd) + size);
        write_int(value_new, size);
        break;
      }
      case instruction_rpn_t::jmp:
      case instruction_rpn_t::pop_jif: {
        write_u8(static_cast<uint8_t>(cmd));
        write_i64(value);
        break;
      }
      case instruction_rpn_t::call:
      case instruction_rpn_t::exit:
      case instruction_rpn_t::ret:
      case instruction_rpn_t::syscall: {
        write_u8(static_cast<uint8_t>(cmd));
        break;
      }
      default: throw utils_n::fatal_error_t("unknown instruction");
    }
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

  int64_t code_t::read_int(size_t& pos, size_t size) const {
    int64_t ret = {};
    for (size_t i{}; i < size; ++i) {
      size_t v = read_u8(pos);
      v <<= 8 *i;
      ret |= v;
    }
    ret = zigzag_decode(ret);
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
        case 1: str += "\t\t" + std::to_string(read_int(rip, 1)); break;
        case 2: str += "\t\t" + std::to_string(read_int(rip, 2)); break;
        case 3: str += "\t\t" + std::to_string(read_int(rip, 3)); break;
        case 4: str += "\t\t" + std::to_string(read_int(rip, 4)); break;
        case 5: str += "\t\t" + std::to_string(read_int(rip, 5)); break;
        case 6: str += "\t\t" + std::to_string(read_int(rip, 6)); break;
        case 7: str += "\t\t" + std::to_string(read_int(rip, 7)); break;
        case 8: str += "\t\t" + std::to_string(read_int(rip, 8)); break;
        case 0: break;
        default: str += " (unknown)";
      }
      str += "\n";
    }
    return str;
  }



  std::string code_ctx_t::save() const {
    std::string str;
    code_t code_writer;
    code_writer.write_i64(rip);
    code_writer.write_i64(rsp);
    return std::string(code_writer.buffer.begin(), code_writer.buffer.end())
      + std::string(code.buffer.begin(), code.buffer.end());
  }

  void code_ctx_t::load(const std::string& str) { // TODO
    if (str.size() < 2 * sizeof(size_t)) {
      throw utils_n::fatal_error_t("invalid pos");
    }

    size_t pos = {};
    code_t code_reader;
    code_reader.buffer.resize(str.size());
    std::copy(str.begin(), str.end(), code_reader.buffer.begin());
    rip = code_reader.read_i64(pos);
    rsp = code_reader.read_i64(pos);
    code.buffer.resize(code_reader.buffer.size() - 2* sizeof(size_t));
    std::copy(code_reader.buffer.begin() + 2 * sizeof(size_t), code_reader.buffer.end(), code.buffer.begin());
  }
}
