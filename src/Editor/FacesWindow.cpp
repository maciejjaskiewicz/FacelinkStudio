#include "FacesWindow.hpp"

#include <imgui.h>
#include <opencv2/core/opengl.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>
#include <dlib/opencv/to_open_cv.h>

namespace Fls
{
    FacesWindow::FacesWindow() = default;

    void FacesWindow::setFaces(const int64 resourceId, const std::vector<std::shared_ptr<FaceAlignmentResult>>& faces, 
        const std::vector<FaceClassificationResult>& predictions)
    {
        if(mCurrentResourceId != resourceId || mFaces.size() != faces.size())
        {
            mFaces = faces;
            mPredictions = predictions;
            mFaceTextures.clear();

            for (const auto& face : faces)
            {
                cv::cuda::GpuMat dRgbFace;

                cv::cuda::GpuMat dFace(dlib::toMat(face->alignedFace));
                cv::cuda::cvtColor(dFace, dRgbFace, cv::COLOR_BGR2RGB);

                const cv::ogl::Buffer buffer(dRgbFace, cv::ogl::Buffer::PIXEL_UNPACK_BUFFER, true);
                buffer.bind(cv::ogl::Buffer::PIXEL_UNPACK_BUFFER);

                const Xenon::Texture2DConfiguration textureCfg(dFace.cols, dFace.rows,
                    Xenon::TextureFormat::RGB);
                const auto faceTexture = Xenon::Texture2D::create(nullptr, textureCfg, 1);

                cv::ogl::Buffer::unbind(cv::ogl::Buffer::PIXEL_UNPACK_BUFFER);

                mFaceTextures.emplace_back(faceTexture);
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

        for(const auto& face : mFaces)
        {
            ImGui::PushID(idx);

            auto* texturePtr = reinterpret_cast<void*>(static_cast<intptr_t>(mFaceTextures[idx]->id()));

            ImGui::BeginGroup();

            ImGui::ImageButton(texturePtr, { itemSize - 20, itemSize - 16 });
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
