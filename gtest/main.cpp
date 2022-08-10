

#include "../work2/logger.h"
#include "../work2/utils.h"
#include <string>

using namespace std;

int test_format()
{
    string cur_time = get_current_time();
    spdlog::info("cur_time={}, time={}", cur_time, str_format("logs/%s.log", cur_time.c_str()));
    return 0;
}

int main(int argc, char *argv[])
{
    test_format();
    return 0;
}