#pragma once
#include <boost/format.hpp>



template<class TFirst>
void BoostFormat(boost::format& fmt, TFirst&& first)
{
    fmt % first;
}

template<class TFirst, class... TOther>
void BoostFormat(boost::format& fmt, TFirst&& first, TOther&&... other)
{
    fmt % first;
    BoostFormat(fmt, other...);
}

template<class TFirst, class... TOther>
std::string BoostFormat(const char* format, TFirst&& first, TOther&&... other)
{
    boost::format fmt(format);
    BoostFormat(fmt, first, other...);
    return fmt.str();
}
