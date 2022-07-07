#include "GGLogger.h"
#include "GGUnicodeUtils.h"
#include <iostream>

#if defined(VK_USE_PLATFORM_WIN32_KHR)
#include <spdlog/sinks/stdout_color_sinks.h>
#include <Windows.h>
#elif defined(__ANDROID__)
#include <spdlog/sinks/android_sink.h>
#endif

namespace Gange {
DefaultSink::DefaultSink() {
#if defined(__ANDROID__)
    std::string tag = "renderCore";
	mDefaultSink = spdlog::android_logger_mt("android", tag);
#elif _WIN32
    mDefaultSink= spdlog::stdout_color_mt("Gange");
    SetConsoleOutputCP(CP_UTF8);
#endif  // _WIN32
}

void DefaultSink::log(const spdlog::details::log_msg &msg) {
    mDefaultSink->log(msg.time, msg.source, msg.level, msg.payload);
}

void DefaultSink::flush() {
    mDefaultSink->flush();
}

void DefaultSink::set_pattern(const std::string &pattern) {}

void DefaultSink::set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) {}
}  // namespace Gange
