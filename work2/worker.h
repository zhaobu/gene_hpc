#pragma once

#include <config.h>
#include <fstream>
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
    return 0;
}