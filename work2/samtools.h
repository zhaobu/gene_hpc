#pragma once

#include "spdlog/spdlog.h"
#include <atomic>
#include <config.h>
#include <fstream>
#include <iostream>
#include <iterator>
// #include <map>
#include <memory>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <utils.h>
#include <vector>
// #include <worker.h>
// #include <mutex>

using namespace std;
#define CAL_ONE ;

enum FLAG
{
    FLAG_BAM_FUNMAP = 1 << 2,
    FLAG_AM_FSECONDARY = 1 << 8,
    FLAG_BAM_FQCFAIL = 1 << 9,
    FLAG_BAM_FDUP = 1 << 10,
};

const int KCH_TOTAL_NUM = 94; //染色体总条数

using DepthInfo = unordered_map<string, vector<atomic<long>>>;
// using DepthInfoLock = unordered_map<string, vector<shared_ptr<mutex>>>;
// using VecWorker = vector<std::shared_ptr<Worker>>;
using VecThread = vector<std::thread>;

//比对信息
class AlignmenSection
{
private:
    unsigned int m_flag;

public:
    unsigned int getMFlag() const
    {
        return m_flag;
    }

    void setMFlag(unsigned int mFlag)
    {
        m_flag = mFlag;
    }

    const string &getMQname() const
    {
        return m_qname;
    }

    void setMQname(const string &mQname)
    {
        m_qname = mQname;
    }

    const string &getMRname() const
    {
        return m_rname;
    }

    void setMRname(const string &mRname)
    {
        m_rname = mRname;
    }

    unsigned int getMPos() const
    {
        return m_pos;
    }

    void setMPos(unsigned int mPos)
    {
        m_pos = mPos;
    }

    const string &getMCigar() const
    {
        return m_cigar;
    }

    void setMCigar(const string &mCigar)
    {
        m_cigar = mCigar;
    }

private:
    // FLAG
    string m_qname;     // qname
    string m_rname;     // 染色体名称
    unsigned int m_pos; // 位点
    string m_cigar;     // cigar值

public:
    AlignmenSection(/* args */);

    AlignmenSection(string qname, unsigned int flag, string rname, unsigned int pos, string cigar);

    ~AlignmenSection();
};

AlignmenSection::AlignmenSection(/* args */)
{
}

AlignmenSection::~AlignmenSection()
{
}

AlignmenSection::AlignmenSection(string qname, unsigned int flag, string rname, unsigned int pos, string cigar)
{
    m_qname = std::move(qname);
    m_flag = flag;
    m_rname = std::move(rname);
    m_pos = pos;
    m_cigar = std::move(cigar);
}

class Tools
{
};

class Samtools
{
private:
    /* data */
    Configer m_conf;
    // VecWorker m_vec_worker;
    DepthInfo m_depth_result;
    VecThread m_threads;
    // DepthInfoLock m_depth_result_locks; //同步锁

public:
    Samtools();

    Samtools(Configer &conf);

    ~Samtools();

    //开始计算
    int start_work();

    //读取头
    int read_header(std::streampos &start_pos, unsigned long &file_size);

    //读取数据部分
    int read_data(int tid, std::streampos start_pos, unsigned long read_size);

    //计算当前行的测序深度
    int cal_line(const string &rname, unsigned int pos, const string &cigar);

    int static_result();
};

Samtools::Samtools(/* args */)
{
}

Samtools::Samtools(Configer &conf) : m_conf(conf)
{
    m_threads.reserve(conf.get_thread());
    // m_vec_worker.reserve(conf.get_thread());
}

Samtools::~Samtools()
{
}

// 读取头文件信息，构建深度统计信息
int Samtools::read_header(std::streampos &start_pos, unsigned long &file_size)
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
    vector<string> cols(3);
    // cols.reserve(3);
    streampos last_line_pos = 0; //记录上一行的位置
    while (getline(inFile, text))
    {
        split(text, cols, '\t');

        // 读取到非头文件内容就停止读取
        if (cols[0][0] != '@')
        {
            /*
            1. 记录读取的位置时,已经读取了一行数据部分,所以数据部分起始点应该是上一行的结束位置
            2. read_dat实现是如果是在行中间,就跳过该行,所以应该用last_line_pos-1表示指针移动到上一行的换行符
             */
            start_pos = last_line_pos - 1l;
            break;
        }

        // 头部其他字段，跳过
        if (cols[0].compare("@SQ") == 0)
        {
            auto sn = cols[1].substr(3);       //得到SN字段
            auto ln = stoi(cols[2].substr(3)); //得到LN字段
#ifdef CAL_ONE
            if (sn != m_conf.get_target_rname())
            {
                last_line_pos = inFile.tellg();
                continue;
            }
#endif
            m_depth_result[sn] = vector<atomic<long>>(ln);
            // m_depth_result_locks[sn] = vector<shared_ptr<mutex>>(ln / 10, make_shared<mutex>());
        }
        last_line_pos = inFile.tellg();
    };

    inFile.close();
    spdlog::info("头文件信息读取成功，共有{}条染色体，头部结束位置为{}, 总文件大小为{}", m_depth_result.size(), start_pos, file_size);
    return 0;
}

