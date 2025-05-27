#pragma once

#ifdef STATKILIB_EXPORTS
#define STATKILIB_API __declspec(dllexport)
#else
#define STATKILIB_API __declspec(dllimport)
#endif

#include <vector>
#include <set>
#include <string>
#include <stdexcept>

namespace Statki {

    enum class Pole : char {
        Puste = ' ',
        Statek = 'S',
        Trafiony = 'X',
        Pudlo = 'O'
    };

    enum class Orientacja {
        Pozioma,
        Pionowa
    };

    struct StatekInfo {
        int x;
        int y;
        int dlugosc;
        Orientacja orientacja;
    };

}