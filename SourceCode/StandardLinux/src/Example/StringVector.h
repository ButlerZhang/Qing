#pragma once
#include <memory>
#include <string>
#include <utility>



class StringVector
{
public:

    //allocator��Ա����Ĭ�ϳ�ʼ��
    StringVector() : m_elements(nullptr), m_first_free(nullptr), m_cap(nullptr) {}

    //�������캯��
    StringVector(const StringVector &sv)
    {
        //����alloc_n_copy����ռ���������s��һ�����Ԫ��
        auto newdata = alloc_n_copy(sv.begin(), sv.end());
        m_elements = newdata.first;
        m_first_free = m_cap = newdata.second;
    }

    //������ֵ�����
    StringVector& operator=(const StringVector &rhs)
    {
        //����alloc_n_copy�����ڴ棬��С��rhs��Ԫ��ռ�ÿռ�һ����
        auto data = alloc_n_copy(rhs.begin(), rhs.end());

        free();
        m_elements = data.first;
        m_first_free = m_cap = data.second;
        return *this;
    }

    //��ͨ��ֵ�����
    StringVector& operator=(std::initializer_list<std::string> il)
    {
        //alloc_n_copy�����ڴ�ռ䲢�Ӹ�����Χ����Ԫ��
        auto data = alloc_n_copy(il.begin(), il.end());

        free();
        m_elements = data.first;
        m_first_free = m_cap = data.second;
        return *this;
    }

    //�ƶ����캯������Ӧ�׳��κ��쳣
    StringVector(StringVector &&sv) noexcept:
        m_elements(sv.m_elements), m_first_free(sv.m_first_free), m_cap(sv.m_cap)
    {
        //ȷ��sv�������������ǰ�ȫ��
        sv.m_elements = sv.m_first_free = sv.m_cap = nullptr;
    }

    //�ƶ���ֵ�����
    StringVector& operator=(StringVector &&rhs) noexcept
    {
        if (this != &rhs)   //ֱ�Ӽ���Ը�ֵ
        {
            free();         //�ͷ�����Ԫ��

            m_elements = rhs.m_elements;
            m_first_free = rhs.m_first_free;
            m_cap = rhs.m_cap;

            rhs.m_elements = rhs.m_first_free = rhs.m_cap = nullptr;
        }

        return *this;
    }

    //��������
    ~StringVector() { free(); }

    //����Ԫ��
    void push_back(const std::string &s)
    {
        chk_n_alloc();//ȷ���пռ�������Ԫ��

        //��first_freeָ���Ԫ���й���s�ĸ���
        m_alloc.construct(m_first_free++, s);
    }

    //�ƶ�Ԫ��
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

    //�����Ԫ�صĺ�����ʹ��
    void chk_n_alloc()
    {
        if (size() == capacity())
        {
            reallocate();
        }
    }

    //���ߺ��������������캯������ֵ���������������ʹ��
    std::pair<std::string*, std::string*>
        alloc_n_copy(const std::string *b, const std::string *e)
    {
        //����ռ䱣�������Χ�е�Ԫ��
        auto data = m_alloc.allocate(e - b);

        //�б��ʼ��������һ��pair
        return { data, std::uninitialized_copy(b,e,data) };
    }

    //����Ԫ�ز��ͷ��ڴ�
    void free()
    {
        if (m_elements)
        {
            for (auto p = m_first_free; p != m_elements; /*��*/)
            {
                m_alloc.destroy(--p);
            }

            m_alloc.deallocate(m_elements, m_cap - m_elements);
        }
    }

    //��ø����ڴ沢��������Ԫ��
    void reallocate()
    {
        //���䵱ǰ��С�������ڴ�ռ�
        auto newcapacity = size() ? 2 * size() : 1;
        //�������ڴ�
        auto newdata = m_alloc.allocate(newcapacity);

        //�����ݴӾ��ڴ��ƶ������ڴ�
        auto dest = newdata;    //ָ������������һ������λ��
        auto elem = m_elements; //ָ�����������һ��Ԫ��

        for (std::size_t i = 0; i != size(); ++i)
        {
            m_alloc.construct(dest++, std::move(*elem++));
        }

        free();//�ƶ���Ԫ�ؾ��ͷž��ڴ�ռ�

        //�������ݽṹ��ִ����Ԫ��
        m_elements = newdata;
        m_first_free = dest;
        m_cap = m_elements + newcapacity;
    }

private:

    std::string *m_elements;                        //ָ��������Ԫ�ص�ָ��
    std::string *m_first_free;                      //ָ�������һ������Ԫ�ص�ָ��
    std::string *m_cap;                             //ָ������β��λ�õ�ָ��
    static std::allocator<std::string> m_alloc;     //����Ԫ��
};