//读取文件数据
int Samtools::read_data(int tid, std::streampos start_pos, unsigned long read_size)
{
    spdlog::info("第{}号线程开始读取，start_pos={}, read_size={}", tid, start_pos, read_size);

    ifstream inFile(m_conf.get_sam_file());
    if (!inFile.good())
    {
        spdlog::error("第{}号线程打开文件失败，start_pos={}, read_size={}", tid, start_pos, read_size);
        inFile.close();
        return -1;
    }

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

    std::streampos cur_pos = inFile.tellg();
    unsigned long read_count = cur_pos - start_pos;
    vector<string> cols(18);
    // cols.reserve(18);
    while (read_count <= read_size && !inFile.eof())
    {
        getline(inFile, line);
        if (line.empty())
        {
            continue;
        }
        unsigned long cur_line_len = inFile.tellg() - cur_pos; //当前行读的长度
        read_count += cur_line_len;                            //累计读取长度
        cur_pos = inFile.tellg();                              //记录当前读取到的位置

        split(line, cols, '\t');

        string &rname = cols[2]; // 染色体名称
#ifdef CAL_ONE
        if (rname != m_conf.get_target_rname())
        {
            continue;
        }
#endif
        int flag = std::stoi(cols[1]);
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

        // string qname = cols[0];                // qname
        unsigned int pos = std::stoi(cols[3]); // 位点
        string &cigar = cols[5];               // cigar值

        cal_line(rname, pos, cigar);
    }
    spdlog::info("第{}号线程读取完毕start_pos={}, read_size={},读取结束后pos={} ", tid, start_pos, read_size, inFile.tellg());
    inFile.close();
    return 0;
}

int Samtools::cal_line(const string &rname, unsigned int pos, const string &cigar)
{
    // auto &lock_list = m_depth_result_locks[rname];
    // auto &cur_lock = lock_list[pos % lock_list.size()];
    // cur_lock->lock();
    // 根据cigar值来统计深度
    for (std::size_t start = 0, i = 0; i < cigar.size(); i++)
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
        case 'M': // M 表示比对匹配,进行统计
            while (t--)
            {
                // auto &cur_lock = lock_list[pos % lock_list.size()];
                // cur_lock->lock();
                m_depth_result[rname][pos++]++;
                // cur_lock->unlock();
            }
            break;
        case 'D':
            pos += t;
        default:
            break;
        }

        //读取下一个数字
        start = i + 1;
    }
    // cur_lock->unlock();

    return 0;
}

int Samtools::start_work()
{
    spdlog::info("开始统计每条染色体每个位点深度,读取文件{}", m_conf.get_sam_file());
    std::streampos start_pos = 0; //需要多线程读的起始位置
    unsigned long file_size = 0;  //总文件大小
    read_header(start_pos, file_size);

    int thread_num = m_conf.get_thread();             //线程数量
    unsigned long read_size = file_size - start_pos;  //需要多线程读取的部分不包括header
    unsigned long once_size = read_size / thread_num; //每个线程平均读取的字节数

    //开启多线程处理
    int tid = 0;

    for (; tid < thread_num; tid++)
    {
        std::thread th(&Samtools::read_data, this, tid, start_pos, once_size);
        m_threads.emplace_back(std::move(th));
        start_pos += once_size;
    }
    //启动线程
    for (auto &th : m_threads)
    {
        th.join();
    }
    //主线程读取剩下的部分
    if (read_size % thread_num != 0)
    {
        long left_size = file_size - start_pos;
        spdlog::info("剩下的部分{}全部由主线程来读取,file_size={},start_pos={}", left_size, file_size, start_pos);
        read_data(tid, start_pos, left_size);
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

    for (const auto &info : m_depth_result)
    {
        auto rname = info.first;        //染色体
        auto &depth_info = info.second; //深度
        auto count = depth_info.size();
#ifdef CAL_ONE
        if (rname != m_conf.get_target_rname())
        {
            continue;
        }
#endif
        for (unsigned int pos = 0; pos < count; pos++)
        {
            if (depth_info[pos] > 0)
            {
                out << rname << "\t" << pos << "\t" << depth_info[pos] << endl;
            }
        }
    }
    spdlog::info("结果已经保存到{}", m_conf.get_result_file());
    out.close();
    return 0;
}