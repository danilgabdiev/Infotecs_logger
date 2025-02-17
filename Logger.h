#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <mutex>
#include <fstream>
#include <chrono>
#include <stdexcept>

namespace LoggerLib {

/// Уровни важности сообщений
enum class LogLevel {
    Low = 0,
    Medium = 1,
    High = 2
};

/// Класс для записи сообщений в лог-файл.
/// При создании открывается указанный файл в режиме добавления, а также задаётся уровень по умолчанию.
class Logger {
public:
    /// Конструктор принимает имя файла и уровень по умолчанию.
    Logger(const std::string &filename, LogLevel defaultLevel);
    
    /// Деструктор закрывает файл.
    ~Logger();
    
    /// Запись сообщения в лог.
    /// Если уровень сообщения ниже текущего уровня по умолчанию, сообщение не записывается.
    void log(const std::string &message, LogLevel level);
    
    /// Изменение уровня важности по умолчанию.
    void setDefaultLevel(LogLevel level);
    
private:
    std::string filename_;
    LogLevel defaultLevel_;
    std::mutex mtx_;
    std::ofstream ofs_;
    
    /// Преобразование времени в строку (формат YYYY-MM-DD HH:MM:SS)
    std::string timeToString(const std::chrono::system_clock::time_point& tp);
    
    /// Преобразование уровня в строку
    std::string logLevelToString(LogLevel level);
};

} // namespace LoggerLib

#endif // LOGGER_H
