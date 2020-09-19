#include "logger.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace aml::logger_n {

  logger_t::logger_t(const char* file, int line, const char* function)
    : file(file), line(line), function(function) {
      logger->log(spdlog::source_loc{file, line, function}, spdlog::level::trace,
          "{}{}", utils_n::indent(indent), function);

      if (level <= spdlog::level::trace)
        indent++;
    }

  logger_t::~logger_t() {
    if (level <= spdlog::level::trace)
      indent--;

    logger->log(spdlog::source_loc{file, line, function}, spdlog::level::trace,
        "{}{}~", utils_n::indent(indent), function);
  }

  void logger_t::init(const std::string& file_log, const std::string& level_str) {
    spdlog::drop(name);
    level = spdlog::level::from_str(level_str);

    if (file_log == "-") {
      logger = spdlog::stdout_color_mt(name);
    } else if (!file_log.empty()) {
      logger = spdlog::basic_logger_mt(name, file_log, true);
    } else {
      logger = spdlog::stderr_color_mt(name);
      level = spdlog::level::off;
    }

    logger->set_level(level);
    logger->set_pattern(pattern);
  }
}
