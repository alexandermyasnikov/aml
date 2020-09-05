#include <functional>
#include "executor.h"
#include "utils.h"

namespace aml::exec_n {
  namespace utils_n = aml::utils_n;



  bool machine_t::step(const code_n::code_t& code) {
    // DEBUG_LOGGER_EXEC("\t%s", machine.show().c_str());
    auto cmd = static_cast<code_n::instruction_rpn_t>(code.read_u8(rip));
    // DEBUG_LOGGER_EXEC("\tcmd: %s", code_t::show_instruction(cmd).c_str());

    if (stack.size() > 1000)
      throw utils_n::fatal_error_t("too big stack");

    switch (cmd) {
      case code_n::instruction_rpn_t::exit: {
        return false;

      } case code_n::instruction_rpn_t::arg: {
        int64_t value = code.read_i64(rip);
        // DEBUG_LOGGER_EXEC("value: %d", value);
        stack.push_back(stack.at(stack.size() - value));
        // DEBUG_LOGGER_EXEC("arg: %d", stack.back());
        break;

      } case code_n::instruction_rpn_t::call: {
        if (stack.empty())
          throw utils_n::fatal_error_t("invalid stack");

        int64_t arg_count = stack.back();
        // DEBUG_LOGGER_EXEC("arg_count: %d", arg_count);
        if (arg_count < 1 || arg_count > stack.size())
          throw utils_n::fatal_error_t("invalid stack");

        for (size_t i{}; i < arg_count - 1; ++i) {
          int64_t arg = stack.at(stack.size() - 1 - 1/*arg_count*/ - i);
          // DEBUG_LOGGER_EXEC("arg: %d", arg);
        }

        int64_t rip_new = stack.at(stack.size() - 1 - arg_count);
        // DEBUG_LOGGER_EXEC("rip: %d", rip);

        stack.push_back(rbp);
        stack.push_back(rip);
        rbp = stack.size() - 1/*rbp*/ - 1/*rip*/;
        rip = rip_new;
        break;

      } case code_n::instruction_rpn_t::jmp: {
        int64_t rip_new = code.read_i64(rip);
        // DEBUG_LOGGER_EXEC("rip: %d", rip);
        rip = rip_new;
        break;

      } case code_n::instruction_rpn_t::pop_jif: {
        if (stack.empty())
          throw utils_n::fatal_error_t("invalid stack");

        int64_t ret = stack.back();
        stack.pop_back();
        // DEBUG_LOGGER_EXEC("ret: %d", ret);

        int64_t rip_new = code.read_i64(rip);
        // DEBUG_LOGGER_EXEC("rip: %d", rip);

        if (!ret) {
          rip = rip_new;
          // DEBUG_LOGGER_EXEC("rip: %d", rip);
        }

        break;

      } case code_n::instruction_rpn_t::push8: {
        int64_t value = code.read_i64(rip);
        stack.push_back(value);
        // DEBUG_LOGGER_EXEC("value: %d", value);
        break;

      } case code_n::instruction_rpn_t::ret: {
        if (stack.size() < 4)
          throw utils_n::fatal_error_t("invalid stack");

        int64_t ret = stack.back();
        stack.pop_back();

        rip = stack.back();
        stack.pop_back();

        rbp = stack.back();
        stack.pop_back();

        int64_t arg_count = stack.back();
        stack.pop_back();

        if (stack.size() < arg_count)
          throw utils_n::fatal_error_t("invalid stack");

        for (size_t i{}; i < arg_count; ++i) {
          stack.pop_back();
        }

        stack.push_back(ret);
        break;

      } case code_n::instruction_rpn_t::syscall: {
        if (stack.empty())
          throw utils_n::fatal_error_t("invalid stack");

        int64_t arg_count = stack.back();
        stack.pop_back();
        // DEBUG_LOGGER_EXEC("arg_count: %d", arg_count);
        if (arg_count > stack.size())
          throw utils_n::fatal_error_t("invalid stack");

        int64_t ret = -1;
        int64_t op  = -1;
        if (arg_count > 0) {
          op = stack.back();
          stack.pop_back();
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
            int64_t opnd1 = stack.back();
            stack.pop_back();
            arg_count--;
            ret = !opnd1;
            // DEBUG_LOGGER_EXEC("%d: %d => %d", op, opnd1, ret);

          } else if (arg_count == 2 && op >= 200 && op < 200 + ops2.size()) {
            int64_t opnd1 = stack.back();
            stack.pop_back();
            arg_count--;
            int64_t opnd2 = stack.back();
            stack.pop_back();
            arg_count--;

            ret = ops2[op - 200](opnd1, opnd2);
            // DEBUG_LOGGER_EXEC("%d: %d %d => %d", op, opnd1, opnd2, ret);
          }
        }

        if (arg_count) {
          // DEBUG_LOGGER_EXEC("invalid syscall: %d : %d", op, arg_count);
          for (size_t i{}; i < arg_count; ++i) {
            int64_t arg = stack.back();
            stack.pop_back();
            // DEBUG_LOGGER_EXEC("arg: %d", arg);
          }
        }

        stack.push_back(ret);
        break;

      } default: throw utils_n::fatal_error_t("unknown cmd");
    }
    return true;
  }

  std::string machine_t::show() const {
    std::string str;
    str += "size: " + std::to_string(stack.size()) + "\t";
    str += "rbp: " + std::to_string(rbp) + "\t";
    str += "rip: " + std::to_string(rip) + "\t";
    str += "stack: ";
    for (const auto& v : stack) {
      str += std::to_string(v) + " ";
    }
    return str;
  }
}
