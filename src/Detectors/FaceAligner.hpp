#pragma once

#include "FaceDetectionResult.hpp"
#include "FaceAlignmentResult.hpp"

#include <dlib/image_processing.h>
#include <future>

namespace Fls
{
    struct UserResource;

    class FaceAligner
    {
    public:
        void init(const std::string& landmarksShapePredictorPath);

        XN_NODISCARD std::vector<std::shared_ptr<FaceAlignmentResult>> align(const UserResource* userResource,
            const std::vector<FaceDetectionResult>& detectedFaces);
    private:
        std::atomic_bool mInitialized{ false };
        std::future<void> mInitializedFuture;

        dlib::shape_predictor mShapePredictor;

        int64 mLastFrameId{ 0 };
        std::vector<std::shared_ptr<FaceAlignmentResult>> mLastFrameAlignmentResult;
    };
}