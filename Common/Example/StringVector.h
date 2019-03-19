#pragma once
#include <memory>
#include <string>
#include <utility>



class StringVector
{
public:

    //allocator成员进行默认初始化
    StringVector() : m_elements(nullptr), m_first_free(nullptr), m_cap(nullptr) {}

    //拷贝构造函数
    StringVector(const StringVector &sv)
    {
        //调用alloc_n_copy分配空间以容纳与s中一样多的元素
        auto newdata = alloc_n_copy(sv.begin(), sv.end());
        m_elements = newdata.first;
        m_first_free = m_cap = newdata.second;
    }

    //拷贝赋值运算符
    StringVector& operator=(const StringVector &rhs)
    {
        //调用alloc_n_copy分配内存，大小与rhs中元素占用空间一样多
        auto data = alloc_n_copy(rhs.begin(), rhs.end());

        free();
        m_elements = data.first;
        m_first_free = m_cap = data.second;
        return *this;
    }

    //普通赋值运算符
    StringVector& operator=(std::initializer_list<std::string> il)
    {
        //alloc_n_copy分配内存空间并从给定范围拷贝元素
        auto data = alloc_n_copy(il.begin(), il.end());

        free();
        m_elements = data.first;
        m_first_free = m_cap = data.second;
        return *this;
    }

    //移动构造函数，不应抛出任何异常
    StringVector(StringVector &&sv) noexcept:
        m_elements(sv.m_elements), m_first_free(sv.m_first_free), m_cap(sv.m_cap)
    {
        //确保sv运行析构函数是安全的
        sv.m_elements = sv.m_first_free = sv.m_cap = nullptr;
    }

    //移动赋值运算符
    StringVector& operator=(StringVector &&rhs) noexcept
    {
        if (this != &rhs)   //直接检测自赋值
        {
            free();         //释放已有元素

            m_elements = rhs.m_elements;
            m_first_free = rhs.m_first_free;
            m_cap = rhs.m_cap;

            rhs.m_elements = rhs.m_first_free = rhs.m_cap = nullptr;
        }

        return *this;
    }

    //析构函数
    ~StringVector() { free(); }

    //拷贝元素
    void push_back(const std::string &s)
    {
        chk_n_alloc();//确保有空间容纳新元素

        //在first_free指向的元素中构造s的副本
        m_alloc.construct(m_first_free++, s);
    }

    //移动元素
    void push_back(std::string &&s)
    {
        chk_n_alloc();
        m_alloc.construct(m_first_free++, std::move(s));
    }

    std::string& operator[](std::size_t n) { return m_elements[n]; }
    const std::string& operator[](std::size_t n) const { return m_elements[n]; }

    std::size_t size() const { return m_first_free - m_elements; }
    std::size_t capacity() const { return m_cap - m_elements; }
    std::string* begin() const { return m_elements; }
    std::string* end() const { return m_first_free; }

private:

    //被添加元素的函数所使用
    void chk_n_alloc()
    {
        if (size() == capacity())
        {
            reallocate();
        }
    }

    //工具函数，被拷贝构造函数、赋值运算符和析构函数使用
    std::pair<std::string*, std::string*>
        alloc_n_copy(const std::string *b, const std::string *e)
    {
        //分配空间保存给定范围中的元素
        auto data = m_alloc.allocate(e - b);

        //列表初始化并返回一个pair
        return { data, std::uninitialized_copy(b,e,data) };
    }

    //销毁元素并释放内存
    void free()
    {
        if (m_elements)
        {
            for (auto p = m_first_free; p != m_elements; /*空*/)
            {
                m_alloc.destroy(--p);
            }

            m_alloc.deallocate(m_elements, m_cap - m_elements);
        }
    }

    //获得更多内存并拷贝已有元素
    void reallocate()
    {
        //分配当前大小两倍的内存空间
        auto newcapacity = size() ? 2 * size() : 1;
        //分配新内存
        auto newdata = m_alloc.allocate(newcapacity);

        //将数据从旧内存移动到新内存
        auto dest = newdata;    //指向新数组中下一个空闲位置
        auto elem = m_elements; //指向旧数组中下一个元素

        for (std::size_t i = 0; i != size(); ++i)
        {
            m_alloc.construct(dest++, std::move(*elem++));
        }

        free();//移动完元素就释放旧内存空间

        //更新数据结构，执行新元素
        m_elements = newdata;
        m_first_free = dest;
        m_cap = m_elements + newcapacity;
    }

private:

    std::string *m_elements;                        //指向数组首元素的指针
    std::string *m_first_free;                      //指向数组第一个空闲元素的指针
    std::string *m_cap;                             //指向数组尾后位置的指针
    static std::allocator<std::string> m_alloc;     //分配元素
};
