
#include "logger.h"
#include <config.h>
#include <samtools.h>
#include <string>
using namespace std;

bool check_args(int arg_num, const string &text)
{
    return true;
}

int main(int argc, char *argv[])
{

    init_log();
    if (argc != 2)
    {
        cout << "请输入配置文件的路径" << endl;
        return 0;
    }

    string conf_file = argv[1];

    spdlog::info("读取配置文件的路径为{}", conf_file);

    Configer conf(conf_file);
    Samtools samtools(conf);
    samtools.start_work();
    return 0;
}