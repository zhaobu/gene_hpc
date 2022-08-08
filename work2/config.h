#ifndef CONFIG
#define CONFIG

#include <string>

using namespace std;

class Configer
{
private:
    /* data */
    string m_sam_file = "data/work2.sam";
    int m_thread = 10;
    string m_result_file = "data/result.log";

public:
    Configer(/* args */);
    Configer(string sam_file, int thread);
    ~Configer();

    string get_sam_file()
    {
        return m_sam_file;
    };
    int get_thread()
    {
        return m_thread;
    };
    string get_result_file()
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

Configer::Configer(string sam_file, int thread) : m_sam_file(sam_file), m_thread(thread){};

#endif
