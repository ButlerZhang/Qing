#pragma once
#include <new>
#include <list>
#include <string>

#pragma warning( disable : 4290 )
#pragma warning( disable : 4996 )



//////////////////////////////////////////////////////////////////////////////
//Ҫ�㣺����ʹ��const��
//ʾ������const��non-const��Ա��������ʵ�ʵȼ۵�ʵ��ʱ����non-const�汾����const
//      �汾���Ա�������ظ���
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
        return const_cast<char&>(static_cast<const TextBlock&>(*this)[position]);
    }

private:
    std::string text;
};



//////////////////////////////////////////////////////////////////////////////
//Ҫ�㣺������ʹ�ñ������Զ����ɵĺ����͸���ȷ�ܾ���
//ʾ��������һ�����࣬�����������͸�ֵ������������Ϊprivate�����һ������ʹ��
//      member������friend���������޷����������ˡ�����ʹ��boost::noncopyable��
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
//Ҫ�㣺д��placement newҲҪдplacement delete��
//ʾ��������һ�����࣬�ں�������ȷ��ʽ��new��delete��
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

class StandardNewDeleteFormsExample : public StandardNewDeleteForms    //�̳б�׼��ʽ
{
public:

    //����Щ��ʽ�ɼ�
    using StandardNewDeleteForms::operator new;
    using StandardNewDeleteForms::operator delete;

    //���һ���Զ����placement new
    static void* operator new(std::size_t size, std::ostream& logStream) throw(std::bad_alloc)
    {
        //TODO
    }

    //���һ���Զ����placement delete
    static void operator delete(void* pMemory, std::ostream& logStream) throw()
    {
        //TODO
    }
};



//////////////////////////////////////////////////////////////////////////////
//Ҫ�㣺����ģʽ�ļ�ʵ��
//ʾ������ֹ����ĳ����Ķ��������׵ķ������ǰѸ���Ĺ��캯�����������private��
//     ���ϣ��ֻӵ��һ��ʵ���������������������һ����Ԫ��������������������һ����̬����
//////////////////////////////////////////////////////////////////////////////
class SingleClassExample
{
private:
    SingleClassExample();

public:
    //TODO

    friend SingleClassExample& Instance()     //�൱�ڶ�����ȫ�ֺ���
    {
        //����Ϊ�����ڵľ�̬�����������ô���
        //1.���Ա�����̬��Աһ����������ͷţ����²�ʹ�øö���
        //2.��̬��Ա�ĳ�ʼ��ʱ�䲻ȷ���������ھ�̬����ֻ���ڵ�һ��ִ�к���ʱ���Żᱻ���졣
        static SingleClassExample instance;
        return instance;
    }

    //static SingleClassExample& Instance();   //Ҳ����ʹ�þ�̬����
};



//////////////////////////////////////////////////////////////////////////////
//Ҫ�㣺���ж���������ܵĻ��ࡣ
//ʾ�������徲̬��Ա�����ٶ����������һ�������������׳��쳣��
//////////////////////////////////////////////////////////////////////////////
template< typename BeingCounted >
class CountedStandard
{
public:
    class TooManyObjects { };       //�����׳��쳣
    static int objectCount() { return m_numObjects; }

protected:
    CountedStandard();
    CountedStandard(const CountedStandard& rhs);
    ~CountedStandard() { --m_numObjects; }

private:
    static int              m_numObjects;
    static const size_t     m_maxObjects;

    void init();                    //���⹹�캯���Ĵ����ظ�
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

class PrinterCountedExample : private CountedStandard<PrinterCountedExample>    //private�̳�
{
public:
    ~PrinterCountedExample() {}

    //��������Ļ�����α���캯��
    //��������������������Printer����Ҫ���Բ���Printer��
    //������������ͨ�����ù��캯���Ϳ������캯�������¶���
    static PrinterCountedExample* makePrinter() { return new PrinterCountedExample(); }
    static PrinterCountedExample* makePrinter(const PrinterCountedExample& lhs) { return new PrinterCountedExample(lhs); }

    //TODO
    //void reset();
    //void submitJob(const PrintJob& Job);

    //ʹ��using����ʹ�����Ǿ���public����
    using CountedStandard<PrinterCountedExample>::objectCount;
    using CountedStandard<PrinterCountedExample>::TooManyObjects;

private:
    PrinterCountedExample();
    PrinterCountedExample(const PrinterCountedExample& rhs);
};

//��Counted��ʵ���ļ��ж���numObjects���Զ���ʼ��Ϊ0��
template<typename BeingCounted> int CountedStandard<BeingCounted>::m_numObjects;

//����maxObject�������������ʵ���ļ���
const size_t CountedStandard<PrinterCountedExample>::m_maxObjects = 10;



//////////////////////////////////////////////////////////////////////////////
//Ҫ�㣺�������������ĸ���
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

