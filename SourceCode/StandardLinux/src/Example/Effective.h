#pragma once
#include <new>
#include <list>
#include <string>

#pragma warning( disable : 4290 )
#pragma warning( disable : 4996 )



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
        return const_cast<char&>(static_cast<const TextBlock&>(*this)[position]);
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

class UncopyableExample : private Uncopyable
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

class StandardNewDeleteFormsExample : public StandardNewDeleteForms    //继承标准形式
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



//////////////////////////////////////////////////////////////////////////////
//要点：单例模式的简单实现
//示例：阻止建立某个类的对象，最容易的方法就是把该类的构造函数声明在类的private域。
//     如果希望只拥有一个实例对象，则可以在类内声明一个友元函数，在这个函数里包括一个静态对象。
//////////////////////////////////////////////////////////////////////////////
class SingleClassExample
{
private:
    SingleClassExample();

public:
    //TODO

    friend SingleClassExample& Instance()     //相当于定义了全局函数
    {
        //声明为函数内的静态对象，有两个好处：
        //1.可以避免像静态成员一样被构造和释放，哪怕不使用该对象。
        //2.静态成员的初始化时间不确定。函数内静态对象只有在第一次执行函数时，才会被构造。
        static SingleClassExample instance;
        return instance;
    }

    //static SingleClassExample& Instance();   //也可以使用静态函数
};



//////////////////////////////////////////////////////////////////////////////
//要点：具有对象计数功能的基类。
//示例：定义静态成员来跟踪对象的数量，一旦超过限制则抛出异常。
//////////////////////////////////////////////////////////////////////////////
template< typename BeingCounted >
class CountedStandard
{
public:
    class TooManyObjects { };       //用来抛出异常
    static int objectCount() { return m_numObjects; }

protected:
    CountedStandard();
    CountedStandard(const CountedStandard& rhs);
    ~CountedStandard() { --m_numObjects; }

private:
    static int              m_numObjects;
    static const size_t     m_maxObjects;

    void init();                    //避免构造函数的代码重复
};

template<typename BeingCounted>
CountedStandard<BeingCounted>::CountedStandard() { init(); }

template<typename BeingCounted>
CountedStandard<BeingCounted>::CountedStandard(const CountedStandard<BeingCounted>&) { init(); }

template<typename BeingCounted>
void CountedStandard<BeingCounted>::init()
{
    if (m_numObjects >= m_maxObjects) throw TooManyObjects();
    ++m_numObjects;
}

class PrinterCountedExample : private CountedStandard<PrinterCountedExample>    //private继承
{
public:
    ~PrinterCountedExample() {}

    //建立对象的环境：伪构造函数
    //即由下面两个函数产生Printer，不要独自产生Printer。
    //下面两个函数通过调用构造函数和拷贝构造函数才生新对象。
    static PrinterCountedExample* makePrinter() { return new PrinterCountedExample(); }
    static PrinterCountedExample* makePrinter(const PrinterCountedExample& lhs) { return new PrinterCountedExample(lhs); }

    //TODO
    //void reset();
    //void submitJob(const PrintJob& Job);

    //使用using声明使得它们具有public属性
    using CountedStandard<PrinterCountedExample>::objectCount;
    using CountedStandard<PrinterCountedExample>::TooManyObjects;

private:
    PrinterCountedExample();
    PrinterCountedExample(const PrinterCountedExample& rhs);
};

//在Counted的实现文件中定义numObjects，自动初始化为0。
template<typename BeingCounted> int CountedStandard<BeingCounted>::m_numObjects;

//对于maxObject则定义在派生类的实现文件中
const size_t CountedStandard<PrinterCountedExample>::m_maxObjects = 10;



//////////////////////////////////////////////////////////////////////////////
//要点：单纯计算类对象的个数
//////////////////////////////////////////////////////////////////////////////
template<typename T>
class CounterTemplate
{
public:

    CounterTemplate() { ++m_count; }
    CounterTemplate(const CounterTemplate&) { ++m_count; }
    ~CounterTemplate() { --m_count; }
    static size_t howMany() { return m_count; }

private:

    //尽管m_count是静态成员，但CounterTemplate对象仍然占用至少有1byte的空间
    //不过，如果CounterTemplate作为其他类的基类，就不占用空间了
    static size_t m_count;
};

