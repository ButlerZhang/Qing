#pragma once
#include <string>

//////////////////////////////////////////////////////////////////////////////
//����03������ʹ��const��
//ʾ�����룺��const��non-const��Ա��������ʵ�ʵȼ۵�ʵ��ʱ����non-const�汾����const
//         �汾���Ա�������ظ���
//////////////////////////////////////////////////////////////////////////////
class TextBlock
{
public:

    const char& operator[] (std::size_t position) const
    {
        //�߽���
        //��־��¼
        //��������
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
