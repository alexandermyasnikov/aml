#include <iostream>
#include <boost/program_options.hpp>
#include "aml.h"

bool parse(aml::aml_n::options_t& options, int argc, char* argv[]) {
  try {
    using namespace boost::program_options;

    options_description desc{"Options"};
    desc.add_options()
      ("help,h",                                   "Help screen")
      ("file_input",  value(&options.file_input),  "Read from file")
      ("file_output", value(&options.file_output), "Write to file. Stdout is used if file is -")
      ("input",       value(&options.input),       "code")
      ("cmd",         value(&options.cmd),         "Availible commands \"compile\" and \"execute\"")
      ("log",         value(&options.file_log),    "Write verbose output to file. Stdout is used if file is -")
      ("level",       value(&options.level),       "Log levels: \"trace\", \"debug\", \"info\", \"warning\", \"error\", \"critical\" \"off\"")
      ("wd",          value(&options.wd),          "Current working directory")
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



int main(int argc, char* argv[]) {
  aml::aml_n::options_t options = {};

  if (!parse(options, argc, argv)) {
    return 1;
  }

  return !aml::aml_n::run(options);
}

