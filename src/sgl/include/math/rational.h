#pragma once
#include <cstdint>
#include "numerics.h"

class rational
{
public:
    constexpr rational() : m_N{0}, m_D{1} {}
    constexpr rational(long double d) : rational()
    {
        constexpr static int digits = std::numeric_limits<long double>::digits10;
    }
private:
    std::intmax_t m_N;
    std::intmax_t m_D;
};