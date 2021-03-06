#include "FaceDetector.hpp"

#include "Resource/UserResource.hpp"
#include "Events/FaceDetectionSettingChangedEvent.hpp"
#include "Xenon/Core/ApplicationServices.hpp"

namespace Fls
{
    void FaceDetector::init(const std::string& netCfgPath, const std::string& netWeightsPath)
    {
        mInitializedFuture = std::async(std::launch::async, 
            [this, netCfgPath, netWeightsPath]()
        {
            mDetectionNetwork = cv::dnn::readNetFromCaffe(netCfgPath, netWeightsPath);

            mDetectionNetwork.setPreferableBackend(cv::dnn::DNN_BACKEND_CUDA);
            mDetectionNetwork.setPreferableTarget(cv::dnn::DNN_TARGET_CUDA);

            mInitialized.store(true);
        });

        Xenon::ApplicationServices::EventBus::ref().subscribe<FaceDetectionSettingChangedEvent>(
        [this](const FaceDetectionSettingChangedEvent& event)
        {
            if(mConfidenceThreshold != event.confidenceThreshold)
            {
                mConfidenceThreshold = event.confidenceThreshold;
                mSettingsChanged = true;
            }
        });
    }

    std::vector<FaceDetectionResult> FaceDetector::detect(const UserResource* userResource)
    {
        std::vector<FaceDetectionResult> result;

        if(!mInitialized.load() || !userResource || userResource->detectionFrame.empty())
            return result;

        if(mLastFrameId != userResource->id || mSettingsChanged)
        {
            const auto detectionBlob = preprocess(userResource->detectionFrame);
            mDetectionNetwork.setInput(detectionBlob);

            auto detection = mDetectionNetwork.forward();
            cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

            for (auto i = 0; i < detectionMat.rows; i++)
            {
                const auto confidence = detectionMat.at<float>(i, 2);

                const auto frameWidth = userResource->frame->width();
                const auto frameHeight = userResource->frame->height();

                if (confidence > mConfidenceThreshold)
                {
                    FaceDetectionResult detectionResult
                    {
                        confidence,
                        static_cast<int>(detectionMat.at<float>(i, 3) * frameWidth),
                        static_cast<int>(detectionMat.at<float>(i, 4) * frameHeight),
                        static_cast<int>(detectionMat.at<float>(i, 5) * frameWidth),
                        static_cast<int>(detectionMat.at<float>(i, 6) * frameHeight)
                    };

                    result.push_back(detectionResult);
                }
            }

            mLastFrameId = userResource->id;
            mLastFrameDetectionResult = result;
            mSettingsChanged = false;

            return result;
        }

        return mLastFrameDetectionResult;
    }

    cv::Mat FaceDetector::preprocess(const cv::Mat& pixels) const
    {
        return cv::dnn::blobFromImage(pixels, 1.0,
            cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));
    }
}
