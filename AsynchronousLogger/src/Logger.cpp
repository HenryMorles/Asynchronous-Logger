#include "Logger.h"

Logger& Logger::getInstance()
{
    static Logger instance;
    return instance;
}

Logger::Logger() : _running(true)
{
    loadEnv();
    createLogDirectory();
    _currentDate = getCurrentDate();
    openLogFile();
    _logThread = std::thread(&Logger::processLogs, this);
}

Logger::~Logger()
{
    stop();
}

void Logger::stop()
{
    _running = false;
    _logCondition.notify_all();
    if (_logThread.joinable())
    {
        _logThread.join();
    }
}

std::string Logger::getCurrentDate()
{
    std::time_t now = std::time(nullptr);
    std::tm timeInfo;

#ifdef _WIN32
    localtime_s(&timeInfo, &now);
#else
    localtime_r(&now, &timeInfo);
#endif

    char buffer[11];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", &timeInfo);
    return std::string(buffer);
}

void Logger::openLogFile()
{
    std::string fileName = _LOG_DIRECTORY + "/" + _currentDate + ".log";
    _logFile.open(fileName, std::ios::app);
    if (!_logFile)
    {
        std::cerr << "Failed to open log file: " << fileName << std::endl;
    }
}

void Logger::sendToTelegram(const std::string& message)
{
    if (_TELEGRAM_BOT_TOKEN.empty() || _TELEGRAM_CHAT_ID.empty())
    {
        std::cerr << "⚠️ Warning: Telegram credentials are missing! Skipping notification." << std::endl;
        return;
    }

    std::string url = "https://api.telegram.org/bot" + _TELEGRAM_BOT_TOKEN +
        "/sendMessage?chat_id=" + _TELEGRAM_CHAT_ID +
        "&text=" + curl_easy_escape(nullptr, message.c_str(), 0);

    CURL* curl = curl_easy_init();
    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK)
        {
            std::cerr << "Failed to send message to Telegram: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
}

void Logger::log(LogLevel level, const std::string& message)
{
    std::string newDate = getCurrentDate();
    if (newDate != _currentDate)
    {
        _currentDate = newDate;
        _logFile.close();
        openLogFile();
    }

    std::string levelStr;
    switch (level)
    {
    case LogLevel::LOG_INFO: levelStr = "INFO"; break;
    case LogLevel::LOG_WARNING: levelStr = "WARNING"; break;
    case LogLevel::LOG_ERROR: levelStr = "ERROR"; break;
    }

    std::ostringstream logStream;
    logStream << "[" << newDate << "] [" << levelStr << "] " << message << std::endl;

    {
        std::lock_guard<std::mutex> lock(_logMutex);
        _logQueue.push(logStream.str());
    }

    _logCondition.notify_one();

    if (level == LogLevel::LOG_ERROR)
    {
        sendToTelegram(message);
    }
}

void Logger::processLogs()
{
    while (_running || !_logQueue.empty())
    {
        std::unique_lock<std::mutex> lock(_logMutex);
        _logCondition.wait(lock, [this] { return !_logQueue.empty() || !_running; });

        while (!_logQueue.empty())
        {
            std::string logMessage = _logQueue.front();
            _logQueue.pop();
            lock.unlock();

            std::cout << logMessage;
            if (_logFile.is_open())
            {
                _logFile << logMessage;
                _logFile.flush();
            }

            lock.lock();
        }
    }
}

void Logger::loadEnv()
{
    std::ifstream envFile(".env");
    if (!envFile.is_open())
    {
        std::cerr << "⚠️ Warning: .env file not found!" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(envFile, line))
    {
        size_t pos = line.find('=');
        if (pos != std::string::npos)
        {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (key == "TELEGRAM_BOT_TOKEN")
            {
                _TELEGRAM_BOT_TOKEN = value;
            }
            else if (key == "TELEGRAM_CHAT_ID")
            {
                _TELEGRAM_CHAT_ID = value;
            }
        }
    }

    envFile.close();
}

void Logger::createLogDirectory()
{
#ifdef _WIN32
    _mkdir(_LOG_DIRECTORY.c_str()); // Windows
#else
    mkdir(LOG_DIRECTORY.c_str(), 0777); // Linux/macOS
#endif
}