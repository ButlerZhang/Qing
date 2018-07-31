// PrimerQing.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <cctype>
#include <vector>
#include <iterator>
#include <array>
#include <map>
#include <stack>
#include <queue>
#include <algorithm>
#include <numeric>
#include <list>
#include <iterator>
#include <functional>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <type_traits>


template <typename T> class Blob
{
public:
    typedef T value_type;
    typedef typename std::vector<T>::size_type size_type;

    Blob();
    Blob(std::initializer_list<T> il);
    size_type size() const { return data->size(); }
    bool empty() const { return data->empty(); }
    void push_back(const T &t) { data->push_back(t); }
    void push_back(const T &&t) { data->push_back(std::move(t)); }
    void pop_back();

    T& back();
    T& operator[](size_type i);

private:
    std::shared_ptr<std::vector<T>> data;
    void check(size_type i, const std::string &msg) const;
};

template<typename T>
void Blob<T>::check(size_type i, const std::string &msg) const
{
    if (i >= data->size()) throw std::out_of_range(msg);
}

template<typename T>
Blob<T>::Blob() :data(std::make_shared<std::vector<T>>()) {}

template<typename T>
Blob<T>::Blob(std::initializer_list<T> il) : data(std::make_shared<std::vector<T>>(il)) {}

template<typename T>
T& Blob<T>::back()
{
    check(0, "back on empty Blob");
    return data->back();
}

template<typename T>
T& Blob<T>::operator[](size_type i)
{
    check(i, "subscript out of range");
    return (*data)[i];
}

template<typename T>
void Blob<T>::pop_back()
{
    check(0, "pop_back on empty Blob");
    data->pop_back();
}


template<typename It>
auto fcn(It beg, It end) -> decltype(*beg)
{
    //TODO
    return *beg;
}

template<typename It>
auto fcn2(It beg, It end) -> typename std::remove_reference<decltype(*beg)>::type
{
    //TODO
    return *beg;
}






int main()
{

    system("pause");
    return 0;
}










