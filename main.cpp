
#include <iostream>
#include <variant>
#include <iomanip>
#include <regex>

#include "debug_logger.h"

#define DEBUG_LOGGER_TRACE_LA            DEBUG_LOGGER("la   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_LA(...)             DEBUG_LOG("la   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_SA            DEBUG_LOGGER("sa   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_SA(...)             DEBUG_LOG("sa   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_ICG           DEBUG_LOGGER("icg  ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_ICG(...)            DEBUG_LOG("icg  ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_CG            DEBUG_LOGGER("cg   ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_CG(...)             DEBUG_LOG("cg   ", logger_indent_aml_t::indent, __VA_ARGS__)

#define DEBUG_LOGGER_TRACE_EXEC          DEBUG_LOGGER("exec ", logger_indent_aml_t::indent)
#define DEBUG_LOGGER_EXEC(...)           DEBUG_LOG("exec ", logger_indent_aml_t::indent, __VA_ARGS__)

template <typename T>
struct logger_indent_t { static inline int indent = 0; };

struct logger_indent_aml_t : logger_indent_t<logger_indent_aml_t> { };


/*
struct segment_t {
  uint64_t type;
  std::deque<uint64_t> data;
};

struct segment_text_t {
  std::deque<uint64_t> data;
};

struct library_file_t {
  std::deque<segment_t> segments;
};
*/

namespace aml_n {

  namespace utils_n {

    template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
    template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

    using data_t = std::vector<uint8_t>;

    struct fatal_error : std::runtime_error {
      fatal_error(const std::string& msg = "unknown error") : std::runtime_error(msg) { }
    };
  }



  namespace lexical_analyzer_n {

    using namespace utils_n;

    struct lexeme_empty_t   { };
    struct lexeme_lp_t      { };
    struct lexeme_rp_t      { };
    struct lexeme_func_t    { };
    struct lexeme_ret_t     { };
    struct lexeme_integer_t { int64_t     value;  };
    struct lexeme_arg_t     { size_t      number; };
    struct lexeme_ident_t   { std::string value;  };

    using lexeme_t = std::variant<
      lexeme_empty_t,
      lexeme_lp_t,
      lexeme_rp_t,
      lexeme_func_t,
      lexeme_ret_t,
      lexeme_integer_t,
      lexeme_arg_t,
      lexeme_ident_t>;

    using lexemes_t = std::deque<lexeme_t>;

    struct rule_t {
      std::regex regex;
      std::function<lexeme_t(const std::string&)> get_lexeme;
    };

    static inline std::vector<rule_t> rules = {
      {
        std::regex(R"(\s+)"),
        [](const std::string&) { return lexeme_empty_t{}; }
      }, {
        std::regex(R"(;.*?\n)"),
        [](const std::string&) { return lexeme_empty_t{}; }
      }, {
        std::regex(R"(\()"),
        [](const std::string&) { return lexeme_lp_t{}; }
      }, {
        std::regex(R"(\))"),
        [](const std::string&) { return lexeme_rp_t{}; }
      }, {
        std::regex(R"(func)"),
          [](const std::string& str) { return lexeme_func_t{}; }
      }, {
        std::regex(R"(ret)"),
          [](const std::string& str) { return lexeme_ret_t{}; }
      }, {
        std::regex(R"([-+]?\d+)"),
          [](const std::string& str) { return lexeme_integer_t{std::stol(str)}; }
      }, {
        std::regex(R"(\$(\d+))"),
          [](const std::string& str) { return lexeme_arg_t{std::stoull(str)}; }
      }, {
        std::regex(R"(\w+)"),
          [](const std::string& str) { return lexeme_ident_t{str}; }
      }
    };

    lexemes_t process(const std::string& code) {
      DEBUG_LOGGER_TRACE_LA;

      lexemes_t lexemes;
      std::string s{code};
      std::smatch m;
      std::regex_constants::match_flag_type flags =
          std::regex_constants::match_continuous | std::regex_constants::match_not_null;

      bool run = true;
      while (run && !s.empty()) {
        run = false;
        for (const auto& rule : rules) {
          if (std::regex_search(s, m, rule.regex, flags)) {
            lexeme_t lexeme = rule.get_lexeme(m.size() > 1 ? m[1].str() : m.str());
            if (!std::get_if<lexeme_empty_t>(&lexeme)) {
              lexemes.push_back(lexeme);
            }
            s = m.suffix().str();
            run = true;
            break;
          }
        }
      }

      if (!s.empty())
        throw fatal_error("invalid lexeme");

      return lexemes;
    }

