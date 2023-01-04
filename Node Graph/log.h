#pragma once

#include <date/date.h>
#include <iostream>
#include <string>

enum log_priority
{
    debug,
    info,
    notice,
    warn,
    error,
    critic,
    alert,
    emerg
};

class Logger {
private:
    static log_priority verbosity;
public:
    static void set_verbosity(const log_priority new_priority) 
    {
        verbosity = new_priority;
    }

    static void log(const log_priority priority, const std::string& message) 
    {
        if (priority >= verbosity) {
            std::cout << date::format("%F %H:%M:%OS - [", std::chrono::system_clock::now());
            switch (priority) {
            case emerg: std::cout << "emerg"; break;
            case alert: std::cout << "alert"; break;
            case critic: std::cout << "critic"; break;
            case error: std::cout << "error"; break;
            case warn: std::cout << "warn"; break;
            case notice: std::cout << "notice"; break;
            case info: std::cout << "info"; break;
            case debug: std::cout << "debug"; break;
            }
            std::cout << "] ";
            std::cout << message << "\n";
        }
    }
};