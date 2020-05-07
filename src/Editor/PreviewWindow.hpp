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

        void init();

        void begin() const;
        void end() const;

        void drawFrame(const std::shared_ptr<Xenon::Texture2D>& frame);
        void drawDetectionResult(const std::vector<FaceDetectionResult>& detectionResult) const;

        void updateGui(const Xenon::DeltaTime& deltaTime);
    private:
        XN_NODISCARD glm::mat4 fitDisplayQuadToFrame(uint32_t frameWidth, uint32_t frameHeight) const;
        XN_NODISCARD glm::mat4 mapDetectionToDisplayQuad(const FaceDetectionResult& detectionResult, 
            uint32_t frameWidth, uint32_t frameHeight) const;

        XN_NODISCARD glm::vec2 calculateDisplayQuadAspect(uint32_t frameWidth, uint32_t frameHeight) const;

        bool mPreviewWindowOpen{ true };
        Xenon::WindowResolution mWindowSize{ Xenon::StandardWindowResolution::A_4X3_R_640X480 };
        Xenon::WindowResolution mCurrentFrameSize{};

        Xenon::ResourceCache<Xenon::Shader> mShaderCache;
        std::shared_ptr<Xenon::FrameBuffer> mFrameBuffer;
        std::shared_ptr<Xenon::OrthographicCamera> mCamera;

        std::shared_ptr<Xenon::Shader> mFlatShader;
        std::shared_ptr<Xenon::Shader> mTextureShader;
        std::shared_ptr<Xenon::VertexBuffer> mQuadVertexBuffer;
        std::shared_ptr<Xenon::VertexBuffer> mTextureQuadVertexBuffer;
        std::shared_ptr<Xenon::IndexBuffer> mIndexBuffer;
        std::shared_ptr<Xenon::VertexArray> mQuadVertexArray;
        std::shared_ptr<Xenon::VertexArray> mTextureQuadVertexArray;
    };
}
