#pragma once

#include <string>

#include "logger_stream_buffer.hpp"

namespace tangue
{

    class Logger
    {
    public:
        static Logger &getInstance(const std::string &ident = "Tangue", int facility = LOG_USER);

        std::ostream &log(int priority);

    private:
        Logger(const std::string &ident, int facility);

        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        ~Logger(void);

        LoggerStreamBuffer emergency_buffer_;
        LoggerStreamBuffer alert_buffer_;
        LoggerStreamBuffer critical_buffer_;
        LoggerStreamBuffer error_buffer_;
        LoggerStreamBuffer warning_buffer_;
        LoggerStreamBuffer notice_buffer_;
        LoggerStreamBuffer info_buffer_;
        LoggerStreamBuffer debug_buffer_;

        std::ostream emergency_stream_;
        std::ostream alert_stream_;
        std::ostream critical_stream_;
        std::ostream error_stream_;
        std::ostream warning_stream_;
        std::ostream notice_stream_;
        std::ostream info_stream_;
        std::ostream debug_stream_;
    };

} // namespace tangue