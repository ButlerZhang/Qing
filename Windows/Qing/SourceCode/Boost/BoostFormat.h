#pragma once
#include "..\QingBase.h"
#include <boost\format.hpp>

QING_NAMESPACE_BEGIN



template<class TFirst>
void QING_DLL BoostFormat(boost::wformat& fmt, TFirst&& first)
{
    fmt % first;
}

template<class TFirst, class... TOther>
void QING_DLL BoostFormat(boost::wformat& fmt, TFirst&& first, TOther&&... other)
{
    fmt % first;
    BoostFormat(fmt, other...);
}

template<class TFirst, class... TOther>
std::wstring QING_DLL BoostFormat(const wchar_t* format, TFirst&& first, TOther&&... other)
{
    boost::wformat fmt(format);
    BoostFormat(fmt, first, other...);
    return fmt.str();
}

QING_NAMESPACE_END