#pragma once

#include <iostream>
#include <mutex>
#include <chrono>
#include <format>
#include <iomanip>
#include <sstream>

class Logger {
public:
    enum class Level {
        DEBUG,
        INFO,
        WARNING,
        ERROR,
        FATAL
    };

    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
    Logger& operator=(const Logger&) = delete;
    Logger& operator=(Logger&&) = delete;

    void setLogLevel(const Level level) {
        currentLevel = level;
    }

    template<typename... Args>
    void debug(std::format_string<Args...> fmt, Args&&... args) {
        log(Level::DEBUG, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void info(std::format_string<Args...> fmt, Args&&... args) {
        log(Level::INFO, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void warning(std::format_string<Args...> fmt, Args&&... args) {
        log(Level::WARNING, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void error(std::format_string<Args...> fmt, Args&&... args) {
        log(Level::ERROR, fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    void fatal(std::format_string<Args...> fmt, Args&&... args) {
        log(Level::FATAL, fmt, std::forward<Args>(args)...);
    }

private:
    Logger() : currentLevel(Level::INFO) {}

    template<typename... Args>
    void log(const Level level, std::format_string<Args...> fmt, Args&&... args) {
        if (level >= currentLevel) {
            const auto now = std::chrono::system_clock::now();
            const auto time_t = std::chrono::system_clock::to_time_t(now);
            
            std::ostringstream timestamp;
            timestamp << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            
            std::string message = std::format(fmt, std::forward<Args>(args)...);
            
            std::lock_guard lock(mutex);
            std::cout << std::format("{} [{}] {}\n",
                                   timestamp.str(),
                                   getLevelString(level),
                                   message);
        }
    }

    static constexpr const char* getLevelString(const Level level) {
        switch (level) {
            case Level::DEBUG:   return "DEBUG";
            case Level::INFO:    return "INFO";
            case Level::WARNING: return "WARNING";
            case Level::ERROR:   return "ERROR";
            case Level::FATAL:   return "FATAL";
            default:             return "UNKNOWN";
        }
    }

    Level currentLevel;
    std::mutex mutex;
};

#define LOG_DEBUG(...) Logger::getInstance().debug(__VA_ARGS__)
#define LOG_INFO(...) Logger::getInstance().info(__VA_ARGS__)
#define LOG_WARNING(...) Logger::getInstance().warning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::getInstance().error(__VA_ARGS__)
#define LOG_FATAL(...) Logger::getInstance().fatal(__VA_ARGS__)
