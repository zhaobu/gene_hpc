#ifndef CONFIG
#define CONFIG
// #include "yaml-cpp/yaml.h"
#include <fstream>
#include <iostream>
#include <string>
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

Configer::Configer(const string &conf_file){
    // YAML::Node config = YAML::LoadFile(conf_file);
    // std::cout << "config:" << config.as<std::string>() << endl;
    // if (!config["sam_file"])
    // {
    //     std::cout << "sam_file未配置" << endl;
    //     return;
    // }

    // if (!config["thread"])
    // {
    //     std::cout << "thread 未配置" << endl;
    //     return;
    // }

    // if (!config["result_file"])
    // {
    //     std::cout << "result_file 未配置" << endl;
    //     return;
    // }
    // m_sam_file = config["sam_file"].as<string>();
    // m_thread = config["thread"].as<int>();
    // m_result_file = config["result_file"].as<string>();

    // std::ofstream fout("config.yaml");
    // fout << config;
};

#endif
