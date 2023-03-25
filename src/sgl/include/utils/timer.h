#pragma once
#include "macro.h"
#include <chrono>

SGL_BEG
class timer
{
    std::chrono::high_resolution_clock::time_point beg;
    double tm;

public:
    inline timer() : beg{}, tm{0} {}

    inline void start()
    {
        beg = std::chrono::high_resolution_clock::now();
    }

    // if you have not called start before this, you will get an undefined result
    inline void stop()
    {
        std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - beg;
        tm = diff.count();
    }

    // if you have not called start before this, you will get an undefined result
    inline double lap() const
    {
        std::chrono::duration<double> diff = std::chrono::high_resolution_clock::now() - beg;
        return diff.count();
    }

    inline double seconds() const
    {
        return tm;
    }
};

class countdown
{
    timer t;
    double tm;

public:
    inline countdown() : t{}, tm{} {}
    inline countdown(double seconds) : t{}, tm{seconds} {};

    inline void set_time(double seconds)
    {
        tm = seconds;
    }

    void start()
    {
        t.start();
    }

    bool finished() const
    {
        return t.lap() >= tm;
    }
};
SGL_END