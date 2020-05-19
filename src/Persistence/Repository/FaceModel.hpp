#pragma once

#include "EntityModel.hpp"

#include <Xenon/Graphics/API/Texture2D.hpp>

#include <vector>
#include <opencv2/core/mat.hpp>
#include <dlib/pixel.h>
#include <dlib/array2d.h>

namespace Fls
{
    struct FaceModel : EntityModel
    {
        int personId{ -1 };
        std::vector<float> embeddings;
        cv::Mat image;

        // extra
        std::shared_ptr<Xenon::Texture2D> texture();
        void setTexture(const std::shared_ptr<Xenon::Texture2D>& texture);
    private:
        std::shared_ptr<Xenon::Texture2D> mTexture;
    };
}
