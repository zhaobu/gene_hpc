#ifndef LOGGER
#define LOGGER
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE //必须定义这个宏,才能输出文件名和行号

#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include "utils.h"
#include <iostream>
#include <memory>

using namespace std;

/* 创建复合logger */
void init_log()
{
    try
    {
        /* 通过multi-sink的方式创建复合logger，实现方式为：先分别创建文件sink和控制台sink，并将两者放入sink 向量中，组成一个复合logger */
        /* file sink */
        string log_file = str_format("logs/%s.log", get_current_time("%Y-%m-%d").c_str());
        string log_format = "[%Y-%m-%d %H:%M:%S %z][%l][%g:%#] [thread %t] %v";
        auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(log_file, true);
        file_sink->set_level(spdlog::level::trace);
        file_sink->set_pattern(log_format);
        std::cout << "MultiLogger: create file sink OK." << std::endl;

        /* 控制台sink */
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(spdlog::level::trace);
        console_sink->set_pattern(log_format);
        std::cout << "MultiLogger: create console sink OK." << std::endl;

        /* Sink组合 */

        auto new_logger = std::make_shared<spdlog::logger>(std::move(spdlog::logger("multi_sink", {console_sink, file_sink})));
        new_logger->set_level(spdlog::level::debug);

        // auto new_logger = spdlog::basic_logger_mt("new_default_logger", "test.log", true);
        spdlog::set_default_logger(new_logger);

        spdlog::info("MultiLogger: create multi sink OK.");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Create multi-logger failed: " << ex.what() << std::endl;
        exit(EXIT_FAILURE);
    }
}

#endif
