#pragma once

#include "Xenon/Core/Api.hpp"

namespace Fls
{
    class Math
    {
    public:
        XN_NODISCARD static float interpolate(float x, float inMin, float inMax, 
            float outMin, float outMax);
    };
}
