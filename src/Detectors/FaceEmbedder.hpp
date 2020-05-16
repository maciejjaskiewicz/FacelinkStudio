#pragma once

#include "Resource/UserResource.hpp"
#include "FaceAlignmentResult.hpp"

#include <string>
#include <vector>
#include <future>

#include <opencv2/dnn.hpp>

namespace Fls
{
    class FaceEmbedder
    {
    public:
        void init(const std::string& embedderPath);

        std::vector<std::vector<float>> embed(const UserResource* userResource, 
            const std::vector<std::shared_ptr<FaceAlignmentResult>>& faces);
        std::vector<float> embed(const cv::Mat& alignedFace);
    private:
        std::atomic_bool mInitialized{ false };
        std::future<void> mInitializedFuture;

        cv::dnn::Net mEmbeddingNetwork;

        int64 mLastFrameId{ 0 };
        std::vector<std::vector<float>> mLastFrameEmbeddingResult;
    };
}
