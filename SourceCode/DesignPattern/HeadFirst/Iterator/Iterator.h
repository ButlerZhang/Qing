#pragma once
#include <list>
#include <vector>
#include "MenuItem.h"

class Iterator {
public:

    virtual bool hasNext() = 0;
    virtual MenuItem *next() = 0;
};

class Menu {
public:

    virtual Iterator *createIterator() = 0;
};

class DinerMenuIterator : public Iterator {
public:

    DinerMenuIterator(std::vector<MenuItem> &items) : m_items(items) {
        m_position = 0;
    }

    virtual bool hasNext() {
        return m_position < m_items.size();
    }

    virtual MenuItem *next() {
        MenuItem *result = &m_items[m_position];
        m_position += 1;
        return result;
    }

private:

    std::vector<MenuItem> &m_items;
    int m_position;
};

class PancakeHouseIterator : public Iterator {
public:

    PancakeHouseIterator(std::list<MenuItem> &items) : m_items(items) {
        m_position = 0;
    }

    virtual bool hasNext() {
        return m_position < m_items.size();
    }

    virtual MenuItem *next() {
        int currentCount = -1;
        for (std::list<MenuItem>::iterator it = m_items.begin(); it != m_items.end(); it++) {
            if (++currentCount == m_position) {
                m_position += 1;
                return &(*it);
            }
        }

        return nullptr;
    }

private:

    std::list<MenuItem> &m_items;
    int m_position;
};
