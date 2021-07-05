#pragma once
#include <stddef.h>
#include "ITACommon.h"

ITA_NS_BEGIN

template <typename T>
class VectorLite {
public:

    explicit VectorLite(int InitSize = 0) : m_Size(0), m_Capacity(2 * InitSize), m_ObjectsArray(nullptr) {
        if (m_Capacity > 0) {
            m_ObjectsArray = new T[m_Capacity];
        }
    }

    ~VectorLite() {
        delete[] m_ObjectsArray;
        m_ObjectsArray = NULL;
    }

    VectorLite(const VectorLite &Rhs) {
        operator=(Rhs);
    }

    const VectorLite &operator=(const VectorLite &Rhs) {
        if (this != &Rhs) {
            m_Size = Rhs.m_Size;
            m_Capacity = Rhs.m_Capacity;

            delete[] m_ObjectsArray;
            m_ObjectsArray = new T[m_ObjectsArray];

            for (int Index = 0; Index < m_Size; Index++) {
                m_ObjectsArray[Index] = Rhs.m_ObjectsArray[Index];
            }
        }

        return *this;
    }

    void resize(int NewSize) {
        if (NewSize > m_Capacity) {
            reserve(NewSize * 2);
        }

        m_Size = NewSize;
    }

    void reserve(int NewCapacity) {
        if (NewCapacity > m_Capacity) {
            T *OldArray = m_ObjectsArray;
            m_ObjectsArray = new T[NewCapacity];
            for (int Index = 0; Index < m_Size; Index++) {
                m_ObjectsArray[Index] = OldArray[Index];
            }

            delete[] OldArray;
            m_Capacity = NewCapacity;
        }
    }

    T &operator[](int index) {
        return m_ObjectsArray[index];
    }

    const T &operator[](int index) const {
        return m_ObjectsArray[index];
    }

    bool empty() const {
        return size() == 0;
    }

    int size() const {
        return m_Size;
    }

    int capacity() const {
        return m_Capacity;
    }

    void push_back(const T &x) {
        if (m_Size == m_Capacity) {
            reserve(m_Size * 2);
        }

        m_ObjectsArray[m_Size++] = x;
    }

    void pop_back() {
        --m_Size;
    }

    const T &back() const {
        return m_ObjectsArray[m_Size - 1];
    }

    typedef T *iterator;
    typedef const T *const_iterator;

    iterator begin() {
        return &m_ObjectsArray[0];
    }

    const_iterator begin() const {
        return &m_ObjectsArray[0];
    }

    iterator end() {
        return &m_ObjectsArray[size()];
    }

    const_iterator end() const {
        return &m_ObjectsArray[size()];
    }

private:

    int     m_Size;
    int     m_Capacity;
    T *m_ObjectsArray;
};

ITA_NS_END