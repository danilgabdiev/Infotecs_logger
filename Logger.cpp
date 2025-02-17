#include "Logger.h"
#include <ctime>
#include <iomanip>
#include <sstream>

namespace LoggerLib {

Logger::Logger(const std::string &filename, LogLevel defaultLevel)
    : filename_(filename), defaultLevel_(defaultLevel)
{
    ofs_.open(filename_, std::ios::app);
    if (!ofs_.is_open()) {
        throw std::runtime_error("Unable to open log file: " + filename_);
    }
}

Logger::~Logger() {
    if (ofs_.is_open()) {
        ofs_.close();
    }
}

void Logger::setDefaultLevel(LogLevel level) {
    std::lock_guard<std::mutex> lock(mtx_);
    defaultLevel_ = level;
}

std::string Logger::timeToString(const std::chrono::system_clock::time_point& tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm tm;
#if defined(_WIN32) || defined(_WIN64)
    localtime_s(&tm, &tt);
#else
    localtime_r(&tt, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string Logger::logLevelToString(LogLevel level) {
    switch(level) {
        case LogLevel::Low: return "Low";
        case LogLevel::Medium: return "Medium";
        case LogLevel::High: return "High";
        default: return "Unknown";
    }
}

void Logger::log(const std::string &message, LogLevel level) {
    // Если уровень сообщения ниже уровня по умолчанию – не записывать
    if (static_cast<int>(level) < static_cast<int>(defaultLevel_)) {
        return;
    }
    
    std::lock_guard<std::mutex> lock(mtx_);
    auto now = std::chrono::system_clock::now();
    
    ofs_ << "[" << timeToString(now) << "] "
         << "[" << logLevelToString(level) << "] "
         << message << std::endl;
    
    if (!ofs_) {
        throw std::runtime_error("Error writing to log file: " + filename_);
    }
}

} // namespace LoggerLib
