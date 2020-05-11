#pragma once

#include <opencv2/core/mat.hpp>
#include <glm/vec2.hpp>
#include <dlib/pixel.h>
#include <dlib/array2d.h>

namespace Fls
{
    struct FaceAlignmentResult
    {
        dlib::array2d<dlib::bgr_pixel> alignedFace;

        std::vector<glm::vec2> landmarks;
        glm::vec2 srcFaceSize{};
    };
}