    static std::string show_lexeme(const lexeme_t& lexeme) {
      std::string str;
      std::visit(overloaded {
          [&str] (lexeme_empty_t)                { str = "(empty)"; },
          [&str] (lexeme_lp_t)                   { str = "("; },
          [&str] (lexeme_rp_t)                   { str = ")"; },
          [&str] (lexeme_func_t)                 { str = "func"; },
          [&str] (lexeme_ret_t)                  { str = "ret"; },
          [&str] (const lexeme_integer_t &value) { str = std::to_string(value.value); },
          [&str] (const lexeme_arg_t     &value) { str = "$" + std::to_string(value.number); },
          [&str] (const lexeme_ident_t   &value) { str = value.value; },
          [&str] (auto)                          { str = "(unknown)"; },
          }, lexeme);
      return str;
    }

    static std::string show(const lexemes_t& lexemes) {
      std::string str;
      for (const auto& lexeme : lexemes) {
        str += show_lexeme(lexeme) + " ";
      }
      return str;
    }
  }



  namespace syntax_lisp_analyzer_n {

    using namespace utils_n;
    using namespace lexical_analyzer_n;

    struct syntax_lisp_tree_t {
      using node_t = lexeme_t;
      using nodes_t = std::deque<syntax_lisp_tree_t>;

      node_t  node  = lexeme_empty_t{};
      nodes_t nodes = {};

      bool is_leaf() const {
        return !std::get_if<lexeme_empty_t>(&node);
      }
    };

    syntax_lisp_tree_t process(const lexemes_t& lexemes) {
      std::stack<syntax_lisp_tree_t> stack;
      stack.push(syntax_lisp_tree_t{});
      for (const auto& lexeme : lexemes) {
        if (std::get_if<lexeme_lp_t>(&lexeme)) {
          stack.push(syntax_lisp_tree_t{});
        } else if (std::get_if<lexeme_rp_t>(&lexeme)) {
          if (stack.empty())
            throw fatal_error("syntax_lisp_analyzer: unexpected ')'");
          auto top = stack.top();
          stack.pop();
          stack.top().nodes.push_back(top);
        } else {
          stack.top().nodes.push_back(syntax_lisp_tree_t{.node = lexeme});
        }
      }

      if (stack.size() != 1)
        throw fatal_error("syntax_lisp_analyzer: parse error");

      return stack.top();
    }

    static std::string show(const syntax_lisp_tree_t& syntax_lisp_tree) {
      std::string str;

      if (syntax_lisp_tree.is_leaf()) {
        str += show_lexeme(syntax_lisp_tree.node);
      } else {
        str += "( ";
        for (const auto& node : syntax_lisp_tree.nodes) {
          str += show(node) + " ";
        }
        str += ")";
      }

      return str;
    }
  }



  namespace syntax_analyzer_n {

    using namespace utils_n;
    using namespace syntax_lisp_analyzer_n;

    // TODO
    // GRAMMAR

    struct state_t {
      using cb_t = std::function<void()>;
      cb_t cb;
    };

    std::vector<state_t> states;

    struct program_stmt_t;
    struct func_stmt_t;

    struct program_stmt_t {
      std::deque<std::shared_ptr<func_stmt_t>> funcs;
    };

    struct func_stmt_t {
      std::shared_ptr<std::string> name;
      // std::shared_ptr<expr_stmt_t> body;
    };



    // using syntax_tree_lisp_t = syntax_analyzer_lisp_t::syntax_tree_lisp_t;
    // using lexeme_list_sptr_t = syntax_analyzer_lisp_t::lexeme_list_sptr_t;
    // using lexeme_ident_t     = lexical_analyzer_n::lexeme_ident_t;
    // using lexeme_t           = lexical_analyzer_n::lexeme_t;

    program_stmt_t process(const syntax_lisp_tree_t& syntax_lisp_tree) {
      program_stmt_t program_stmt;

      ;

      return program_stmt;
    }
  }



  namespace semantic_analyzer_n {

  }



#if 0
  namespace intermediate_code_generator_n {

    using namespace utils_n;
    using namespace syntax_analyzer_n;

    union instruction_t {
      uint16_t value;
      struct {
        uint8_t  op  : 4;
        uint8_t  rd  : 4;
        uint8_t  rs1 : 4;
        uint8_t  rs2 : 4;
      } __attribute__((packed)) cmd;
      struct {
        uint8_t  op : 4;
        uint8_t  rd : 4;
        uint8_t  val;
      } __attribute__((packed)) cmd_set;
    };

