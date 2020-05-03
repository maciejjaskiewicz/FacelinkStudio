#pragma once

#include <Xenon/Graphics.hpp>
#include <Xenon/Core/Window/WindowResolution.hpp>

namespace Fls
{
    class PreviewWindow
    {
    public:
        explicit PreviewWindow();

        void draw(const std::shared_ptr<Xenon::Texture2D>& frame) const;
        void updateGui(const Xenon::DeltaTime deltaTime);
    private:
        XN_NODISCARD glm::mat4 fitDisplayQuadToFrame(uint32_t frameWidth, uint32_t frameHeight) const;

        bool mPreviewWindowOpen{ true };
        Xenon::WindowResolution mWindowSize{ Xenon::StandardWindowResolution::A_4X3_R_640X480 };

        Xenon::ResourceCache<Xenon::Shader> mShaderCache;
        std::shared_ptr<Xenon::FrameBuffer> mFrameBuffer;
        std::shared_ptr<Xenon::OrthographicCamera> mCamera;

        std::shared_ptr<Xenon::Shader> mShader;
        std::shared_ptr<Xenon::VertexBuffer> mVertexBuffer;
        std::shared_ptr<Xenon::IndexBuffer> mIndexBuffer;
        std::shared_ptr<Xenon::VertexArray> mVertexArray;
    };
}
