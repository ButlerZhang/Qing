#pragma once
#include <string>

//////////////////////////////////////////////////////////////////////////////
//条款03：尽量使用const。
//示例代码：当const和non-const成员函数有着实质等价的实现时，令non-const版本调用const
//         版本可以避免代码重复。
//////////////////////////////////////////////////////////////////////////////
class TextBlock
{
public:

    const char& operator[] (std::size_t position) const
    {
        //边界检查
        //日志记录
        //其它操作
        return  text[position];
    }

    char& operator[] (std::size_t position)
    {
        return const_cast<char&>(
            static_cast<const TextBlock&>(*this)[position]);
    }

private:
    std::string text;
};
