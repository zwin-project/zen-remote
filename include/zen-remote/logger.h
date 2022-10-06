#pragma once

#include <stdarg.h>

#include <memory>

namespace zen::remote {

typedef enum class Severity {
  DEBUG = 0,  // logs for debugging during development.
  INFO,       // logs that may be useful to some users.
  WARN,       // logs for recoverable failures.
  ERROR,      // logs for unrecoverable failures.
  FATAL,      // logs when aborting.
  SILENT,     // for internal use only.
} Severity;

struct ILogSink {
  virtual ~ILogSink() = default;

  virtual void Sink(Severity severity, const char* pretty_function,
      const char* file, int line, const char* format, va_list vp) = 0;
};

void InitializeLogger(std::unique_ptr<ILogSink> sink);

}  // namespace zen::remote
