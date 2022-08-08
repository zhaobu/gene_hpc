#include <samtools.h>
#include <config.h>
#include <string>
using namespace std;

bool check_args(int arg_num, string text)
{
    return true;
}

int main(int argc, char *argv[])
{

    if (argc != 3)
    {
        cout << "请输入sam文件路径和线程个数, eg: ./main.out tmp.sam 2" << endl;
        return 0;
    }
    string sam_file = argv[1];
    int thread_num = stoi(argv[2]);

    cout << "输入的sam文件路径为" << sam_file << endl;
    cout << "请输入线程个数" << thread_num << endl;

    Configer conf(sam_file, thread_num);
    Samtools samtools(conf);
    samtools.start_work();
    return 0;
}