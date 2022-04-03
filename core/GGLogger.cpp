#include "GGLogger.h"
#include "GGUnicodeUtils.h"
#include <iostream>
#include <spdlog/sinks/stdout_color_sinks.h>

#if _WIN32
#include <Windows.h>
#endif  // _WIN32

namespace Gange {
DefaultSink::DefaultSink() {
    default_logger_ = spdlog::stdout_color_mt("Gange");
#if _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif  // _WIN32
}

void DefaultSink::log(const spdlog::details::log_msg &msg) {
    default_logger_->log(msg.time, msg.source, msg.level, msg.payload);
}

void DefaultSink::flush() {
    default_logger_->flush();
}

void DefaultSink::set_pattern(const std::string &pattern) {}

void DefaultSink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) {}
}  // namespace Gange
