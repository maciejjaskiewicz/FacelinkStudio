#pragma once

#include "Xenon/Core/Api.hpp"
#include "Xenon/Graphics/API/Texture2D.hpp"

namespace cv {
    class Mat;
}

namespace Fls
{
    class DataUtils
    {
    public:
        static void matToVector(const cv::Mat& mat, std::vector<float>& out);
        XN_NODISCARD static std::string vectorToString(const std::vector<int>& arr, const std::string& separator);
        XN_NODISCARD static std::shared_ptr<Xenon::Texture2D> imgToTexture(const cv::Mat& mat);
    };
}
