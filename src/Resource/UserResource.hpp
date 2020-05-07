#pragma once

#include <Xenon/Graphics/API/Texture2D.hpp>
#include <opencv2/core/mat.hpp>

namespace Fls
{
    struct UserResource
    {
        std::string id;
        std::string name;
        std::string path;

        std::shared_ptr<Xenon::Texture2D> frame;
        std::shared_ptr<Xenon::Texture2D> thumbnail;

        cv::Mat detectionFrame;

        static constexpr uint32_t THUMBNAIL_WIDTH = 256;
        static constexpr uint32_t THUMBNAIL_HEIGHT = 256;
    };
}
