#pragma once

#include "FaceDetectionResult.hpp"

#include <opencv2/dnn.hpp>
#include <future>

namespace Fls
{
    struct UserResource;

    class FaceDetector
    {
    public:
        void init(const std::string& netCfgPath, const std::string& netWeightsPath);
        XN_NODISCARD cv::Mat preprocess(const cv::Mat& pixels) const;

        XN_NODISCARD std::vector<FaceDetectionResult> detect(const UserResource* userResource);
    private:
        std::atomic_bool mInitialized{ false };
        std::future<void> mInitializedFuture;

        cv::dnn::Net mDetectionNetwork;

        int64 mLastFrameId;
        std::vector<FaceDetectionResult> mLastFrameDetectionResult;
    };
}
