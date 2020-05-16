#include "FaceEmbedder.hpp"

#include "Utils/DataUtils.hpp"

#include <dlib/opencv/to_open_cv.h>

namespace Fls
{
    void FaceEmbedder::init(const std::string& embedderPath)
    {
        mInitializedFuture = std::async(std::launch::async,
            [this, embedderPath]()
        {
            mEmbeddingNetwork = cv::dnn::readNetFromTorch(embedderPath);

            mInitialized.store(true);
        });
    }

    std::vector<std::vector<float>> FaceEmbedder::embed(const UserResource* userResource,
        const std::vector<std::shared_ptr<FaceAlignmentResult>>& faces)
    {
        std::vector<std::vector<float>> result;

        if (!mInitialized.load() || !userResource || faces.empty())
            return result;

        if (mLastFrameId != userResource->id)
        {
            for(const auto& face : faces)
            {
                auto embeddings = embed(dlib::toMat(face->alignedFace));
                result.emplace_back(embeddings);
            }

            mLastFrameId = userResource->id;
            mLastFrameEmbeddingResult = result;

            return result;
        }

        return mLastFrameEmbeddingResult;
    }

    std::vector<float> FaceEmbedder::embed(const cv::Mat& alignedFace)
    {
        const auto faceBlob = cv::dnn::blobFromImage(alignedFace, 1.0f / 255,
            cv::Size(96, 96), cv::Scalar(), true, false);

        mEmbeddingNetwork.setInput(faceBlob);
        const auto embeddings = mEmbeddingNetwork.forward();

        std::vector<float> embeddingsVec;
        DataUtils::matToVector(embeddings, embeddingsVec);

        return embeddingsVec;
    }
}