    //����m_count�Ǿ�̬��Ա����CounterTemplate������Ȼռ��������1byte�Ŀռ�
    //���������CounterTemplate��Ϊ������Ļ��࣬�Ͳ�ռ�ÿռ���
    static size_t m_count;
};

template<typename T> size_t CounterTemplate<T>::m_count = 0;    //����ʽ

class WidgetExample1 : public CounterTemplate<WidgetExample1>   //���м̳еķ�ʽ
{
    //ʹ�ü̳еķ�ʽ�����������ĸ�������ȻҪ��CounterTemplate��һ����������������
    //��û����������������ôʹ��CounterTemplate* pointerɾ��һ�����������ʱ���ͻ�
    //����û�е����������������������Ȼ���Խ�new��delete��CounterTemplate����Ϊprivate��
    //�����վ������㣬�����������������ᵼ��CounterTemplate�Ķ���������
    //��ˣ����Ƽ�ʹ��public�̳з�ʽ��

    //member
};

class WidgetExample2                                            //��Ϊ���ݳ�Ա�ķ�ʽ
{
public:
    static size_t howMany() { return CounterTemplate<WidgetExample2>::howMany(); }

private:

    //Ϊ�˼���������MyClass��������
    //��ˣ�Ҳ���Ƽ�ʹ�ø��Ϸ�ʽ��
    CounterTemplate<WidgetExample2> c;
};

class WidgetExample3 : private CounterTemplate<WidgetExample3>  //˽�м̳еķ�ʽ
{
    //ǰ�᣺���ʹ�õı������пհ׻���������ѻ��ļ�������ô���У�
    //����˽�м̳У�CounterTemplate�Ķ����СΪ�㣬����������������ҪΪvirtual��
    //��ˣ����ʹ��private�̳з�ʽ��

public:

    //���WidgetExample3û���������࣬��ô����ֱ��ͨ��using�ؼ�
    //��ʹ��CounterTemplate���howMany����
    using CounterTemplate<WidgetExample3>::howMany;

    //static size_t howMany() { return CounterTemplate<WidgetExample3>::howMany(); }

    //other member
};

class SpecialWidgetExample : public WidgetExample3, private CounterTemplate<SpecialWidgetExample>
{
public:

    //SpecialWidgetExample�ȼ̳���WidgetExample3��Ҳ�̳���CounterTemplate��Ϊ�˻����
    //�ĸ���������ͨ�����¾�̬����ʵ�֡�����ע����ǣ�����������Ѿ�����WidgetExample3
    //������������SpecialWidgetExample������ôʹ��WidgetExample3
    //��howMany����������ֵ������8��
    static size_t howMany() { return CounterTemplate<SpecialWidgetExample>::howMany(); }

    //���һ�㣬�̳�˳��������������������ô��CounterTemplate<SpecialWidgetExample>����
    //���ִ��󣬵����Ա���ͨ������������ǽ��̳�˳��Ե���
};



//////////////////////////////////////////////////////////////////////////////
//Ҫ�㣺�ж�һ�������Ƿ��ڶ��С�
//////////////////////////////////////////////////////////////////////////////
class HeapTracked               //����࣬���ٴ�operator new���ص�ptr
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

//���徲̬����
std::list<HeapTracked::RawAddress> HeapTracked::m_addressesList;

//HeapTracked�����������Ǵ��麯����ʹ�ø����Ϊ�����࣬����Ȼ��Ҫһ������
HeapTracked::~HeapTracked() { }

//����ȫ�ֵ�operator new����
void* HeapTracked::operator new (size_t size)
{
    void* memPtr = ::operator new(size);
    HeapTracked::m_addressesList.push_front(memPtr);
    return memPtr;
}

//����ȫ�ֵ�operator delete����
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
    //�õ�һ��ָ�룬ָ��*thisռ�ݵ��ڴ�ռ����ʼ����
    //��ΪisOnHeap��������HeapTracked�����У�������ʹ��dynamic_cast������
    //��һ��ָ��dynamic_cast��void*���ͣ���const void*��volatile void*������
    //���ɵ�ָ�뽫ָ��ԭָ��ָ������ڴ桱�Ŀ�ʼ��
    const void* rawAddress = dynamic_cast<const void*>(this);
    std::list<RawAddress>::iterator it =
        std::find(m_addressesList.begin(), m_addressesList.end(), rawAddress);
    return it != m_addressesList.end();
}



//////////////////////////////////////////////////////////////////////////////
//Ҫ�㣺ʹ�����ü�����String��
//ʾ����ͨ��refCount��������
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
    //дʱ����
    if (m_value->refCount > 1)
    {
        --m_value->refCount;
        m_value = new StringValue(m_value->data);
    }
    return m_value->data[index];
}



//////////////////////////////////////////////////////////////////////////////
//Ҫ�㣺�����ü����Ļ��ࡣ
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
