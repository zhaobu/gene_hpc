#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <config.h>
#include <samtools.h>
#include <string>
using namespace std;

bool check_args(int arg_num, const string &text)
{
    return true;
}

void init_log()
{
    try
    {
        auto new_logger = spdlog::basic_logger_mt("new_default_logger", "test.log", true);
        spdlog::set_default_logger(new_logger);
        spdlog::info("new logger log start");
    }
    catch (const spdlog::spdlog_ex &ex)
    {
        std::cout << "Log init failed: " << ex.what() << std::endl;
    }
}

int main(int argc, char *argv[])
{

    // init_log();
    if (argc != 2)
    {
        cout << "请输入配置文件的路径" << endl;
        return 0;
    }

    string conf_file = argv[1];

    spdlog::info("配置文件的路径{}", conf_file);

    Configer conf(conf_file);
    Samtools samtools(conf);
    samtools.start_work();
    return 0;
}