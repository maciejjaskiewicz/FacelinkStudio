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
        bool mSettingsChanged{ false };

        cv::dnn::Net mDetectionNetwork;
        float mConfidenceThreshold{ 0.5f };

        int64 mLastFrameId{ 0 };
        std::vector<FaceDetectionResult> mLastFrameDetectionResult;
    };
}
