#include "code_segment.h"

#include <functional>

#include "utils.h"
#include "logger.h"

namespace aml::code_n {
  namespace utils_n = aml::utils_n;



  cmd_t::cmd_t(cmd_id_t cmd_id, int64_t value) {
    bits.id  = static_cast<uint8_t>(cmd_id) & ((1 << 4) - 1);
    bits.ext = {};
    bits.len = {};
    val = value;
  }

  void cmd_t::encode() {
    // id, val -> ext, len, val
    AML_LOGGER(debug, "cmd before: {} {} {} {:08b} {} \t {}",
        static_cast<size_t>(bits.id), static_cast<size_t>(bits.ext), static_cast<size_t>(bits.len), cmd, val, show());
    bits.ext = {};
    bits.len = {};
    switch (static_cast<cmd_id_t>(bits.id)) {
      case cmd_id_t::arg:
      case cmd_id_t::jmp:
      case cmd_id_t::pop:
      case cmd_id_t::pop_jif:
      case cmd_id_t::push:
      case cmd_id_t::var:
      {
        if (val >= -2 && val <= 5) {
          bits.ext = 1;
          bits.len = (val - -2) & ((1 << 3) - 1);
        } else {
          val = zigzag_encode(val);
          bits.len = (zigzag_size(val) - 1) & ((1 << 3) - 1);
        }
        break;
      }
      case cmd_id_t::call:
      case cmd_id_t::exit:
      case cmd_id_t::ret:
      case cmd_id_t::syscall:
      {
        break;
      }
      default:
      {
        AML_LOGGER(err, "unknown cmd: {} {}", static_cast<uint8_t>(bits.id), show());
        break;
      }
    }
    AML_LOGGER(debug, "cmd after:  {} {} {} {:08b} {} \t {}",
        static_cast<size_t>(bits.id), static_cast<size_t>(bits.ext), static_cast<size_t>(bits.len), cmd, val, show());
  }

  void cmd_t::decode() {
    // id, ext, len, value -> value
    AML_LOGGER(debug, "cmd before: {} {:02x} {:08b} {}", static_cast<size_t>(bits.id), static_cast<size_t>(bits.id), cmd, val);
    if (bits.ext) {
      val = bits.len + -2;
    } else {
      val = zigzag_decode(val);
    }
    AML_LOGGER(debug, "cmd after:  {} {:02x} {:08b} {}", static_cast<size_t>(bits.id), static_cast<size_t>(bits.id), cmd, val);
  }

  std::string cmd_t::show() const {
    std::string str;
    switch (static_cast<cmd_id_t>(bits.id)) {
      case cmd_id_t::arg:     str += "arg";     break;
      case cmd_id_t::call:    str += "call";    break;
      case cmd_id_t::exit:    str += "exit";    break;
      case cmd_id_t::jmp:     str += "jmp";     break;
      case cmd_id_t::pop_jif: str += "pop_jif"; break;
      case cmd_id_t::pop:     str += "pop";     break;
      case cmd_id_t::push:    str += "push";    break;
      case cmd_id_t::ret:     str += "ret";     break;
      case cmd_id_t::syscall: str += "syscall"; break;
      case cmd_id_t::var:     str += "var";     break;
      default: str += "unknown";
    }
    switch (static_cast<cmd_id_t>(bits.id)) {
      case cmd_id_t::arg:     str += " " + std::to_string(val); break;
      case cmd_id_t::call:                                      break;
      case cmd_id_t::exit:                                      break;
      case cmd_id_t::jmp:     str += " " + std::to_string(val); break;
      case cmd_id_t::pop_jif: str += " " + std::to_string(val); break;
      case cmd_id_t::pop:     str += " " + std::to_string(val); break;
      case cmd_id_t::push:    str += " " + std::to_string(val); break;
      case cmd_id_t::ret:                                       break;
      case cmd_id_t::syscall:                                   break;
      case cmd_id_t::var:     str += " " + std::to_string(val); break;
      default: str += " unknown";
    }
    return str;
  }



  int64_t zigzag_encode(int64_t value) {
    return (value << 1) ^ (value >> (8 * sizeof(int64_t) - 1));
  }

  int64_t zigzag_decode(int64_t value) {
    return (value >> 1) ^ (-(value & 1));
  }

  uint8_t zigzag_size(int64_t value) {
    uint8_t ret = {};
    for (; ret < 8 && value; ++ret) {
      value >>= 8;
    }
    return std::max(ret, uint8_t(1));
  }



