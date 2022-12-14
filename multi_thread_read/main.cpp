#include "spdlog/sinks/basic_file_sink.h"
#include "spdlog/spdlog.h"
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
using namespace std;

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

void thread_read_file(int tid, const string &file_path, std::streampos start_pos, int each_size)
{
    ifstream file(file_path.c_str(), ios::in);
    if (!file.good())
    {
        file.close();
        spdlog::info("线程{} 打开文件{}失败", tid, file_path);
        return;
    }

    file.seekg(start_pos, ios::beg);
    //
    string text;
    if (start_pos != 0)
    {
        char cur_ch = 0;
        // spdlog::info("读取前{}", file.tellg());
        file.read(&cur_ch, 1); //会让指针向后移动一个字节
        // spdlog::info("读取后{}", file.tellg());
        if (cur_ch != '\n')
        {
            getline(file, text);
            // spdlog::info("线程{},跳过{}", tid, text);
        }
        // spdlog::info("线程{} cur_ch={}", tid, cur_ch);
    }

    std::streampos cur_pos = file.tellg();
    int read_count = cur_pos - start_pos;
    while (read_count <= each_size && !file.eof())
    {
        getline(file, text);
        int cur_line_len = file.tellg() - cur_pos;
        read_count += cur_line_len;
        spdlog::info("线程{} start_pos={},each_size={},本行开始pos={},本行结束pos={},本行读长={},text={}",
                     tid, start_pos, each_size, cur_pos, file.tellg(), cur_line_len, text);
        cur_pos = file.tellg();
    }
    // spdlog::info("线程{} start_pos={},next_pos={},each_size={},结束时cur_pos={},总共区间长度为{}\n", tid, start_pos, next_pos, each_size, cur_pos, cur_pos - start_pos);
    file.close();
    return;
}

void test_detach(const string &file_path)
{
    // for (int i = 0; i < 10; ++i)
    // {
    //     std::thread th(thread_read_file, i, file_path);
    //     th.detach();
    // }
}

void test_join(const string &file_path)
{
    //确定文件长度
    ifstream file(file_path.c_str(), ios::in);

    //把指针指到文件末尾求出文件大小
    int file_size = file.seekg(0, ios::end).tellg();
    file.close();

    int thread_nums = 40;                    //线程个数
    int each_size = file_size / thread_nums; //平均每个线程读取的字节数
    assert(each_size > 0);
    std::streampos start_pos = 0;    //每个线程读取的起始位置
    vector<std::thread> vec_threads; //线程列表
    spdlog::info("thread_nums={},each_size={},file_size={}", thread_nums, each_size, file_size);
    int t_id = 0; //线程id
    for (; t_id < thread_nums; ++t_id)
    {
        std::thread th(thread_read_file, t_id, file_path, start_pos, each_size);
        vec_threads.emplace_back(std::move(th)); // push_back() is also OK
        start_pos += each_size;
    }

    if (file_size % thread_nums != 0)
    {
        int left_size = file_size - start_pos;
        spdlog::info("剩下的部分{}全部由主线程来读取", left_size);
        thread_read_file(t_id, file_path, start_pos, left_size);
    }

    for (auto &it : vec_threads)
    {
        it.join();
    }
}
int main()
{
    // init_log();
    string file_path = "/data/lush-dev/liwei/homework1/2.sam";
    file_path = "1.txt";
    // file_path = "work2.sam";
    // test_detach(file_path);
    // std::this_thread::sleep_for(std::chrono::seconds(1)); // wait for detached threads done
    test_join(file_path);
    return 0;
}
