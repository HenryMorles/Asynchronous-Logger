#include <iostream>
#include <chrono>
#include "Logger.h"

int main()
{
    Logger& logger = Logger::getInstance();

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < 1000; ++i)
    {
        std::cout << "E";
        logger.log(LogLevel::LOG_INFO, "Logging iteration " + std::to_string(i));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

    logger.log(LogLevel::LOG_ERROR, "Error !");

    std::cout << "\nExecution time: " << elapsed.count() << " microseconds" << std::endl;

    return 0;
}
