#pragma once

#include "Events/FaceDetectionSettingChangedEvent.hpp"
#include "Events/FaceRecognitionSettingChangedEvent.hpp"
#include "Events/GeneralSettingsChangedEvent.hpp"

#include <Xenon/Graphics.hpp>

namespace Fls
{
    class InspectorWindow
    {
    public:
        explicit InspectorWindow();

        void init();
        void updateGui(const Xenon::DeltaTime& deltaTime);

        void drawDiagnosticSection(const Xenon::DeltaTime& deltaTime) const;
        void drawGeneralSection();
        void drawFaceDetectionSection();
        void drawFaceRecognitionSection();
    private:
        struct GeneralSettings
        {
            glm::vec4 backgroundColor{ 0.15f, 0.15f, 0.15f, 1.0f };
            bool cameraEnabled{ true };
            float cameraZoom{ 1.0f };

            XN_NODISCARD GeneralSettingsChangedEvent toEvent() const;
        } mGeneralSettings;

        struct FaceDetectionSettings
        {
            bool showDetectionBox{ true };
            float confidenceThreshold{ 0.5f };
            float outlineThickness{ 0.02f };
            glm::vec4 outlineColor{ 0.1f, 0.8f, 0.1f, 0.8f };
            glm::vec4 fillColor{ 0.1f, 0.8f, 0.1f, 0.15f };

            XN_NODISCARD FaceDetectionSettingChangedEvent toEvent() const;
        } mFaceDetectionSettings;

        struct FaceRecognitionSettings
        {
            float confidenceThreshold{ 0.3f };
            bool showLandmarks{ true };
            float landmarkSize{ 0.02f };
            glm::vec4 landmarkColor{ 0.1f, 0.1f, 0.8f, 0.8f };

            XN_NODISCARD FaceRecognitionSettingChangedEvent toEvent() const;
        } mFaceRecognitionSettings;

        Xenon::RendererAPIDetails mApiDetails{};
        int mCudaVersion{};
    };
}
