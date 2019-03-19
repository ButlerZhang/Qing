#pragma once
#include <string>
#include <new>

#pragma warning( disable : 4290 )

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

//////////////////////////////////////////////////////////////////////////////
//����06��������ʹ�ñ������Զ����ɵĺ����͸���ȷ�ܾ���
//ʾ�����룺����һ�����࣬�����������͸�ֵ������������Ϊprivate�����һ������ʹ��
//         member������friend���������޷����������ˡ�����ʹ��boost::noncopyable��
//////////////////////////////////////////////////////////////////////////////
class Uncopyable
{
protected:
    Uncopyable() {}
    ~Uncopyable() {}
private:
    Uncopyable(const Uncopyable&);
    Uncopyable& operator=(const Uncopyable&);
};

class UncopyableDerived : private Uncopyable
{
    //TODO
};

//////////////////////////////////////////////////////////////////////////////
//����52��д��placement newҲҪдplacement delete��
//ʾ�����룺����һ�����࣬�ں�������ȷ��ʽ��new��delete��
//////////////////////////////////////////////////////////////////////////////
class StandardNewDeleteForms
{
public:
    //normal new/delete
    static void* operator new(std::size_t size) throw(std::bad_alloc)
    {
        return ::operator new(size);
    }

    static void operator delete(void* pMemory) throw()
    {
        ::operator delete(pMemory);
    }

    //placement new/delete
    static void* operator new(std::size_t size, void* ptr) throw()
    {
        return ::operator new(size, ptr);
    }

    static void operator delete(void* pMemory, void* ptr) throw()
    {
        ::operator delete(pMemory, ptr);
    }

    //nothrow new/delete
    static void* operator new(std::size_t size, const std::nothrow_t& nt) throw()
    {
        return ::operator new(size, nt);
    }

    static void operator delete(void* pMemory, const std::nothrow_t&) throw()
    {
        ::operator delete(pMemory);
    }
};

class MyWidget : public StandardNewDeleteForms    //�̳б�׼��ʽ
{
public:
    //����Щ��ʽ�ɼ�
    using StandardNewDeleteForms::operator new;
    using StandardNewDeleteForms::operator delete;

    static void* operator new(std::size_t size, std::ostream& logStream) throw(std::bad_alloc)  //���һ���Զ����placement new
    {
        //TODO
    }

    static void operator delete(void* pMemory, std::ostream& logStream) throw()        //���һ���Զ����placement delete
    {
        //TODO
    }
};