  void code_t::write(const void* data, size_t size, size_t pos) {
    pos = std::min(pos, buffer.size());
    for (size_t i{}; i < size; ++i, ++pos) {
      if (pos < buffer.size()) {
        buffer[pos] = static_cast<const uint8_t*>(data)[i];
      } else {
        buffer.push_back(static_cast<const uint8_t*>(data)[i]);
      }
    }
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

  void code_t::write_cmd(cmd_t cmd) {
    cmd.encode();
    AML_LOGGER(debug, "cmd: {} {:08b} {} \t {}", static_cast<size_t>(cmd.bits.id), cmd.cmd, cmd.val, cmd.show());
    write_u8(cmd.cmd);
    if (!cmd.bits.ext) {
      write_int(cmd.val, cmd.bits.len + 1);
    }
  }

  void code_t::read(void* data, size_t size, size_t& pos) const {
    if (buffer.empty())
      return;

    for (size_t i{}; i < size; ++i) {
      static_cast<uint8_t*>(data)[i] = buffer[pos % buffer.size()];
      pos++;
    }
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
    return ret;
  }

  cmd_t code_t::read_cmd(size_t& pos) const {
    cmd_t cmd = {};
    cmd.cmd = read_u8(pos);
    if (!cmd.bits.ext) {
      cmd.val = read_int(pos, cmd.bits.len + 1);
    }
    cmd.decode();
    AML_LOGGER(debug, "cmd: {} {:08b} {} \t {}", static_cast<size_t>(cmd.bits.id), cmd.cmd, cmd.val, cmd.show());
    return cmd;
  }

  std::string code_t::show() const {
    std::string str;
    size_t rip = {};
    while (rip < buffer.size()) {
      str += std::to_string(rip) + "\t\t";
      cmd_t cmd = read_cmd(rip);
      str += cmd.show();
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



  bool stack_t::step(const code_t& code) {
    AML_TRACER;
    auto cmd = code.read_cmd(rip);
    AML_LOGGER(debug, "cmd:  {}", cmd.show());
    AML_LOGGER(debug, "size: {}", size());
    AML_LOGGER(debug, "rip:  {}", rip);
    AML_LOGGER(debug, "rbp:  {}", rbp);

    if (size() > 1000)
      throw utils_n::fatal_error_t("too big stack");

    switch (static_cast<cmd_id_t>(cmd.bits.id)) {
      case cmd_id_t::arg:
      {
        push_back(get(size() - cmd.val));
        break;
      }

      case cmd_id_t::call:
      {
        size_t arg_count = back();
        int64_t rip_new = get(size() - 1 - arg_count);

        push_back(rbp);
        push_back(rip);
        rbp = size() - 1/*rbp*/ - 1/*rip*/;
        rip = rip_new;
        break;
      }

      case cmd_id_t::exit:
      {
        return false;
      }

      case cmd_id_t::jmp:
      {
        rip += cmd.val;
        break;
      }

      case cmd_id_t::pop:
      {
        int64_t ret = back();
        for (int64_t i{}; i < cmd.val; ++i) {
          pop_back();
        }
        push_back(ret);
        break;
      }

      case cmd_id_t::pop_jif:
      {
        int64_t offset = cmd.val;
        int64_t ret = back();
        pop_back();

        if (!ret) {
          rip += offset;
        }
        break;
      }

      case cmd_id_t::push:
      {
        push_back(cmd.val);
        break;
      }

      case cmd_id_t::ret:
      {
        int64_t ret = back();
        pop_back();

        rip = back();
        pop_back();

        rbp = back();
        pop_back();

        size_t arg_count = back();
        pop_back();

        for (size_t i{}; i < arg_count; ++i) {
          pop_back();
        }

        push_back(ret);
        break;
      }

      case cmd_id_t::syscall:
      {
        size_t arg_count = back();
        pop_back();

        int64_t ret = -1;
        size_t op  = -1;
        if (arg_count > 0) {
          op = back();
          pop_back();
          arg_count--;

          static std::vector<std::function<int64_t(int64_t, int64_t)>> ops2 = {
            /*200*/ [](int64_t a, int64_t b) { return a + b; },
            /*201*/ [](int64_t a, int64_t b) { return a - b; },
            /*202*/ [](int64_t a, int64_t b) { return a * b; },
            /*203*/ [](int64_t a, int64_t b) { return !b ? 0 : a / b; },
            /*204*/ [](int64_t a, int64_t b) { return a == b; },
            /*205*/ [](int64_t a, int64_t b) { return a < b; },
            /*206*/ [](int64_t a, int64_t b) { return a && b; },
            /*207*/ [](int64_t a, int64_t b) { return a || b; },
          };

          if (arg_count == 1 && op == 100) {
            int64_t opnd1 = back();
            pop_back();
            arg_count--;
            ret = !opnd1;

          } else if (arg_count == 2 && op >= 200 && op < 200 + ops2.size()) {
            int64_t opnd1 = back();
            pop_back();
            arg_count--;
            int64_t opnd2 = back();
            pop_back();
            arg_count--;

            ret = ops2[op - 200](opnd1, opnd2);
          }
        }

        if (arg_count) {
          for (size_t i{}; i < arg_count; ++i) {
            pop_back();
          }
        }

        push_back(ret);
        break;
      }

      case cmd_id_t::var:
      {
        push_back(get(rbp + 1/*rbp*/ + 1/*rip*/ + cmd.val));
        break;
      }

      default:
      {
        AML_LOGGER(err, "unknown cmd: {} {}", static_cast<uint8_t>(cmd.bits.id), cmd.show());
        break;
      }
    }

    return true;
  }

  size_t stack_t::size() const {
    return buffer.size();
  }

  int64_t& stack_t::get(size_t pos) {
    static int64_t fake_value = {};
    if (buffer.empty())
      return fake_value;

    return buffer[pos % buffer.size()];
  }

  int64_t& stack_t::back() {
    return get(buffer.size() - 1);
  }

  void stack_t::push_back(int64_t value) {
    if (buffer.size() >= limit)
      buffer.pop_front();

    buffer.push_back(value);
  }

  void stack_t::pop_back() {
    if (!buffer.empty())
      buffer.pop_back();
  }

  std::string stack_t::show() const {
    std::string str;
    str += "size: " + std::to_string(buffer.size()) + "\t";
    str += "rbp: " + std::to_string(rbp) + "\t";
    str += "rip: " + std::to_string(rip) + "\t";
    str += "stack: ";
    for (const auto& v : buffer) {
      str += std::to_string(v) + " ";
    }
    return str;
  }
}
