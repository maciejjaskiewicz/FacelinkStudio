#pragma once

#include "FaceClassificationResult.hpp"
#include "Persistence/FlsDatabase.hpp"
#include "Resource/UserResource.hpp"

#include <memory>
#include <opencv2/core/hal/interface.h>
#include <dlib/svm/kernel.h>
#include <dlib/svm/function.h>

namespace Fls
{
    struct UserResource;

    class FaceClassifier
    {
    public:
        void init(const std::shared_ptr<FlsDatabase>& database);

        std::vector<FaceClassificationResult> classify(const UserResource* userResource, 
            const std::vector<std::vector<float>>& embeddedFaces);
        int classify(const std::vector<float>& faceEmbeddings) const;
    private:
        typedef dlib::matrix<double, 0, 1> Sample;
        typedef dlib::linear_kernel<Sample> LinearKernel;
        typedef dlib::multiclass_linear_decision_function<LinearKernel, int> DecisionFunction;
        typedef dlib::normalized_function<DecisionFunction> Classifier;

        std::vector<FaceClassificationResult> namePredictions(const std::vector<int>& predictions) const;

        std::atomic_bool mInitialized{ false };
        std::future<void> mInitializedFuture;

        std::shared_ptr<FlsDatabase> mDatabase;
        Classifier mClassifier;
        float mConfidenceThreshold{ 0.4f };

        int64 mLastFrameId{ 0 };
        std::vector<FaceClassificationResult> mLastFrameClassificationResult;
    };
}