    using instructions_t = std::vector<instruction_t>;

    struct opcode_index_t {
      uint8_t     offset;
      uint8_t     index;
      std::string name;
    };

    static inline std::vector<opcode_index_t> opcodes_table = {
      {  0,  0, "SET"  },
      {  0,  1, "AND"  },
      {  0,  2, "OR"   },
      {  0,  3, "XOR"  },
      {  0,  4, "ADD"  },
      {  0,  5, "SUB"  },
      {  0,  6, "MULT" },
      {  0,  7, "DIV"  },
      {  0,  8, "LSH"  },
      {  0,  9, "RSH"  },
      // ...
      {  0, 15, "OTH0" },
      {  1,  0, "BR"   },
      {  1,  1, "NOT"  },
      {  1,  2, "LOAD" },
      {  1,  3, "SAVE" },
      {  1,  4, "MOV"  },
      // ...
      {  1, 15, "OTH1" },
      {  2,  0, "CALL" },
      // ...
      {  2, 15, "OTH2" },
      {  3,  0, "RET"  },
      // ...
    };

    struct reg_index_t {
      uint8_t     index;
      std::string name;
    };

    static inline std::vector<reg_index_t> regs_table = {
      {   0, "RI"  },   // instruction pointer
      {   1, "RP"  },   // previous base pointer
      {   2, "RB"  },   // base pointer
      {   3, "RS"  },   // stack pointer
      {   4, "RF"  },   // flags
      {   5, "RT"  },   // tmp
      {   6, "RC"  },   // const
      {   7, "RA"  },   // args
      {   8, "R1"  },
      {   9, "R2"  },
      {  10, "R3"  },
      {  11, "R4"  },
      {  12, "R5"  },
      {  13, "R6"  },
      {  14, "R7"  },
      {  15, "R8"  },
    };

    using reg_value_t = int64_t;
    using reg_uvalue_t = std::make_unsigned<reg_value_t>::type;

    // INC1     Ra:      set(Rt, 1); INC(Ra, Rt);
    // DEC1     Ra:      set(Rt, 1); DEC(Ra, Rt);
    // SETF     Ra:      or(Rf, Rf, Ra);
    // CLRF     Ra:      not(Rt, Rf); or(Rt, Rt, Ra); not(Rf, Rt);
    // NEG      Ra:      set(Rt, 0); sub(Ra, Rt, Ra);
    // PLOADI   Ra  I:   set(Rt, I); add(Rt, Rt, SP); load(Ra, Rt);
    // PLOAD    Ra Rb:   add(Rt, Rb, SP); load(Ra, Rt);
    // PSAVE    Ra Rb:   add(Rt, Rb, SP); save(Rt, Ra);
    // INC      Ra Rb:   add(Ra, Ra, Rb);
    // DEC      Ra Rb:   sub(Ra, Ra, Rb);

    using functions_t = std::map<std::string, size_t>;

    uint8_t opcode_index(uint8_t offset, const std::string& name) {
      auto it = std::find_if(opcodes_table.begin(), opcodes_table.end(),
          [offset, name](auto& opcode) { return opcode.offset == offset && opcode.name == name; });
      if (it == opcodes_table.end()) {
        throw fatal_error("unknown opcode");
      }
      return it->index;
    }

    std::string opcode_name(uint8_t offset, uint8_t index) {
      auto it = std::find_if(opcodes_table.begin(), opcodes_table.end(),
          [offset, index](auto& opcode) { return opcode.offset == offset && opcode.index == index; });
      if (it == opcodes_table.end()) {
        throw fatal_error("unknown opcode");
      }
      return it->name;
    }

    uint8_t reg_index(const std::string& name) {
      auto it = std::find_if(regs_table.begin(), regs_table.end(),
          [name](auto& reg) { return reg.name == name; });
      if (it == regs_table.end()) {
        throw fatal_error("unknown reg");
      }
      return it->index;
    }

    std::string reg_name(uint8_t index) {
      auto it = std::find_if(regs_table.begin(), regs_table.end(),
          [index](auto& reg) { return reg.index == index; });
      if (it == regs_table.end()) {
        throw fatal_error("unknown reg");
      }
      return it->name;
    }

