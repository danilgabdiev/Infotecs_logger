#include "Logger.h"
#include <fstream>
#include <cassert>
#include <cstdio>
#include <iostream>

using namespace LoggerLib;

/// Утилита для удаления файла (если существует)
void removeFile(const std::string &filename) {
    std::remove(filename.c_str());
}

/// Тест: Проверка, что сообщение записывается в лог-файл
void test_log_output() {
    const std::string testFile = "test_log.txt";
    removeFile(testFile);
    
    // Инициализируем логгер с уровнем Low, чтобы записать любое сообщение
    Logger logger(testFile, LogLevel::Low);
    logger.log("Test message", LogLevel::High);
    
    std::ifstream ifs(testFile);
    assert(ifs.is_open() && "Лог-файл должен быть создан.");
    
    std::string content((std::istreambuf_iterator<char>(ifs)),
                         std::istreambuf_iterator<char>());
    // Проверяем, что в файле содержится наше сообщение
    assert(content.find("Test message") != std::string::npos &&
           "Лог-файл должен содержать записанное сообщение.");
    
    ifs.close();
    removeFile(testFile);
}

/// Тест: Проверка фильтрации сообщений по уровню важности
void test_log_filter() {
    const std::string testFile = "test_log_filter.txt";
    removeFile(testFile);
    
    // Инициализируем логгер с уровнем High, поэтому сообщения с уровнем ниже не должны записываться
    Logger logger(testFile, LogLevel::High);
    logger.log("This message should not appear", LogLevel::Low);
    
    std::ifstream ifs(testFile);
    // Если файл создан, проверяем, что он пустой
    if (ifs.is_open()) {
        std::string content((std::istreambuf_iterator<char>(ifs)),
                             std::istreambuf_iterator<char>());
        assert(content.empty() && "Лог-файл должен быть пустым, так как сообщение не удовлетворяет порогу важности.");
        ifs.close();
    }
    
    removeFile(testFile);
}

int main() {
    test_log_output();
    test_log_filter();
    
    std::cout << "All tests passed." << std::endl;
    return 0;
}
