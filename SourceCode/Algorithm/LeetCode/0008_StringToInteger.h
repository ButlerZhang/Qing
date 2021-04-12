#pragma once
#include <iostream>
#include "LeetCodeCommon.h"

LEET_CODE_NS_BEGIN

class StringToInteger{
public:
    /*
    * 题目中的算法步骤：
    *
    * 1.读入字符串并丢弃无用的前导空格。
    * 2.检查下一个字符（假设还未到字符末尾）为正还是负号，
    *   读取该字符（如果有）。确定最终结果是负数还是正数。
    *   如果两者都不存在，则假定结果为正。
    * 3.读入下一个字符，直到到达下一个非数字字符或到达输
    *   入的结尾。字符串的其余部分将被忽略。
    * 4.将前面步骤读入的这些数字转换为整数（即，"123"->123，"0032"->32）。
    *   如果没有读入数字，则整数为0。必要时更改符号（从步骤2开始）。
    * 5.如果整数数超过32位有符号整数范围[−2^31,2^31−1]，
    *   需要截断这个整数，使其保持在这个范围内。具体来说，
    *   小于−2^31的整数应该被固定为−2^31，大于2^31−1的整数应该被固定为2^31−1。
    * 6.返回整数作为最终结果。
    */

    int myAtoi(string s) {
        int result = 0;

        //先找到非空格的第一个字符下标
        std::string::size_type start = 0;
        while (start < s.size() && s[start] == ' ') {
            start++;
        }

        if (start >= s.size()) {
            return result;
        }

        //判断正负
        bool isPositive = true;
        if (s[start] == '+') {
            start++;
        }
        else if (s[start] == '-') {
            isPositive = false;
            start++;
        }

        if (start >= s.size()) {
            return result;
        }

        int single = 0;
        const int base = 10;

        //根据正负分别计算
        if (isPositive) {
            const int MIN_POP = INT32_MAX % base;
            const int MAX_LIMIT = INT32_MAX / base;
            for (std::string::size_type index = start; index < s.size(); index++) {
                //非数字字符就不处理了
                if (s[index] < '0' || s[index] > '9') {
                    break;
                }

                //将单个字符转换成单个数字
                single = s[index] - '0';

                //判断是否超过了最大值
                if (result > MAX_LIMIT || (result == MAX_LIMIT && single > MIN_POP)) {
                    result = INT32_MAX;
                    break;
                }

                //累积当前数字
                result = result * base + single;
            }
        }
        else {
            const int MAX_POP = INT32_MIN % base;
            const int MIN_LIMIT = INT32_MIN / base;
            for (std::string::size_type index = start; index < s.size(); index++) {
                if (s[index] < '0' || s[index] > '9') {
                    break;
                }

                single = s[index] - '0';

                //判断是否小于最小值，这里需要特别注意要把single转成负数来判断
                if (result < MIN_LIMIT || (result == MIN_LIMIT && (-single) < MAX_POP)) {
                    result = INT32_MIN;
                    break;
                }

                //注意这里是减法
                result = result * base - single;
            }
        }

        return result;
    }

    static void testCase() {
        StringToInteger test;

        assert(test.myAtoi("42") == 42);
        assert(test.myAtoi("   -42") == -42);
        assert(test.myAtoi("4193 with words") == 4193);
        assert(test.myAtoi("words and 987") == 0);
        assert(test.myAtoi("-91283472332") == INT32_MIN);

        assert(test.myAtoi("     ") == 0);
        assert(test.myAtoi("+") == 0);
        assert(test.myAtoi("-") == 0);
        assert(test.myAtoi("+42") == 42);
        assert(test.myAtoi(".42") == 0);
        assert(test.myAtoi("1.42") == 1);
        assert(test.myAtoi("0022") == 22);
        assert(test.myAtoi("192039591283472332") == INT32_MAX);
        assert(test.myAtoi("hello") == 0);
        assert(test.myAtoi("-2147483649") == INT32_MIN); //error case
        assert(test.myAtoi("-2147483648") == INT32_MIN);
        assert(test.myAtoi("2147483647") == INT32_MAX);
        assert(test.myAtoi("2147483648") == INT32_MAX);
    }
};

LEET_CODE_NS_END