    std::string print_instruction(instruction_t instruction) {
      std::stringstream ss;

      ss << std::hex << std::setfill('0') << std::setw(2 * sizeof(instruction.value))
          << instruction.value << "   ";

      if (instruction.cmd_set.op == opcode_index(0, "SET")) {
        ss << opcode_name(0, instruction.cmd_set.op) << " "
          << reg_name(instruction.cmd_set.rd) << " "
          << (reg_value_t) instruction.cmd_set.val << " ";
      } else {
        if (instruction.cmd.op == opcode_index(0, "OTH0")) {
          if (instruction.cmd.rd == opcode_index(1, "OTH1")) {
            if (instruction.cmd.rs1 == opcode_index(2, "OTH2")) {
                ss << opcode_name(3, instruction.cmd.rs2) << " ";
            } else {
              ss << opcode_name(2, instruction.cmd.rs1) << " "
                << reg_name(instruction.cmd.rs2) << " ";
            }
          } else {
            ss << opcode_name(1, instruction.cmd.rd) << " "
              << reg_name(instruction.cmd.rs1) << " "
              << reg_name(instruction.cmd.rs2) << " ";
          }
        } else {
          ss << opcode_name(0, instruction.cmd.op) << " "
            << reg_name(instruction.cmd.rd) << " "
            << reg_name(instruction.cmd.rs1) << " "
            << reg_name(instruction.cmd.rs2) << " ";
        }
      }

      return ss.str();
    };

    void macro_set(instructions_t& instructions, uint8_t rd, reg_value_t value) {
      DEBUG_LOGGER_TRACE_ICG;
      // DEBUG_LOGGER_ICG("rd: '%x'", rd);
      // DEBUG_LOGGER_ICG("value: '%ld'", value);

      uint8_t bytes[sizeof(value)];
      memcpy(bytes, &value, sizeof(value));
      std::reverse(std::begin(bytes), std::end(bytes));

      size_t i = 0;
      for (; i < sizeof(bytes); i++) {
        if (bytes[i])
          break;
      }

      instructions.push_back({ .cmd_set = { opcode_index(0, "SET"), rd, 0 } });

      auto rt = reg_index("RT");

      for (; i < sizeof(bytes); i++) {
        instructions.push_back({ .cmd_set = { opcode_index(0, "SET"), rt, 8 } });
        instructions.push_back({ .cmd     = { opcode_index(0, "LSH"), rd, rd, rt } });
        instructions.push_back({ .cmd_set = { opcode_index(0, "SET"), rt, bytes[i] } });
        instructions.push_back({ .cmd     = { opcode_index(0, "OR"),  rd, rd, rt } });
      }
    }

    void process(instructions_t& instructions, functions_t& functions, const cmds_str_t& cmds_str) {
      for (auto cmd_str : cmds_str) {
        if (cmd_str.at(0) == "SET" && cmd_str.size() == 3) {
          auto op = opcode_index(0, cmd_str.at(0));
          auto rd = reg_index(cmd_str.at(1));
          reg_value_t value = strtol(cmd_str.at(2).c_str(), nullptr, 0);
          macro_set(instructions, rd, value);

        } else if (cmd_str.at(0) == "FUNCTION" && cmd_str.size() == 2) {
          auto name = cmd_str.at(1);

          if (functions.find(name) != functions.end())
            throw fatal_error("function exists");

          functions[name] = instructions.size() * sizeof(instruction_t);

        } else if (cmd_str.at(0) == "LABEL") {
          throw fatal_error("LABEL TODO");

        } else if (cmd_str.at(0) == "ADDRESS" && cmd_str.size() == 3) {
          auto rd   = reg_index(cmd_str.at(1));
          auto name = cmd_str.at(2);

          if (functions.find(name) == functions.end())
            throw fatal_error("function not exists");

          macro_set(instructions, rd, functions[name]);

        } else if (cmd_str.size() == 4) {
          auto op  = opcode_index(0, cmd_str.at(0));
          auto rd  = reg_index(cmd_str.at(1));
          auto rs1 = reg_index(cmd_str.at(2));
          auto rs2 = reg_index(cmd_str.at(3));
          instructions.push_back({ .cmd  = { op, rd, rs1, rs2 } });

        } else if (cmd_str.size() == 3) {
          auto op1 = opcode_index(0, "OTH0");
          auto op2 = opcode_index(1, cmd_str.at(0));
          auto rd  = reg_index(cmd_str.at(1));
          auto rs  = reg_index(cmd_str.at(2));
          instructions.push_back({ .cmd  = { op1, op2, rd, rs } });

        } else if (cmd_str.size() == 2) {
          auto op1 = opcode_index(0, "OTH0");
          auto op2 = opcode_index(1, "OTH1");
          auto op3 = opcode_index(2, cmd_str.at(0));
          auto rd  = reg_index(cmd_str.at(1));
          instructions.push_back({ .cmd  = { op1, op2, op3, rd } });

        } else if (cmd_str.size() == 1) {
          auto op1 = opcode_index(0, "OTH0");
          auto op2 = opcode_index(1, "OTH1");
          auto op3 = opcode_index(2, "OTH2");
          auto op4 = opcode_index(3, cmd_str.at(0));
          instructions.push_back({ .cmd  = { op1, op2, op3, op4 } });

        } else {
          throw fatal_error("unknown cmd format");
        }
      }

      for (size_t i = 0; i < instructions.size(); ++i) {
        DEBUG_LOGGER_ICG("instruction: %08x '%s'", i * sizeof(instruction_t), print_instruction(instructions[i]).c_str());
      }
    }
  }



