#include "pch.h"

#include "core/logger.h"
#include "zen/display-system/remote/core/logger.h"

namespace zen::display_system::remote::log {

void
Logger::Print(Severity severity, const char* pretty_function, const char* file,
    int line, const char* format, ...)
{
  va_list args;
  va_start(args, format);
  if (sink_) sink_->Sink(severity, pretty_function, file, line, format, args);
  va_end(args);
}

std::unique_ptr<Logger> Logger::instance;

void
InitializeLogger(std::unique_ptr<ILogSink> sink)
{
  Logger::instance = std::make_unique<Logger>();
  Logger::instance->sink_ = std::move(sink);
}

}  // namespace zen::display_system::remote::log
