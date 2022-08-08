#ifndef SAMTOOLS
#define SAMTOOLS

#include "spdlog/spdlog.h"
#include <config.h>
#include <fstream>
#include <iostream>
#include <iterator>
// #include <map>
// #include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <utils.h>
#include <vector>
// #include <worker.h>
#include <mutex>
using namespace std;

enum FLAG
{
    FLAG_BAM_FUNMAP = 1 << 2,
    FLAG_AM_FSECONDARY = 1 << 8,
    FLAG_BAM_FQCFAIL = 1 << 9,
    FLAG_BAM_FDUP = 1 << 10,
};

const int KCH_TOTAL_NUM = 94; //染色体总条数

using DepthInfo = unordered_map<string, vector<int>>;
using DepthInfoLock = unordered_map<string, shared_ptr<mutex>>;
// using VecWorker = vector<std::shared_ptr<Worker>>;
using VecThread = vector<std::thread>;

//比对信息
class AlignmenSsection
{
private:
    unsigned int m_flag; // FLAG
    string m_qname;      // qname
    string m_rname;      // 染色体名称
    unsigned int m_pos;  // 位点
    string m_cigar;      // cigar值

public:
    AlignmenSsection(/* args */);
    AlignmenSsection(string qname, unsigned int flag, string rname, unsigned int pos, string cigar);
    ~AlignmenSsection();
};

AlignmenSsection::AlignmenSsection(/* args */)
{
}

AlignmenSsection::~AlignmenSsection()
{
}

AlignmenSsection::AlignmenSsection(string qname, unsigned int flag, string rname, unsigned int pos, string cigar)
{
    m_qname = qname;
    m_flag = flag;
    m_rname = rname;
    m_pos = pos;
    m_cigar = cigar;
}

class Samtools
{
private:
    /* data */
    Configer m_conf;
    // VecWorker m_vec_worker;
    DepthInfo m_depth_result;
    VecThread m_threads;
    unordered_set<string> m_filter_read; //过滤多线程时重复读取的的行
    DepthInfoLock m_depth_result_locks;  //同步锁

public:
    Samtools();
    Samtools(Configer &conf);
    ~Samtools();
    //开始计算
    int start_work();

    //读取头
    int read_header(int &start_pos, int &file_size);

    //读取数据部分
    int read_data(int start_pos, int next_pos, int tid);

    //计算当前行的测序深度
    int cal_line(string qname, unsigned int flag, string rname, unsigned int pos, string cigar);

    int static_result();
};

Samtools::Samtools(/* args */)
{
}
Samtools::Samtools(Configer &conf) : m_conf(conf)
{
    m_filter_read.reserve(conf.get_thread());
    m_threads.reserve(conf.get_thread());
    // m_vec_worker.reserve(conf.get_thread());
}

Samtools::~Samtools()
{
}

// 读取头文件信息，构建深度统计信息
int Samtools::read_header(int &start_pos, int &file_size)
{
    ifstream inFile(m_conf.get_sam_file());
    if (!inFile)
    {
        spdlog::error("打开文件失败");
        return 0;
    }
    //把指针指到文件末尾求出文件大小
    file_size = inFile.seekg(0, ios::end).tellg();
    //指针重新定位到文件开头
    inFile.seekg(0, ios::beg);

    string text;
    start_pos = 0;
    while (getline(inFile, text))
    {
        auto cols = split(text);

        // 读取到非头文件内容就停止读取
        if (cols[0][0] != '@')
        {
            //记录读取的位置
            start_pos = inFile.tellg();
            break;
        }
        // 头部其他字段，跳过
        if (cols[0].compare("@SQ") == 0)
        {
            auto sn = cols[1].substr(3);       //得到SN字段
            auto ln = stoi(cols[2].substr(3)); //得到LN字段
            m_depth_result[sn] = vector<int>(ln);
            m_depth_result_locks[sn] = make_shared<mutex>();
        }
    };

    inFile.close();
    spdlog::info("头文件信息读取成功，共有{}条染色体，文件大小为{}", m_depth_result.size(), file_size);
    return 0;
}

