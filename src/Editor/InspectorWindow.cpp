#include "InspectorWindow.hpp"

#include "Events/FaceDetectionSettingChangedEvent.hpp"
#include "Events/PreviewCameraResetEvent.hpp"

#include <Xenon/Core/ApplicationServices.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <numeric>
#include <glm/gtc/type_ptr.hpp>
#include <cuda_runtime.h>

namespace Fls
{
    InspectorWindow::InspectorWindow() = default;

    void InspectorWindow::init()
    {
        mApiDetails = Xenon::ApplicationServices::Renderer::ref().getDetails();

        Xenon::ApplicationServices::EventBus::ref().subscribe<GeneralSettingsChangedEvent>(
            [this](const GeneralSettingsChangedEvent& event)
        {
            if (event.triggeredByCamera)
            {
                mGeneralSettings.cameraEnabled = event.cameraEnabled;
                mGeneralSettings.cameraZoom = event.cameraZoom;
            }
        });

        cudaRuntimeGetVersion(&mCudaVersion);
    }

    void InspectorWindow::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        ImGui::Begin("Inspector");

        drawDiagnosticSection(deltaTime);
        drawGeneralSection();
        drawFaceDetectionSection();
        drawFaceRecognitionSection();

        ImGui::End();
    }

    void InspectorWindow::drawDiagnosticSection(const Xenon::DeltaTime& deltaTime) const
    {
        if (!ImGui::CollapsingHeader("Diagnostic", ImGuiTreeNodeFlags_DefaultOpen)) 
            return;

        const auto frameRateBufferSize = 60;
        static float frameRateBuffer[frameRateBufferSize];
        static auto frameRateBuffIdx = 0;

        frameRateBuffer[frameRateBuffIdx] = deltaTime.milliseconds();
        frameRateBuffIdx = (frameRateBuffIdx + 1) % frameRateBufferSize;

        const auto sum = std::accumulate(frameRateBuffer, frameRateBuffer + frameRateBufferSize, 0.0f);
        const auto avg = sum / frameRateBufferSize;

        const auto fpsBufferSize = 60;
        static float fpsBuffer[fpsBufferSize];
        static auto fpsBufferIdx = 0;

        if (frameRateBuffIdx % 10 == 0)
        {
            fpsBuffer[fpsBufferIdx] = 1000.0f / avg;
            fpsBufferIdx = (fpsBufferIdx + 1) % fpsBufferSize;
        }

        ImGui::Text("Application Frame Rate: %.3f ms/frame (%.1f FPS)", avg, 1000.0f / avg);

        ImGui::PushItemWidth(-1);
        ImGui::PushStyleColor(ImGuiCol_PlotLines, ImVec4(0.1f, 1.0f, 0.1f, 1.0f));
        ImGui::PlotLines("", fpsBuffer, IM_ARRAYSIZE(fpsBuffer), fpsBufferIdx);
        ImGui::PopStyleColor();
        ImGui::PopItemWidth();

        ImGui::Text("Graphics API: %s %s", mApiDetails.name.c_str(), mApiDetails.version.c_str());
        ImGui::Text("Renderer: %s", mApiDetails.renderer.c_str());
        ImGui::Text("CUDA Version: %d.%d", mCudaVersion / 1000, mCudaVersion % 100);
    }

    void InspectorWindow::drawGeneralSection()
    {
        if (!ImGui::CollapsingHeader("General", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        auto settingsChanged = false;

        settingsChanged = ImGui::ColorEdit4("Background Color", 
            glm::value_ptr(mGeneralSettings.backgroundColor)) || settingsChanged;

        ImGui::PushItemFlag(ImGuiItemFlags_Disabled, true);
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, ImGui::GetStyle().Alpha * 0.9f);

        ImGui::Checkbox("Camera", &mGeneralSettings.cameraEnabled);

        ImGui::PopItemFlag();
        ImGui::PopStyleVar();

        ImGui::SameLine();

        if (ImGui::Button("Reset"))
        {
            Xenon::ApplicationServices::EventBus::ref().trigger<PreviewCameraResetEvent>();
        }

        settingsChanged = ImGui::SliderFloat("Camera Zoom", &mGeneralSettings.cameraZoom, 
            1.0f, 5.0f, "%.1f") || settingsChanged;

        if(settingsChanged)
        {
            Xenon::ApplicationServices::EventBus::ref().trigger(mGeneralSettings.toEvent());
        }
    }

    void InspectorWindow::drawFaceDetectionSection()
    {
        if (!ImGui::CollapsingHeader("Face Detection", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        auto settingsChanged = false;

        settingsChanged = ImGui::SliderFloat("Confidence Threshold##Detection", 
            &mFaceDetectionSettings.confidenceThreshold, 0.0f, 1.0f) || settingsChanged;
        settingsChanged = ImGui::Checkbox("Show Detection Box", 
            &mFaceDetectionSettings.showDetectionBox) || settingsChanged;
        settingsChanged = ImGui::ColorEdit4("Box Outline Color", 
            glm::value_ptr(mFaceDetectionSettings.outlineColor)) || settingsChanged;
        settingsChanged = ImGui::ColorEdit4("Box Fill Color", 
            glm::value_ptr(mFaceDetectionSettings.fillColor)) || settingsChanged;
        settingsChanged = ImGui::SliderFloat("Box Outline Thickness", 
            &mFaceDetectionSettings.outlineThickness, 0.001f, 0.1f) || settingsChanged;

        if(settingsChanged)
        {
            Xenon::ApplicationServices::EventBus::ref().trigger(mFaceDetectionSettings.toEvent());
        }
    }

    void InspectorWindow::drawFaceRecognitionSection()
    {
        if (!ImGui::CollapsingHeader("Face Recognition", ImGuiTreeNodeFlags_DefaultOpen))
            return;

        auto settingsChanged = false;

        settingsChanged = ImGui::SliderFloat("Confidence Threshold##Recognition", 
            &mFaceRecognitionSettings.confidenceThreshold, 0.0f, 1.0f) || settingsChanged;
        settingsChanged = ImGui::Checkbox("Show Face Landmarks", 
            &mFaceRecognitionSettings.showLandmarks) || settingsChanged;
        settingsChanged = ImGui::SliderFloat("Landmark Size",
            &mFaceRecognitionSettings.landmarkSize, 0.01f, 0.05f) || settingsChanged;
        settingsChanged = ImGui::ColorEdit4("Landmark Color", 
            glm::value_ptr(mFaceRecognitionSettings.landmarkColor)) || settingsChanged;

        if (settingsChanged)
        {
            Xenon::ApplicationServices::EventBus::ref().trigger(mFaceRecognitionSettings.toEvent());
        }
    }

    GeneralSettingsChangedEvent InspectorWindow::GeneralSettings::toEvent() const
    {
        return GeneralSettingsChangedEvent(backgroundColor, cameraEnabled, cameraZoom);
    }

    FaceDetectionSettingChangedEvent InspectorWindow::FaceDetectionSettings::toEvent() const
    {
        return FaceDetectionSettingChangedEvent(showDetectionBox, confidenceThreshold, 
            outlineThickness, outlineColor, fillColor
        );
    }

    FaceRecognitionSettingChangedEvent InspectorWindow::FaceRecognitionSettings::toEvent() const
    {
        return FaceRecognitionSettingChangedEvent(confidenceThreshold, showLandmarks, landmarkSize, landmarkColor);
    }
}
