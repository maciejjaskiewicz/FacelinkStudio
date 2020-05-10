#pragma once

#include <Xenon/Graphics.hpp>
#include "Events/FaceDetectionSettingChangedEvent.hpp"

namespace Fls
{
    class InspectorWindow
    {
    public:
        explicit InspectorWindow();

        void init();
        void updateGui(const Xenon::DeltaTime& deltaTime);

        void drawDiagnosticSection(const Xenon::DeltaTime& deltaTime) const;
        void drawFaceDetectionSection();
        void drawFaceRecognitionSection() const;
    private:
        struct FaceDetectionSettings
        {
            bool showDetectionBox{ true };
            float confidenceThreshold{ 0.5f };
            float outlineThickness{ 0.02f };
            glm::vec4 outlineColor{ 0.1f, 0.8f, 0.1f, 0.8f };
            glm::vec4 fillColor{ 0.1f, 0.8f, 0.1f, 0.15f };

            XN_NODISCARD FaceDetectionSettingChangedEvent toEvent() const;
        } mFaceDetectionSettings;

        Xenon::RendererAPIDetails mApiDetails{};
        int mCudaVersion{};
    };
}
