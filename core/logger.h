#pragma once

#include "common.h"
#include "zen/display-system/remote/core/logger.h"

namespace zen::display_system::remote::log {

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

#define LOG_DEBUG(format, ...)                                                \
  zen::display_system::remote::log::Logger::instance->Print(                  \
      zen::display_system::remote::log::DEBUG, __PRETTY_FUNCTION__, __FILE__, \
      __LINE__, format, ##__VA_ARGS__)

#define LOG_INFO(format, ...)                                                \
  zen::display_system::remote::log::Logger::instance->Print(                 \
      zen::display_system::remote::log::INFO, __PRETTY_FUNCTION__, __FILE__, \
      __LINE__, format, ##__VA_ARGS__)

#define LOG_WARN(format, ...)                                                \
  zen::display_system::remote::log::Logger::instance->Print(                 \
      zen::display_system::remote::log::WARN, __PRETTY_FUNCTION__, __FILE__, \
      __LINE__, format, ##__VA_ARGS__)

#define LOG_ERROR(format, ...)                                                \
  zen::display_system::remote::log::Logger::instance->Print(                  \
      zen::display_system::remote::log::ERROR, __PRETTY_FUNCTION__, __FILE__, \
      __LINE__, format, ##__VA_ARGS__)

#define LOG_FATAL(format, ...)                                                \
  zen::display_system::remote::log::Logger::instance->Print(                  \
      zen::display_system::remote::log::FATAL, __PRETTY_FUNCTION__, __FILE__, \
      __LINE__, format, ##__VA_ARGS__)

}  // namespace zen::display_system::remote::log
