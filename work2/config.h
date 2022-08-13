#pragma once
// #include "yaml-cpp/yaml.h"
#include "spdlog/spdlog.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

using json = nlohmann::json;
using namespace std;

class Configer
{
private:
    // int m_thread = 40;
    string m_sam_file = "data/work2.sam";
    // string m_result_file = "data/result.log";

    int m_thread = 210;
    // string m_sam_file = "/data/lush-dev/liwei/homework1/2.sam";
    string m_result_file = "data/result.log";
    string m_target_rname = "chr22"; //要统计的染色体

public:
    Configer(/* args */);
    Configer(const string &conf_file);
    ~Configer();

    const string &get_target_rname()
    {
        return m_target_rname;
    };

    const string &get_sam_file()
    {
        return m_sam_file;
    };
    int get_thread()
    {
        return m_thread;
    };
    const string &get_result_file()
    {
        return m_result_file;
    };
};

Configer::Configer()
{
}
Configer::~Configer()
{
}

Configer::Configer(const string &conf_file)
{
    try
    {
        std::ifstream file(conf_file);
        json conf_data = json::parse(file);
        spdlog::info("读取到的配置文件为 config:{}", conf_data.dump(2));

        if (conf_data["sam_file"].empty())
        {
            spdlog::error("sam_file未配置");
            return;
        }

        if (conf_data["thread"].empty())
        {
            spdlog::error("thread 未配置");
            return;
        }

        if (conf_data["result_file"].empty())
        {
            spdlog::error("result_file 未配置");
            return;
        }
        if (conf_data["target_rname"].empty())
        {
            spdlog::error("target_rname 未配置");
            return;
        }
        m_sam_file = conf_data["sam_file"].get<string>();
        m_thread = conf_data["thread"].get<int>();
        m_result_file = conf_data["result_file"].get<string>();
        m_target_rname = conf_data["target_rname"].get<string>();

        // std::ofstream fout("config.yaml");
        // fout << config;
    }
    catch (const std::exception &e)
    {
        spdlog::error("读取配置文件{}错误:{}", conf_file, e.what());
        return;
    };
};
