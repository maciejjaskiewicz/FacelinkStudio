#pragma once

#include "Xenon/Core/Api.hpp"

#include <random>

namespace Fls
{
    class Random
    {
    public:
        XN_NODISCARD static int generateInt(int min, int max);

        XN_NODISCARD static std::string generateHex(uint32_t length);
    };
}
