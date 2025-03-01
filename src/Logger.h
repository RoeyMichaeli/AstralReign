#pragma once
#pragma once
#include <iostream>
#include <fstream>
#include <string>
#include <mutex>

#ifdef _DEBUG
#define LOG(msg) Logger::getInstance().log(msg)
#else
#define LOG(msg) do {} while (0)
#endif


class Logger {
public:
    // Get the singleton instance
    static Logger& getInstance() {
        static Logger instance;
        return instance;
    }

    // Log a message (thread-safe)
    void log(const std::string& message) {
        std::lock_guard<std::mutex> lock(m_mutex);
        // Write to console
        std::cout << message << std::endl;
        // Also write to a file (optional)
        if (m_logFile.is_open())
            m_logFile << message << std::endl;
    }

    // Delete copy constructor and assignment operator
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

private:
    Logger() {
        // Open log file for appending
        m_logFile.open("game.log", std::ios::out | std::ios::app);
        if (!m_logFile.is_open())
            std::cerr << "Failed to open log file." << std::endl;
    }
    ~Logger() {
        if (m_logFile.is_open())
            m_logFile.close();
    }

    std::ofstream m_logFile;
    std::mutex m_mutex;
};
