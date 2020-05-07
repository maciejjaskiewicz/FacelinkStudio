#pragma once

namespace Fls
{
    struct FaceDetectionResult
    {
        float confidence{ 0.0f };

        int x1{ 0 }, y1{ 0 };
        int x2{ 0 }, y2{ 0 };
    };
}