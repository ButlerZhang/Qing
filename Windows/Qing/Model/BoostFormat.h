#pragma once
#include "..\HeaderFiles\QingBase.h"
#include <boost\format.hpp>

QING_NAMESPACE_BEGIN



template<class TFirst>
void QING_DLL BoostFormat(boost::format& fmt, TFirst&& first)
{
    fmt % first;
}

template<class TFirst, class... TOther>
void QING_DLL BoostFormat(boost::format& fmt, TFirst&& first, TOther&&... other)
{
    fmt % first;
    BoostFormat(fmt, other...);
}

template<class TFirst, class... TOther>
std::string QING_DLL BoostFormat(const char* format, TFirst&& first, TOther&&... other)
{
    boost::format fmt(format);
    BoostFormat(fmt, first, other...);
    return fmt.str();
}

QING_NAMESPACE_END