#pragma once

#include <Xenon/Graphics.hpp>
#include "Detectors/FaceAlignmentResult.hpp"

namespace Fls
{
    class FacesWindow
    {
    public:
        explicit FacesWindow();

        void setFaces(int64 resourceId, const std::vector<std::shared_ptr<FaceAlignmentResult>>& faces);

        void updateGui(const Xenon::DeltaTime& deltaTime);
    private:
        int64 mCurrentResourceId{};
        std::vector<std::shared_ptr<FaceAlignmentResult>> mFaces;
        std::vector<std::shared_ptr<Xenon::Texture2D>> mFaceTextures;
    };
}