template<typename T> size_t CounterTemplate<T>::m_count = 0;    //定义式

class WidgetExample1 : public CounterTemplate<WidgetExample1>   //公有继承的方式
{
    //使用继承的方式来计算类对象的个数，必然要求CounterTemplate有一个虚析构函数。如
    //果没有虚析构函数，那么使用CounterTemplate* pointer删除一个派生类对象时，就会
    //导致没有调用派生类的析构函数。虽然可以将new和delete在CounterTemplate声明为private，
    //但这终究不方便，而且虚析构函数还会导致CounterTemplate的对象体积变大。
    //因此，不推荐使用public继承方式。

    //member
};

class WidgetExample2                                            //作为数据成员的方式
{
public:
    static size_t howMany() { return CounterTemplate<WidgetExample2>::howMany(); }

private:

    //为了计数，导致MyClass的体积变大
    //因此，也不推荐使用复合方式。
    CounterTemplate<WidgetExample2> c;
};

class WidgetExample3 : private CounterTemplate<WidgetExample3>  //私有继承的方式
{
    //前提：如果使用的编译器有空白基础类型最佳化的技术，那么就有：
    //采用私有继承，CounterTemplate的对象大小为零，而且析构函数不需要为virtual。
    //因此，最好使用private继承方式。

public:

    //如果WidgetExample3没有其他基类，那么可以直接通过using关键
    //字使用CounterTemplate类的howMany函数
    using CounterTemplate<WidgetExample3>::howMany;

    //static size_t howMany() { return CounterTemplate<WidgetExample3>::howMany(); }

    //other member
};

class SpecialWidgetExample : public WidgetExample3, private CounterTemplate<SpecialWidgetExample>
{
public:

    //SpecialWidgetExample既继承了WidgetExample3，也继承了CounterTemplate，为了获得它
    //的个数，可以通过如下静态函数实现。必须注意的是，如果程序中已经存在WidgetExample3
    //五个对象和三个SpecialWidgetExample对象，那么使用WidgetExample3
    //的howMany函数，返回值将会是8。
    static size_t howMany() { return CounterTemplate<SpecialWidgetExample>::howMany(); }

    //最后一点，继承顺序如上面那样声明，那么在CounterTemplate<SpecialWidgetExample>处会
    //出现错误，但可以编译通过。解决方法是将继承顺序对调。
};



//////////////////////////////////////////////////////////////////////////////
//要点：判断一个对象是否在堆中。
//////////////////////////////////////////////////////////////////////////////
class HeapTracked               //混合类，跟踪从operator new返回的ptr
{
public:
    virtual ~HeapTracked() = 0;

    static void* operator new (size_t size);
    static void operator delete(void* ptr);

    bool isOnHeap() const;

private:
    typedef const void* RawAddress;
    static std::list<RawAddress> m_addressesList;
};

//定义静态对象：
std::list<HeapTracked::RawAddress> HeapTracked::m_addressesList;

//HeapTracked的析构函数是纯虚函数，使得该类变为抽象类，但仍然需要一个定义
HeapTracked::~HeapTracked() { }

//调用全局的operator new函数
void* HeapTracked::operator new (size_t size)
{
    void* memPtr = ::operator new(size);
    HeapTracked::m_addressesList.push_front(memPtr);
    return memPtr;
}

//调用全局的operator delete函数
void HeapTracked::operator delete(void* ptr)
{
    std::list<HeapTracked::RawAddress>::iterator it =
        std::find(HeapTracked::m_addressesList.begin(),
            HeapTracked::m_addressesList.end(), ptr);
    if (it != HeapTracked::m_addressesList.end())
    {
        HeapTracked::m_addressesList.erase(it);
        ::operator delete(ptr);
    }
    else
    {
        //throw std::exception();
    }
}

bool HeapTracked::isOnHeap() const
{
    //得到一个指针，指向*this占据的内存空间的起始处。
    //因为isOnHeap仅仅用于HeapTracked对象中，所以能使用dynamic_cast操作符
    //把一个指针dynamic_cast成void*类型（或const void*或volatile void*…），
    //生成的指针将指向“原指针指向对象内存”的开始处
    const void* rawAddress = dynamic_cast<const void*>(this);
    std::list<RawAddress>::iterator it =
        std::find(m_addressesList.begin(), m_addressesList.end(), rawAddress);
    return it != m_addressesList.end();
}



