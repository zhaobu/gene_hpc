

#include "../work2/logger.h"
#include "../work2/utils.h"
#include <string>
#include <thread>

using namespace std;

int test_format()
{
    string cur_time = get_current_time();
    spdlog::info("cur_time={}, time={}", cur_time, str_format("logs/%s.log", cur_time.c_str()));
    return 0;
}

int test_stack_buffer_overflow()
{
    int arr[8] = {0};
    arr[0] = 10;
    arr[9] = 11;

    return 0;
}

int test_heap_use_after_free()
{
    char *p = NULL;
    p = (char *)malloc(16);
    free(p);
    p[0] = 1;

    return 0;
}

int test_heap_buffer_overflow()
{
    char *p = NULL;

    p = (char *)malloc(16);

    p[17] = 12;

    free(p);

    return 0;
}

int test_stack_use_after_scope()
{
    char *ptr;

    {
        char my_char;
        ptr = &my_char;
    }

    *ptr = 123;

    return 0;
}

int g = 0;
int test_data_race()
{
    auto thread_func = [&]() -> int
    {
        g++;
        return g;
    };
    thread th1(thread_func);
    thread th2(thread_func);
    th1.join();
    th2.join();
    return 0;
}

int test_memory_leak()
{
    char *p = (char *)malloc(16);
    p[0] = 1;

    return 0;
}

int main(int argc, char *argv[])
{
    test_format();
    test_stack_buffer_overflow();
    test_memory_leak();
    test_data_race();
    return 0;
}