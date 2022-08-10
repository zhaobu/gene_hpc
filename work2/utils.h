
#ifndef UTILS
#define UTILS
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
// #include <string_view>
#include <chrono>
#include <iomanip>
#include <vector>
using namespace std;

//按空格分隔，返回vector<string>类型。有时间想想怎么把这个函数搞成我所有程序都能引入头文件直接用的。
vector<string> split(string text)
{
    regex ws_re("\\s+"); // 这个可以把所有空格都挖掉      而如果是regex ws_re("");就只能删掉第一个空格
    //测试一下这个字符串就明白区别了  "Quick brown    fox."
    return move(vector<string>(sregex_token_iterator(text.begin(), text.end(), ws_re, -1), sregex_token_iterator()));
}

void split(const string &s, vector<string> &tokens, char delim = ' ')
{
    tokens.clear();
    auto string_find_first_not = [s, delim](size_t pos = 0) -> size_t
    {
        for (size_t i = pos; i < s.size(); i++)
        {
            if (s[i] != delim)
                return i;
        }
        return string::npos;
    };
    size_t lastPos = string_find_first_not(0);
    size_t pos = s.find(delim, lastPos);
    while (lastPos != string::npos)
    {
        tokens.emplace_back(s.substr(lastPos, pos - lastPos));
        lastPos = string_find_first_not(pos);
        pos = s.find(delim, lastPos);
    }
}
// vector<string> split(const string &str, const string &delims = " ")
// {
//     vector<string> output;
//     auto first = cbegin(str);

//     while (first != cend(str))
//     {
//         const auto &second = find_first_of(first, cend(str),
//                                            cbegin(delims), cend(delims));

//         if (first != second)
//             output.emplace_back(first, second);

//         if (second == cend(str))
//             break;

//         first = next(second);
//     }

//     return output;
// }

// vector<std::string_view> split(string_view strv, string_view delims = " ")
// {
//     vector<string_view> output;
//     size_t first = 0;

//     while (first < strv.size())
//     {
//         const auto second = strv.find_first_of(delims, first);

//         if (first != second)
//             output.emplace_back(strv.substr(first, second - first));

//         if (second == string_view::npos)
//             break;

//         first = second + 1;
//     }

//     return output;
// }

template <typename... Args>
static std::string str_format(const std::string &format, Args... args)
{
    auto size_buf = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new (std::nothrow) char[size_buf]);

    if (!buf)
        return std::string("");

    std::snprintf(buf.get(), size_buf, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size_buf - 1);
}

template <typename... Args>
static std::wstring wstr_format(const std::wstring &format, Args... args)
{
    auto size_buf = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new (std::nothrow) char[size_buf]);

    if (!buf)
        return std::wstring("");

    std::snprintf(buf.get(), size_buf, format.c_str(), args...);
    return std::wstring(buf.get(), buf.get() + size_buf - 1);
}

string get_current_time(const string &str = "%Y-%m-%d %H:%M:%S")
{
    auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()); //时间点可以做差

    stringstream ss;
    ss << std::put_time(localtime(&t), str.c_str());
    return ss.str();
}

#endif