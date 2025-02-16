#pragma once

#include <iostream>
#include <fstream>
#include <mutex>
#include <string>
#include <queue>
#include <thread>
#include <condition_variable>
#include <atomic>
#include <sstream>
#include <curl/curl.h>
#include <direct.h>

enum class LogLevel
{
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR
};

class Logger
{
private:

    std::ofstream _logFile;
    std::mutex _logMutex;
    std::queue<std::string> _logQueue;
    std::thread _logThread;
    std::condition_variable _logCondition;
    std::atomic<bool> _running;
    std::string _currentDate;

    const std::string _LOG_DIRECTORY = "logs";
    std::string _TELEGRAM_BOT_TOKEN;
    std::string _TELEGRAM_CHAT_ID;

    Logger();
    ~Logger();
    void processLogs();
    std::string getCurrentDate();
    void openLogFile();
    void sendToTelegram(const std::string& message);
    void loadEnv();
    void createLogDirectory();

public:

    static Logger& getInstance();
    void log(LogLevel level, const std::string& message);
    void stop();
};