  namespace code_optimizer_n {
  }



  namespace code_generator_n {
    using namespace intermediate_code_generator_n;

    void process(data_t& text, const instructions_t& instructions) {
      text.assign(sizeof(instruction_t) * instructions.size(), 0);
      for (size_t i = 0; i < instructions.size(); ++i) {
        memcpy(text.data() + i * sizeof(instruction_t), &instructions[i].value, sizeof(instruction_t));
      }

      for (size_t i = 0; i < text.size(); i += sizeof(instruction_t)) {
        DEBUG_LOGGER_CG("text: '%02hhx%02hhx'", text.at(i), text.at(i + 1));
      }
    }
  }
#endif



#if 0
  namespace executor_n {

    using namespace intermediate_code_generator_n;

    using registers_set_t = reg_value_t[16];

    std::string print_stack(const data_t& stack, registers_set_t* registers_set) {
      std::stringstream ss;
      ss << std::endl;

      for (uint8_t i = 0; i < 16; ++i) {
        ss << reg_name(i) << "   " << std::hex << std::setfill('0') << std::setw(2 * sizeof(reg_value_t))
          << (*registers_set)[i] << std::endl;
      }

      ss << "size: " << (reinterpret_cast<const uint8_t*>(registers_set) - reinterpret_cast<const uint8_t*>(stack.data())) << std::endl;

      for (size_t i = (*registers_set)[reg_index("RB")]; i < (*registers_set)[reg_index("RS")]; ++i) {
        ss << "stack data: " << std::hex << std::setfill('0') << std::setw(2 * sizeof(uint8_t))
          << (uint64_t) stack.at(i) << std::endl;
      }

      return ss.str();
    }

    void exec_cmd3(const data_t& text, data_t& stack, registers_set_t*& registers_set, instruction_t instruction) {
      if (instruction.cmd.rs2 == opcode_index(3, "RET")) {
        if (!(*registers_set)[reg_index("RP")])
          throw fatal_error("exit TODO");
        registers_set_t* registers_set_new = reinterpret_cast<registers_set_t*>(stack.data()
            + (*registers_set)[reg_index("RP")] - sizeof(registers_set_t));
        registers_set = registers_set_new;
      } else {
        throw fatal_error("unknown cmd3");
      }
    }

    void exec_cmd2(const data_t& text, data_t& stack, registers_set_t*& registers_set, instruction_t instruction) {
      if (instruction.cmd.rs1 == opcode_index(2, "CALL")) {
        registers_set_t* registers_set_new = reinterpret_cast<registers_set_t*>(stack.data()
            + (*registers_set)[reg_index("RS")]);
        (*registers_set_new)[reg_index("RI")] = (*registers_set)[instruction.cmd.rs2];
        (*registers_set_new)[reg_index("RP")] = (*registers_set)[reg_index("RB")];
        (*registers_set_new)[reg_index("RB")] = (*registers_set)[reg_index("RS")] + sizeof(registers_set_t);
        (*registers_set_new)[reg_index("RS")] = (*registers_set_new)[reg_index("RB")];
        registers_set = registers_set_new;
      } else if (instruction.cmd.rs1 == opcode_index(2, "OTH2")) {
        exec_cmd3(text, stack, registers_set, instruction);
      } else {
        throw fatal_error("unknown cmd2");
      }
    }

