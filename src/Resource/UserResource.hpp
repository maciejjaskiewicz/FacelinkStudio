#pragma once

#include <Xenon/Graphics/API/Texture2D.hpp>
#include <opencv2/core/mat.hpp>
#include <opencv2/cudaimgproc.hpp>

namespace Fls
{
    struct UserResource
    {
        int64 id;
        std::string name;
        std::string path;

        cv::Mat pixels;
        cv::cuda::GpuMat gpuPixels;

        std::shared_ptr<Xenon::Texture2D> frame;
        std::shared_ptr<Xenon::Texture2D> thumbnail;

        cv::Mat detectionFrame;

        bool preprocessed{ false };

        UserResource()
            : id(0), gpuPixels(cv::cuda::GpuMat())
        { }

        static constexpr uint32_t THUMBNAIL_WIDTH = 256;
        static constexpr uint32_t THUMBNAIL_HEIGHT = 256;
    };
}
