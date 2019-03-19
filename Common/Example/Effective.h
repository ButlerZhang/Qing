#pragma once
#include <new>
#include <string>

#pragma warning( disable : 4290 )



//////////////////////////////////////////////////////////////////////////////
//要点：尽量使用const。
//示例：当const和non-const成员函数有着实质等价的实现时，令non-const版本调用const
//      版本可以避免代码重复。
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

//////////////////////////////////////////////////////////////////////////////
//要点：若不想使用编译器自动生成的函数就该明确拒绝。
//示例：定义一个基类，将拷贝函数和赋值操作符都声明为private，如此一来，即使是
//      member函数或friend函数，都无法拷贝对象了。可以使用boost::noncopyable。
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
//要点：写了placement new也要写placement delete。
//示例：建立一个基类，内含所有正确形式的new和delete。
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

class MyWidget : public StandardNewDeleteForms    //继承标准形式
{
public:

    //让这些形式可见
    using StandardNewDeleteForms::operator new;
    using StandardNewDeleteForms::operator delete;

    //添加一个自定义的placement new
    static void* operator new(std::size_t size, std::ostream& logStream) throw(std::bad_alloc)
    {
        //TODO
    }

    //添加一个自定义的placement delete
    static void operator delete(void* pMemory, std::ostream& logStream) throw()
    {
        //TODO
    }
};
