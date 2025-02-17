#include "Logger.h"
#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <sstream>
#include <atomic>
#include <algorithm>

// Структура для хранения данных задачи логирования
struct LogTask {
    std::string message;
    LoggerLib::LogLevel level;
};

// Потокобезопасная очередь задач
class LogTaskQueue {
public:
    void push(const LogTask &task) {
        std::lock_guard<std::mutex> lock(mtx_);
        queue_.push(task);
        cv_.notify_one();
    }
    
    // Извлечение задачи; функция блокируется, если очередь пуста
    bool pop(LogTask &task) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [&]{ return !queue_.empty() || done_; });
        if (queue_.empty())
            return false;
        task = queue_.front();
        queue_.pop();
        return true;
    }
    
    // Уведомление о завершении
    void setDone() {
        std::lock_guard<std::mutex> lock(mtx_);
        done_ = true;
        cv_.notify_all();
    }
    
private:
    std::queue<LogTask> queue_;
    std::mutex mtx_;
    std::condition_variable cv_;
    bool done_ = false;
};

// Функция-потребитель, которая извлекает задачи и вызывает логирование
void worker(LogTaskQueue &taskQueue, LoggerLib::Logger &logger) {
    LogTask task;
    while (taskQueue.pop(task)) {
        try {
            logger.log(task.message, task.level);
        } catch (const std::exception &ex) {
            std::cerr << "Logging error: " << ex.what() << std::endl;
        }
    }
}

/// Функция для преобразования строки в LogLevel.
/// При некорректном значении возвращается defaultLevel.
LoggerLib::LogLevel parseLogLevel(const std::string &levelStr, LoggerLib::LogLevel defaultLevel) {
    std::string ls = levelStr;
    // Приводим строку к нижнему регистру
    std::transform(ls.begin(), ls.end(), ls.begin(), ::tolower);
    if (ls == "low") return LoggerLib::LogLevel::Low;
    if (ls == "medium") return LoggerLib::LogLevel::Medium;
    if (ls == "high") return LoggerLib::LogLevel::High;
    return defaultLevel;
}

/// Помощь по использованию приложения
void printUsage(const char* progName) {
    std::cout << "Usage: " << progName << " <log_file> <default_level>\n";
    std::cout << "    default_level: low | medium | high\n";
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printUsage(argv[0]);
        return 1;
    }
    
    std::string logFile = argv[1];
    std::string defLevelStr = argv[2];
    LoggerLib::LogLevel defaultLevel = parseLogLevel(defLevelStr, LoggerLib::LogLevel::Medium);
    
    // Инициализируем логгер
    LoggerLib::Logger logger(logFile, defaultLevel);
    
    LogTaskQueue taskQueue;
    std::thread workerThread(worker, std::ref(taskQueue), std::ref(logger));
    
    std::cout << "Введите сообщения для логирования.\n";
    std::cout << "Формат ввода: <message> [<level>]. Для выхода введите 'exit'\n";
    
    std::string inputLine;
    while (std::getline(std::cin, inputLine)) {
        if (inputLine == "exit")
            break;
        
        // Разбираем ввод: сообщение и, возможно, уровень (разделитель – пробел)
        std::istringstream iss(inputLine);
        std::string message;
        std::getline(iss, message, '|'); // Можно использовать символ разделителя (например, '|')
        std::string levelStr;
        std::getline(iss, levelStr); // Если уровень не указан, строка будет пустой
        
        // Если уровень не указан, используем defaultLevel
        LoggerLib::LogLevel msgLevel = defaultLevel;
        if (!levelStr.empty()) {
            // Удаляем пробелы по краям
            levelStr.erase(0, levelStr.find_first_not_of(" \t"));
            levelStr.erase(levelStr.find_last_not_of(" \t")+1);
            msgLevel = parseLogLevel(levelStr, defaultLevel);
        }
        
        // Создаем задачу и помещаем её в очередь
        LogTask task{message, msgLevel};
        taskQueue.push(task);
    }
    
    // Завершаем работу потока-потребителя
    taskQueue.setDone();
    if (workerThread.joinable())
        workerThread.join();
    
    return 0;
}