//////////////////////////////////////////////////////////////////////////////
//要点：使用引用计数的String类
//示例：通过refCount来计数。
//////////////////////////////////////////////////////////////////////////////
class ReferenceCountString
{
public:
    ReferenceCountString(const char* initValue = "");
    ReferenceCountString(const ReferenceCountString& rhs);
    ReferenceCountString& operator=(const ReferenceCountString& rhs);
    ~ReferenceCountString();

    const char& operator[](int index) const;
    char& operator[](int index);

private:
    struct StringValue
    {
        int refCount;
        char* data;
        StringValue(const char* initValue);
        ~StringValue();
    };
    StringValue* m_value;
};

ReferenceCountString::StringValue::StringValue(const char* initValue)
{
    refCount = 1;
    int len = static_cast<int>(strlen(initValue));
    data = new char[len + 1];
    strcpy(data, initValue);
}

ReferenceCountString::StringValue::~StringValue()
{
    delete[] data;
}

ReferenceCountString::ReferenceCountString(const char* initValue) :
    m_value(new StringValue(initValue)) { }

ReferenceCountString::ReferenceCountString(const ReferenceCountString& rhs) : m_value(rhs.m_value)
{
    ++m_value->refCount;
}

ReferenceCountString& ReferenceCountString::operator=(const ReferenceCountString& rhs)
{
    if (m_value == rhs.m_value)
        return *this;

    if (0 == --m_value->refCount)
        delete m_value;

    m_value = rhs.m_value;
    ++m_value->refCount;
    return *this;
}

ReferenceCountString::~ReferenceCountString()
{
    if (0 == --m_value->refCount)
        delete m_value;
}

const char& ReferenceCountString::operator[](int index) const
{
    return m_value->data[index];
}

char& ReferenceCountString::operator[](int index)
{
    //写时拷贝
    if (m_value->refCount > 1)
    {
        --m_value->refCount;
        m_value = new StringValue(m_value->data);
    }
    return m_value->data[index];
}



//////////////////////////////////////////////////////////////////////////////
//要点：带引用计数的基类。
//////////////////////////////////////////////////////////////////////////////
template<class T>
class RCPtr
{
public:
    RCPtr(T* realPtr = 0) : pointee(realPtr) { init(); }
    RCPtr(const RCPtr& rhs) : pointee(rhs.pointee) { init(); }
    ~RCPtr() { if (pointee) pointee->removeReference(); }

    RCPtr& operator=(const RCPtr& rhs)
    {
        if (pointee != rhs.pointee)
        {
            if (pointee) pointee->removeReference();
            pointee = rhs.pointee;
            init();
        }
        return *this;
    }

    T* operator->() const { return pointee; }
    T& operator*() const { return *pointee; }

private:
    T* pointee;
    void init()
    {
        if (pointee == 0) return;
        if (pointee->isShareable() == false)
            pointee = new T(*pointee);
        pointee->addReference();
    }
};

class RCObject
{
public:
    void addReference() { ++refCount; }
    void removeReference() { if (--refCount == 0) delete this; }
    void markUnshareable() { shareable = false; }
    bool isShareable() const { return shareable; }
    bool isShared() const { return refCount > 1; }
protected:
    RCObject() : refCount(0), shareable(true) { }
    RCObject(const RCObject& rhs) : refCount(0), shareable(true) { }
    RCObject& operator=(const RCObject& rhs) { return *this; }
    virtual ~RCObject() = 0 { }
private:
    int refCount;
    bool shareable;
};

class EnhanceReferenceCountString
{
public:
    EnhanceReferenceCountString(const char* initValue = "") :
        value(new StringValue(initValue)) { }

    const char& operator[](int index) const
    { return value->data[index]; }

    char& operator[](int index)
    {
        if (value->isShareable())
        {
            value = new StringValue(value->data);
        }
        value->markUnshareable();
        return value->data[index];
    }

private:
    struct StringValue : public RCObject
    {
        char* data;
        StringValue(const char* initValue) { init(initValue); }
        StringValue(const StringValue& rhs) { init(rhs.data); }
        void init(const char* initValue)
        {
            data = new char[strlen(initValue) + 1];
            strcpy(data, initValue);
        }
        ~StringValue() { delete[] data; }
    };
    RCPtr<StringValue> value;
};
