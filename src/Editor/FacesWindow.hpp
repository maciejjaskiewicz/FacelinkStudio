#pragma once

#include "Detectors/FaceAlignmentResult.hpp"
#include "Detectors/FaceClassificationResult.hpp"
#include "Persistence/Repository/FaceModel.hpp"

#include <Xenon/Graphics.hpp>

namespace Fls
{
    class FacesWindow
    {
    public:
        explicit FacesWindow();

        void setFaces(int64 resourceId, const std::vector<std::shared_ptr<FaceAlignmentResult>>& faces, 
            const std::vector<std::vector<float>>& embeddings, const std::vector<FaceClassificationResult>& predictions);

        void updateGui(const Xenon::DeltaTime& deltaTime);
    private:
        int64 mCurrentResourceId{};
        std::vector<std::shared_ptr<FaceAlignmentResult>> mFaces;
        std::vector<std::shared_ptr<FaceModel>> mFaceModels;
        std::vector<FaceClassificationResult> mPredictions;
    };
}
