#include "Math.hpp"

namespace Fls
{
    float Math::interpolate(const float x, const float inMin, const float inMax, 
        const float outMin, const float outMax)
    {
        return (x - inMin) * (outMax - outMin) / (inMax - inMin) + outMin;
    }
}
