#pragma once

#include "FaceDetectionResult.hpp"

#include <opencv2/dnn.hpp>

namespace Fls
{
    struct UserResource;

    

    class FaceDetector
    {
    public:
        void init(const std::string& netCfgPath, const std::string& netWeightsPath);
        cv::Mat preprocess(const cv::Mat& pixels) const;

        XN_NODISCARD std::vector<FaceDetectionResult> detect(const UserResource* userResource);
    private:
        cv::dnn::Net mDetectionNetwork;

        std::string mLastFrameId;
        std::vector<FaceDetectionResult> mLastFrameDetectionResult;
    };
}