//读取文件数据
int Samtools::read_data(int start_pos, int next_pos, int tid)
{
    spdlog::info("第{}号线程开始读取，start_pos={}，next_pos={}", tid, start_pos, next_pos);

    ifstream inFile(m_conf.get_sam_file());
    inFile.seekg(start_pos, ios::beg);

    //如果当前位置不在行尾,就跳过一行
    string line;
    if (start_pos != 0)
    {
        char cur_ch;
        inFile.read(&cur_ch, 1);
        // ss << "Thread " << tid << ", cur_ch=" << cur_ch << endl;
        if (cur_ch != '\n')
        {
            getline(inFile, line);
        }
    }
    //读取一行并记录到set中
    bool record_filter = true;

    while (start_pos <= next_pos && getline(inFile, line))
    {
        start_pos = inFile.tellg(); //记录当前读取到的位置

        auto cols = split(line);

        unsigned int flag = std::stoi(cols[1]);

        // flag值
        // FLAG_BAM_FUNMAP = 1 << 2,
        // FLAG_AM_FSECONDARY = 1 << 8,
        // FLAG_BAM_FQCFAIL = 1 << 9,
        // FLAG_BAM_FDUP = 1 << 10,

        //根据flag值过滤匹配失败的情况
        if (flag & FLAG_BAM_FUNMAP || flag & FLAG_AM_FSECONDARY || flag & FLAG_BAM_FQCFAIL || flag & FLAG_BAM_FDUP)
        {
            continue;
        }

        string qname = cols[0];                // qname
        string rname = cols[2];                // 染色体名称
        unsigned int pos = std::stoi(cols[3]); // 位点
        string cigar = cols[5];                // cigar值

        // string filter_key = qname + cols[3];
        // if (m_filter_read.count(filter_key) > 0)
        // {
        //     continue;
        // }
        // if (record_filter)
        // {
        //     m_filter_read.insert(filter_key);
        //     record_filter = false;
        // }
        cal_line(qname, flag, rname, pos, cigar);
    }
    spdlog::info("第{}号线程读取完毕start_pos={}，next_pos={}, tellg={}", tid, start_pos, next_pos, inFile.tellg());
    return 0;
}

int Samtools::cal_line(string qname, unsigned int flag, string rname, unsigned int pos, string cigar)
{
    auto &cur_lock = m_depth_result_locks[rname];
    // 根据cigar值来统计深度
    for (auto start = 0, i = 0; i < cigar.size(); i++)
    {
        if (isdigit(cigar[i]))
        {
            continue;
        }

        // substr截取范围为[starg,end)
        unsigned int t = stoi(cigar.substr(start, i - start));
        // 3M1D2M1I1M
        switch (cigar[i])
        {
        // case 'M': // M 表示比对匹配,进行统计
        //     while (t--)
        //     {
        //         cur_lock->lock();
        //         m_depth_result[rname][pos++]++;
        //         cur_lock->unlock();
        //     }
        //     break;
        // case 'I':
        //     while (t--)
        //     {
        //         cur_lock->lock();
        //         m_depth_result[rname][pos++]++;
        //         cur_lock->unlock();
        //     }
        //     break;
        case 'D':
            pos += t;
        default:
            cur_lock->lock();
            m_depth_result[rname][pos++]++;
            cur_lock->unlock();
            break;
        }

        //读取下一个数字
        start = i + 1;
    }
    return 0;
}

int Samtools::start_work()
{
    spdlog::info("开始统计每条染色体每个位点深度");
    int start_pos = 0;
    int file_size = 0;
    read_header(start_pos, file_size);

    ifstream inFile(m_conf.get_sam_file());
    inFile.seekg(start_pos, ios::beg); //直接跳过headr部分

    int thread_num = m_conf.get_thread();

    int once_size = (file_size - start_pos) / thread_num;
    int next_pos = start_pos;

    //开启多线程处理
    int tid = 0;
    for (; tid < thread_num; tid++)
    {
        next_pos += once_size;
        std::thread th(&Samtools::read_data, this, start_pos, next_pos, tid);
        m_threads.emplace_back(std::move(th));
        start_pos = next_pos;
    }

    //启动线程
    for (auto &th : m_threads)
    {
        th.join();
    }

    //主线程读取剩下的部分
    if ((file_size - start_pos) % thread_num != 0)
    {
        read_data(start_pos, next_pos, tid);
    }

    static_result();
    return 0;
}

int Samtools::static_result()
{
    spdlog::info("开始写入结果到{}", m_conf.get_result_file());

    // 统计结果写入文件
    ofstream out(m_conf.get_result_file(), ios::out);
    if (!out)
    {
        out.close(); //程序结束前不能忘记关闭以前打开过的文件
        spdlog::info("打开文件{}失败", m_conf.get_result_file());
        return 0;
    }

    for (auto &info : m_depth_result)
    {
        auto rname = info.first;       //染色体
        auto depth_info = info.second; //深度
        auto count = depth_info.size();
        for (unsigned int pos = 0; pos < count; pos++)
        {
            out << rname << "\t" << pos << "\t" << depth_info[pos] << endl;
        }
    }
    spdlog::info("结果已经保存到{}", m_conf.get_result_file());
    out.close();
    return 0;
}

#endif