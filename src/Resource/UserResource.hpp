#pragma once

#include <Xenon/Graphics/API/Texture2D.hpp>

namespace Fls
{
    struct  UserResource
    {
        std::string id;
        std::string name;
        std::string path;

        std::shared_ptr<Xenon::Texture2D> frame;
        std::shared_ptr<Xenon::Texture2D> thumbnail;

        static constexpr uint32_t THUMBNAIL_WIDTH = 256;
        static constexpr uint32_t THUMBNAIL_HEIGHT = 256;
    };
}