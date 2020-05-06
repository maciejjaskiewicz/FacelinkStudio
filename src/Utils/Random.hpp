#pragma once

#include <random>

namespace Fls
{
    class Random
    {
    public:
        static int generateInt(int min, int max);

        static std::string generateHex(uint32_t length);
    };
}
