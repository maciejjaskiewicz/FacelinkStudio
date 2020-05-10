#pragma once

#include "Detectors/FaceDetectionResult.hpp"

#include <Xenon/Graphics.hpp>
#include <Xenon/Core/Window/WindowResolution.hpp>

namespace Fls
{
    class PreviewWindow
    {
    public:
        explicit PreviewWindow();

        void init(Xenon::ResourceCache<Xenon::Shader>& shaderCache);

        void begin() const;
        void end() const;

        void drawFrame(const std::shared_ptr<Xenon::Texture2D>& frame);
        void drawDetectionResult(const std::vector<FaceDetectionResult>& detectionResult) const;

        void updateGui(const Xenon::DeltaTime& deltaTime);
    private:
        struct FaceDetectionBoxNdc
        {
            float x, y;
            float width, height;
        };

        XN_NODISCARD glm::mat4 fitDisplayQuadToFrame(uint32_t frameWidth, uint32_t frameHeight) const;

        XN_NODISCARD glm::vec2 calculateDisplayQuadAspect(uint32_t frameWidth, uint32_t frameHeight) const;
        XN_NODISCARD FaceDetectionBoxNdc translateDetectionBoxToNdc(const FaceDetectionResult& detectionResult, 
            uint32_t frameWidth, uint32_t frameHeight) const;

        bool mPreviewWindowOpen{ true };
        Xenon::WindowResolution mWindowSize{ Xenon::StandardWindowResolution::A_4X3_R_640X480 };
        Xenon::WindowResolution mCurrentFrameSize{};

        std::shared_ptr<Xenon::FrameBuffer> mFrameBuffer;
        std::shared_ptr<Xenon::OrthographicCamera> mCamera;

        std::shared_ptr<Xenon::Shader> mDetectionBoxShader, mTextureShader;
        std::shared_ptr<Xenon::VertexBuffer> mQuadVertexBuffer;
        std::shared_ptr<Xenon::IndexBuffer> mIndexBuffer;
        std::shared_ptr<Xenon::VertexArray> mQuadVertexArray;

        bool mShowDetectionBoxes{ true };
        float mDetectionBoxOutlineWidth{ 0.02f };
        glm::vec4 mDetectionBoxOutline{ 0.1f, 0.8f, 0.1f, 0.8f };
        glm::vec4 mDetectionBoxFill{ 0.1f, 0.8f, 0.1f, 0.15f };
    };
}
