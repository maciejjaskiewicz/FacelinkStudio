#include "FacesWindow.hpp"

#include "Utils/DataUtils.hpp"

#include <Xenon/Core/ApplicationServices.hpp>
#include <imgui.h>
#include <dlib/opencv/to_open_cv.h>
#include "Events/FaceSelectedEvent.hpp"

namespace Fls
{
    FacesWindow::FacesWindow() = default;

    void FacesWindow::setFaces(const int64 resourceId, const std::vector<std::shared_ptr<FaceAlignmentResult>>& faces, 
        const std::vector<std::vector<float>>& embeddings, const std::vector<FaceClassificationResult>& predictions)
    {
        if(mCurrentResourceId != resourceId || mFaces.size() != faces.size())
        {
            mFaces = faces;
            mPredictions = predictions;

            mFaceModels.clear();

            for (std::size_t i = 0; i < faces.size(); i++)
            {
                auto faceModel = std::make_shared<FaceModel>();
                faceModel->embeddings = embeddings[i];
                dlib::toMat(faces[i]->alignedFace).copyTo(faceModel->image);

                mFaceModels.emplace_back(faceModel);
            }

            mCurrentResourceId = resourceId;
        }
    }

    void FacesWindow::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        ImGui::Begin("Faces");

        const auto contentWidth = ImGui::GetContentRegionAvailWidth();
        const auto contentHeight = ImGui::GetContentRegionAvail().y;

        ImGui::BeginChild("##ContentRegion", { contentWidth, contentHeight }, true);

        const auto itemSize = 128.0f;

        auto columns = static_cast<int>(contentWidth / itemSize);
        columns = columns < 1 ? 1 : columns;
        ImGui::Columns(columns, nullptr, false);

        uint32_t idx{ 0 };

        for(const auto& face : mFaceModels)
        {
            ImGui::PushID(idx);

            auto* texturePtr = reinterpret_cast<void*>(static_cast<intptr_t>(face->texture()->id()));

            ImGui::BeginGroup();

            ImGui::ImageButton(texturePtr, { itemSize - 20, itemSize - 16 });
            
            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
            {
                Xenon::ApplicationServices::EventBus::ref().trigger<FaceSelectedEvent>(face);
            }

            ImGui::TextWrapped(mPredictions[idx].name.c_str());

            ImGui::EndGroup();

            ImGui::PopID();
            ImGui::NextColumn();
            idx++;
        }

        ImGui::EndChild();

        ImGui::End();
    }
}
