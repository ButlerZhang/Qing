#pragma once
#include "..\HeaderFiles\QingBase.h"
#include <Windows.h>
#include <boost\format.hpp>

QING_NAMESPACE_BEGIN



template<class TFirst>
void StringFormat(boost::format& fmt, TFirst&& first)
{
    fmt % first;
}

template<class TFirst, class... TOther>
void StringFormat(boost::format& fmt, TFirst&& first, TOther&&... other)
{
    fmt % first;
    StringFormat(fmt, other...);
}

template<class TFirst, class... TOther>
std::string StringFormat(const char* format, TFirst&& first, TOther&&... other)
{
    boost::format fmt(format);
    StringFormat(fmt, first, other...);
    return fmt.str();
}

QING_NAMESPACE_END