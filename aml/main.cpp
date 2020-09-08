#include <iostream>
#include <boost/program_options.hpp>
#include <filesystem>

#include "aml.h"



struct options_t {
  std::string input  = {};
  std::string output = {};
  std::string cmd    = {};
  std::string log    = "-";
  std::string wd     = std::filesystem::current_path();
  // bool        debug  = {};

  std::string show() {
    std::stringstream ss;
    ss << "input:  " << input  << std::endl;
    ss << "output: " << output << std::endl;
    ss << "cmd:    " << cmd    << std::endl;
    ss << "log:    " << log    << std::endl;
    ss << "wd:     " << wd     << std::endl;
    return ss.str();
  }

  bool parse(int argc, char* argv[]) {
    try {
      using namespace boost::program_options;

      options_description desc{"Options"};
      desc.add_options()
        ("help,h",                            "Help screen")
        ("input",  value(&input)->required(), "Read from file")
        ("output", value(&output),            "Write to file")
        ("cmd",    value(&cmd)->required(),   "Availible commands \"compile\" and \"execute\"")
        ("log",    value(&log),               "Write verbose output to file. Stdout is used if file is '-'")
        ("wd",     value(&wd),                "Current working directory")
        ;

      variables_map vm;
      store(parse_command_line(argc, argv, desc), vm);

      if (vm.count("help")) {
        std::cout << desc << std::endl;
        return false;
      }
      notify(vm);

    } catch (const std::exception& ex) {
      std::cerr << ex.what() << std::endl;
      return false;
    }

    return true;
  }
};



int main(int argc, char* argv[]) {

  options_t options = {};
  if (!options.parse(argc, argv)) {
    return 1;
  }

  bool success = true;
  std::string input = aml::utils_n::str_from_file(options.input);
  std::stringstream log;

  log << options.show() << std::endl;

  if (options.cmd == "compile") {
    if (options.output.empty()) {
      std::cerr << "Option output is not set" << std::endl;
      return 1;
    }

    std::string output;
    success = aml::aml_n::compile(input, output, options.wd, log);
    aml::utils_n::str_to_file(output, options.output);

  } else if (options.cmd == "execute") {
    success = aml::aml_n::execute(input, log);

  } else {
    std::cerr << "Unknown cmd. Use --help" << std::endl;
  }

  if (options.log.empty()) {
    ;
  } else if (options.log == "-") {
    std::cout << log.str();
  } else {
    aml::utils_n::str_to_file(log.str(), options.log);
  }

  return !success;
}

