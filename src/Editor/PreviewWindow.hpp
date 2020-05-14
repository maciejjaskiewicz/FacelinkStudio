#pragma once

#include "Detectors/FaceDetectionResult.hpp"
#include "Detectors/FaceAlignmentResult.hpp"
#include "PreviewCameraController.hpp"

#include <Xenon/Graphics.hpp>
#include <Xenon/Core/Window/WindowResolution.hpp>

namespace Fls
{
    class PreviewWindow
    {
    public:
        explicit PreviewWindow();

        void init(Xenon::ResourceCache<Xenon::Shader>& shaderCache);

        void begin(const Xenon::DeltaTime& deltaTime) const;
        void end() const;

        void drawFrame(int64 frameId, const std::shared_ptr<Xenon::Texture2D>& frame);
        void drawDetectionResult(const std::vector<FaceDetectionResult>& detectionResult) const;
        void drawAlignmentResult(const std::vector<std::shared_ptr<FaceAlignmentResult>>& alignmentResult) const;

        void updateGui(const Xenon::DeltaTime& deltaTime);
    private:
        struct BoxNdc
        {
            float x, y;
            float width, height;
        };

        XN_NODISCARD glm::mat4 fitDisplayQuadToFrame() const;

        XN_NODISCARD glm::vec2 calculateDisplayQuadAspect(uint32_t frameWidth, uint32_t frameHeight) const;
        XN_NODISCARD BoxNdc translateBoxToNdc(glm::vec2 point1, glm::vec2 point2) const;

        bool mPreviewWindowOpen{ true };
        Xenon::WindowResolution mWindowSize{ Xenon::StandardWindowResolution::A_4X3_R_640X480 };
        Xenon::WindowResolution mCurrentFrameSize{};
        glm::vec2 mCurrentDisplayQuadAspect{};

        std::unique_ptr<PreviewCameraController> mCameraController;
        std::shared_ptr<Xenon::FrameBuffer> mFrameBuffer;

        std::shared_ptr<Xenon::Shader> mDetectionBoxShader, mTextureShader, mQuadShader;
        std::shared_ptr<Xenon::VertexBuffer> mQuadVertexBuffer;
        std::shared_ptr<Xenon::IndexBuffer> mIndexBuffer;
        std::shared_ptr<Xenon::VertexArray> mQuadVertexArray;

        int64 mLastFrameId{};
        glm::vec4 mBackgroundColor{ 0.15f, 0.15f, 0.15f, 1.0f };

        bool mShowDetectionBoxes{ true };
        float mDetectionBoxOutlineWidth{ 0.02f };
        glm::vec4 mDetectionBoxOutline{ 0.1f, 0.8f, 0.1f, 0.8f };
        glm::vec4 mDetectionBoxFill{ 0.1f, 0.8f, 0.1f, 0.15f };

        bool mShowFaceLandmarks{ true };
        float mFaceLandmarkSize{ 0.02f };
        glm::vec4 mFaceLandmarkColor{ 0.1f, 0.1f, 0.8f, 1.0f };
    };
}
