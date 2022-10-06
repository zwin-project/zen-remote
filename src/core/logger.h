#pragma once

#include "core/common.h"
#include "zen-remote/logger.h"

namespace zen::remote {

struct Logger {
  // logger singleton set by InitializeLogger;
  static std::unique_ptr<Logger> instance;

  DISABLE_MOVE_AND_COPY(Logger);
  Logger() = default;
  virtual ~Logger() = default;

  void Print(Severity severity, const char* pretty_function, const char* file,
      int line, const char* format, ...)
      __attribute__((__format__(printf, 6, 7)));

 private:
  friend void InitializeLogger(std::unique_ptr<ILogSink> sink);
  std::unique_ptr<ILogSink> sink_;
};

#define LOG_DEBUG(format, ...)                                       \
  zen::remote::Logger::instance->Print(zen::remote::Severity::DEBUG, \
      __PRETTY_FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_INFO(format, ...)                                       \
  zen::remote::Logger::instance->Print(zen::remote::Severity::INFO, \
      __PRETTY_FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_WARN(format, ...)                                       \
  zen::remote::Logger::instance->Print(zen::remote::Severity::WARN, \
      __PRETTY_FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...)                                       \
  zen::remote::Logger::instance->Print(zen::remote::Severity::ERROR, \
      __PRETTY_FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)

#define LOG_FATAL(format, ...)                                       \
  zen::remote::Logger::instance->Print(zen::remote::Severity::FATAL, \
      __PRETTY_FUNCTION__, __FILE__, __LINE__, format, ##__VA_ARGS__)

}  // namespace zen::remote
