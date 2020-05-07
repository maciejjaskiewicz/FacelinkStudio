#pragma once

#include "UserResource.hpp"

namespace cv {
    namespace ogl {
        class Buffer;
    }

    namespace cuda {
        class GpuMat;
    }

    class Mat;
}

namespace Fls
{
    class ResourcePreprocessor
    {
    public:
        static void preprocess(const cv::Mat& pixels, UserResource* userResource);
    private:
        static void preprocessThumbnail(cv::cuda::GpuMat& src, cv::cuda::GpuMat& dst);
        static void preprocessForDetection(const cv::Mat& pixels, UserResource* userResource);

        static void generateFrameTexture(const cv::ogl::Buffer& pixels, UserResource* userResource);
        static void generateThumbnailTexture(const cv::cuda::GpuMat& thumbnailPixels, UserResource* userResource);

        static Xenon::TextureFormat resolveFormat(uint32_t channels);
    };
}
