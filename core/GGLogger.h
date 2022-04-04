#ifndef _GG_LOGGER_H_
#define _GG_LOGGER_H_

#include "GGSingleton.h"
#include <algorithm>
#include <assert.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <vulkan/vulkan.h>

namespace Gange {
class DefaultSink : public spdlog::sinks::sink {
public:
    DefaultSink();
    void log(const spdlog::details::log_msg &msg) override;
    void flush() override;
    void set_pattern(const std::string &pattern) override;
    void set_formatter(std::unique_ptr<spdlog::formatter> sink_formatter) override;

private:
    std::shared_ptr<spdlog::logger> default_logger_;
};

class Logger {
public:
    Logger() {
        auto default_sink = std::make_shared<DefaultSink>();
        logger_ = std::make_shared<spdlog::logger>("Gange", default_sink);
    }

    template <typename T, typename... Args> std::shared_ptr<T> AddSink(const Args &...args) {
        auto name = logger_->name();
        auto sinks = logger_->sinks();
        auto new_sink = std::make_shared<T>(args...);
        sinks.push_back(new_sink);
        logger_ = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
        return new_sink;
    }

    void RemoveSink(spdlog::sinks::sink *removeSink) {
        auto name = logger_->name();
        auto sinks = logger_->sinks();
        auto removeIt = std::remove_if(sinks.begin(), sinks.end(),
                                       [removeSink](const spdlog::sink_ptr &ptr) { return ptr.get() == removeSink; });
        sinks.erase(removeIt, sinks.end());
        logger_ = std::make_shared<spdlog::logger>(name, sinks.begin(), sinks.end());
    }

    std::shared_ptr<spdlog::logger> GetLogger() {
        return logger_;
    }

    template <typename... Args> void Info(const char *message, const Args &...args) {
        logger_->info(message, args...);
    }

    template <typename... Args> void Warn(const char *message, const Args &...args) {
        logger_->warn(message, args...);
    }

    template <typename... Args> void Error(const char *message, const Args &...args) {
        logger_->error(message, args...);
    }

    template <typename... Args> void Trace(const char *message, const Args &...args) {
        logger_->trace(message, args...);
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
};

template <typename... Args> void Info(const char *message, const Args &...args) {
    Singleton<Logger>::Get()->Info(message, args...);
}

template <typename... Args> void Trace(const char *message, const Args &...args) {
    Singleton<Logger>::Get()->Trace(message, args...);
}

template <typename... Args> void Warn(const char *message, const Args &...args) {
    Singleton<Logger>::Get()->Warn(message, args...);
}

template <typename... Args> void Error(const char *message, const Args &...args) {
    Singleton<Logger>::Get()->Error(message, args...);
}
}  // namespace Gange

#define GG_INFO(message, ...) Gange::Info(message, ##__VA_ARGS__)

#define GG_WARN(message, ...) Gange::Warn(message, ##__VA_ARGS__)

#define GG_ERROR(message, ...) Gange::Error(message, ##__VA_ARGS__)

#define GG_TRACE(message, ...) Gange::Trace(message, ##__VA_ARGS__)

#define GG_ASSERT(expr) assert(expr)

#define VK_CHECK_RESULT(f)                                                                  \
    {                                                                                                                 \
        VkResult res = (f);                                                                               \
        if (res != VK_SUCCESS) {                                                                     \
            GG_ERROR("ERROR!", res);                                                            \
            GG_ASSERT(res == VK_SUCCESS);                                                \
        }                                                                                                              \
    }

#endif  // GG_CORE_LOG_H_
