#pragma once

#define SPDLOG_FMT_EXTERNAL
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#define SPDLOG_DEBUG_ON
#define SPDLOG_TRACE_ON

#include <spdlog/sinks/null_sink.h>
#include <spdlog/spdlog.h>

#include "utils.h"

namespace aml::logger_n {

  struct logger_t {
    const char* file;
    int line;
    const char* function;
    static inline size_t indent = 0;
    static inline spdlog::level::level_enum level = spdlog::level::off;

    static inline const std::string name = "core";
    static inline const std::string pattern = "[%L] #%-5# %v"; // [%Y-%m-%d %H:%M:%S:%e]
    static inline std::shared_ptr<spdlog::logger> logger = spdlog::null_logger_mt(name);

    logger_t(const char* file, int line, const char* function);
    ~logger_t();

    static void init(const std::string& file_log, const std::string& level_str);

#if 0
    static void log(spdlog::level level, const char* file, int line, const char* function, const char* /*format*/, ...) {
      logger->log(spdlog::source_loc{file, line, function}, level,
          "{}{}", utils_n::indent(indent), "log");
    }
#endif
  };



#define AML_TRACER aml::logger_n::logger_t logger{__FILE__, __LINE__, __PRETTY_FUNCTION__}

#define AML_LOGGER(lvl, format, ...) \
      aml::logger_n::logger_t::logger->log(spdlog::source_loc{__FILE__, __LINE__, __FUNCTION__}, spdlog::level::lvl, \
          "{}{}: " format, aml::utils_n::indent(aml::logger_n::logger_t::indent), __FUNCTION__, __VA_ARGS__);
}
