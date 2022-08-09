
#ifndef UTILS
#define UTILS

#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

using namespace std;

//按空格分隔，返回vector<std::string>类型。有时间想想怎么把这个函数搞成我所有程序都能引入头文件直接用的。
vector<string> split(string text)
{
    regex ws_re("\\s+"); // 这个可以把所有空格都挖掉      而如果是regex ws_re("");就只能删掉第一个空格
    //测试一下这个字符串就明白区别了  "Quick brown    fox."
    return std::move(vector<string>(sregex_token_iterator(text.begin(), text.end(), ws_re, -1), sregex_token_iterator()));
}

#endif