#pragma once

#include <string>

namespace Fls
{
    struct FaceClassificationResult
    {
        int personId{ -1 };
        std::string name{ "Unknown" };
    };
}
