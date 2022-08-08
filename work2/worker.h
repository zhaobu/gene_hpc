#ifndef WORKER
#define WORKER

#include <config.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>
using namespace std;

class Worker
{
private:
    int m_start_pos;
    int m_max_line;
    string m_sam_file;

public:
    Worker(int start_pos, int max_line, string m_sam_file);
    ~Worker();
    int start_caldepth();
};

Worker::Worker(int start_pos, int max_line, string sam_file) : m_start_pos(start_pos), m_max_line(max_line), m_sam_file(sam_file)
{
}

Worker::~Worker()
{
}

int Worker::start_caldepth()
{
    // // 读取sam文件
    // string samFileName;

    // if (argc == 2)
    // {
    //     samFileName = argv[1];
    // }

    // if (argc != 2)
    // {
    //     cout << "请输入sam文件路径: " << endl;
    //     cin >> samFileName;
    // }

    // cout << "开始统计sam文件位点深度：" << samFileName << endl;
    // ifstream inFile(samFileName);

    // // 按行解析sam文件
    // string line;
    // while (getline(inFile, line))
    // {
    //     auto field = split(line);

    //     // 头部@SQ 生成统计直方图空间
    //     if (field[0] == "@SQ")
    //     {
    //         auto chr = field[1].substr(3);
    //         auto len = stoi(field[2].substr(3));
    //         vector<int> p = vector<int>(len);
    //         wghDepthInfo[chr] = p;

    //         continue;
    //     }
    //     // 头部其他字段，跳过
    //     if (field[0][0] == '@')
    //     {
    //         continue;
    //     }

    //     // 过滤匹配失败
    //     if (stoi(field[1]) & 4)
    //     {
    //         continue;
    //     }

    //     string chr = field[2]; // 染色体名称

    //     unsigned int pos = std::stoi(field[3]);
    //     ;                        // 位点
    //     string cigar = field[5]; // cigar

    //     // 位点有序统计
    //     for (auto u = 0, i = 0; i < cigar.size(); i++)
    //     {
    //         while (isdigit(cigar[i]))
    //         {
    //             i++;
    //         }

    //         auto t = stoi(cigar.substr(u, i - u)); // 数字区间 [u, i)
    //         u = i + 1;

    //         switch (cigar[i])
    //         {
    //         case 'M': // read匹配到，统计
    //             while (t--)
    //             {
    //                 wghDepthInfo[chr][pos++]++;
    //             }
    //             break;
    //         case 'I': // read插入字符，表示多统计，Match部分的位置要先计算偏移再进行统计，Inset部分要从插入位置开始统计
    //             while (t--)
    //             {
    //                 wghDepthInfo[chr][pos++]++;
    //             }
    //             break;
    //         case 'D': // read删除字符，表示跳过
    //             pos += t;
    //             break;

    //         default: // N S H P 跳过
    //             break;
    //         }
    //     }
    // }

    // // 统计结果写入文件
    // ofstream out("./result.txt");
    // auto it = wghDepthInfo.begin();
    // while (it++ != wghDepthInfo.end())
    // {
    //     auto chr = it->first;  // key：染色体
    //     auto arr = it->second; // value：统计值

    //     for (auto i = 0; i < arr.size(); i++)
    //     {
    //         out << chr << " " << i << " " << arr[i] << endl;
    //     }
    // }

    // cout << "处理结束，结果在 ./result.txt" << endl;

    return 0;
}

#endif