    void exec_cmd1(const data_t& text, data_t& stack, registers_set_t*& registers_set, instruction_t instruction) {
      if (instruction.cmd.rd == opcode_index(1, "BR")) {
        throw fatal_error("BR TODO");
      } else if (instruction.cmd.rd == opcode_index(1, "NOT")) {
        (*registers_set)[instruction.cmd.rs1] = ~(*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.rd == opcode_index(1, "LOAD")) {
        throw fatal_error("LOAD TODO");
      } else if (instruction.cmd.rd == opcode_index(1, "SAVE")) {
        throw fatal_error("SAVE TODO");
      } else if (instruction.cmd.rd == opcode_index(1, "MOV")) {
        (*registers_set)[instruction.cmd.rs1] = (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.rd == opcode_index(1, "OTH1")) {
        exec_cmd2(text, stack, registers_set, instruction);
      } else {
        throw fatal_error("unknown cmd1");
      }
    }

    void exec_cmd0(const data_t& text, data_t& stack, registers_set_t*& registers_set, instruction_t instruction) {
      if (instruction.cmd.op == opcode_index(0, "SET")) {
        (*registers_set)[instruction.cmd_set.rd] = instruction.cmd_set.val;
      } else if (instruction.cmd.op == opcode_index(0, "AND")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] & (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "OR")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] | (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "XOR")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] ^ (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "AND")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] + (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "SUB")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] - (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "MULT")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] * (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "DIV")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] / (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "LSH")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] << (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "RSH")) {
        (*registers_set)[instruction.cmd.rd] =
          (*registers_set)[instruction.cmd.rs1] >> (*registers_set)[instruction.cmd.rs2];
      } else if (instruction.cmd.op == opcode_index(0, "OTH0")) {
        exec_cmd1(text, stack, registers_set, instruction);
      } else {
        throw fatal_error("unknown cmd0");
      }
    }

    void process(const data_t& text, const functions_t& functions) {
      DEBUG_LOGGER_TRACE_EXEC;

      if (functions.find("__start") == functions.end())
        throw fatal_error("__start not exists");

      data_t stack(0xFFFF, 0);

      registers_set_t* registers_set = reinterpret_cast<registers_set_t*>(stack.data());
      (*registers_set)[reg_index("RP")] = 0;
      (*registers_set)[reg_index("RI")] = functions.at("__start");
      (*registers_set)[reg_index("RB")] = sizeof(registers_set_t);
      (*registers_set)[reg_index("RS")] = (*registers_set)[reg_index("RB")];

      DEBUG_LOGGER_EXEC("stack frame: '%s'", print_stack(stack, registers_set).c_str());

      while (true) {
        instruction_t instruction = *reinterpret_cast<const instruction_t*>(text.data() + (*registers_set)[reg_index("RI")]);
        exec_cmd0(text, stack, registers_set, instruction);

        DEBUG_LOGGER_EXEC("instruction: '%s'", print_instruction(instruction).c_str());
        DEBUG_LOGGER_EXEC("stack frame: '%s'", print_stack(stack, registers_set).c_str());

        (*registers_set)[reg_index("RI")] += sizeof(instruction_t);
      }
    }
  }
#endif
}


struct interpreter_t {

  void exec(const std::string code) {
    using namespace aml_n;

    auto lexemes = lexical_analyzer_n::process(code);
    DEBUG_LOGGER_LA("lexemes: \n%s", lexical_analyzer_n::show(lexemes).c_str());

    auto syntax_lisp_tree = syntax_lisp_analyzer_n::process(lexemes);
    DEBUG_LOGGER_SA("syntax_lisp_tree: \n%s", syntax_lisp_analyzer_n::show(syntax_lisp_tree).c_str());

    auto program_stmt = syntax_analyzer_n::process(syntax_lisp_tree);
    // DEBUG_LOGGER_SA("syntax_tree: \n%s", syntax_analyzer_n::show(syntax_tree).c_str());

    // TODO
    // intermediate_code_generator_n::instructions_t instructions;
    // intermediate_code_generator_n::functions_t functions;
    // intermediate_code_generator_n::process(instructions, functions, cmds_str);

    // utils_n::data_t text;
    // code_generator_n::process(text, instructions);

    // executor_n::process(text, functions);
  }
};



int main() {
  std::string code = R"ASM(
    (func sum
      (block
        (set res (add $1 $1 $2))
        (ret res)))

    (func main
      (block
        (set tmp
          (call sum
            (call sum 1 2)
            10))
        (call print tmp)))
  )ASM";

  // std::cout << code << std::endl;

  interpreter_t interpreter;
  interpreter.exec(code);

  return 0;
}

