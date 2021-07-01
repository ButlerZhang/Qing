#pragma once
#include <map>
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

//²ÍÌü²Ëµ¥µü´úÆ÷
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

//¼å±ýÎÝµü´úÆ÷
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

//¿§·Èµü´úÆ÷
class CafeIterator : public Iterator {
public:

    CafeIterator(std::map<int, MenuItem> &items) : m_items(items) {
        m_position = 0;
    }

    virtual bool hasNext() {
        return m_position < m_items.size();
    }

    virtual MenuItem *next() {
        std::map<int, MenuItem>::iterator it = m_items.find(m_position++);
        return it != m_items.end() ? &(it->second) : nullptr;
    }

private:

    std::map<int, MenuItem> &m_items;
    int m_position;
};
