#include "PreviewWindow.hpp"

#include <Xenon/Core/ApplicationServices.hpp>

#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Fls
{
    // TODO: EditorWindow
    PreviewWindow::PreviewWindow() = default;

    void PreviewWindow::init()
    {
        mFrameBuffer = Xenon::FrameBuffer::create(mWindowSize.width, mWindowSize.height);

        float vertices[4 * 5] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f
        };

        uint32_t indices[3 * 2] = {
            0, 1, 2,
            2, 1, 3
        };

        mVertexBuffer = Xenon::VertexBuffer::create(vertices, sizeof(vertices));
        mIndexBuffer = Xenon::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));

        const Xenon::BufferLayout bufferLayout = {
            { Xenon::DataType::Float3, "aPosition" },
            { Xenon::DataType::Float2, "aTextureCoordinate" }
        };
        mVertexBuffer->setLayout(bufferLayout);

        mVertexArray = Xenon::VertexArray::create();
        mVertexArray->pushVertexBuffer(mVertexBuffer);
        mVertexArray->bindIndexBuffer(mIndexBuffer);

        mShader = mShaderCache.load<Xenon::ShaderLoader>("texture", Xenon::ShaderLoaderArgs(
            "assets/shaders/TextureVertex.glsl",
            "assets/shaders/TextureFragment.glsl"
        ));

        mCamera = std::make_shared<Xenon::OrthographicCamera>(Xenon::OrthographicCameraProjConfiguration{
            -1.0f, 1.0f, -1.0f, 1.0f
        });
    }

    void PreviewWindow::draw(const std::shared_ptr<Xenon::Texture2D>& frame) const
    {
        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        mFrameBuffer->bind();
        renderer.beginScene(*mCamera);

        Xenon::RenderCmd::setClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
        Xenon::RenderCmd::clear();

        glm::mat4 transform = glm::mat4(1.0f);
        transform *= fitDisplayQuadToFrame(frame->width(), frame->height());

        frame->bind();
        mShader->bind();
        mShader->setInt("uTexture", 0);
        renderer.submit(mShader, mVertexArray, transform);

        renderer.endScene();
        mFrameBuffer->unbind();
    }

    void PreviewWindow::updateGui(const Xenon::DeltaTime deltaTime)
    {
        const auto sceneFlags = ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | 
            ImGuiWindowFlags_NoScrollWithMouse;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_ResizeGrip, 0);

        ImGui::Begin("Preview", &mPreviewWindowOpen, sceneFlags);

        const auto pos = ImGui::GetCursorScreenPos();
        const auto imGuiWindowSize = ImGui::GetContentRegionAvail();
        const auto windowSize = Xenon::WindowResolution(
            static_cast<uint32_t>(imGuiWindowSize.x),
            static_cast<uint32_t>(imGuiWindowSize.y)
        );

        if (windowSize != mWindowSize)
        {
            mFrameBuffer = Xenon::FrameBuffer::create(windowSize.width, windowSize.height);
            mWindowSize = windowSize;

            mCamera->setProjection(Xenon::OrthographicCameraProjConfiguration{
                -windowSize.aspectRatio(),
                windowSize.aspectRatio(),
                -1.0f,
                1.0f
            });
        }

        auto* texturePtr = reinterpret_cast<void*>(static_cast<intptr_t>(mFrameBuffer->texture().id()));
        const auto textureWidth = static_cast<float>(mFrameBuffer->texture().width());
        const auto textureHeight = static_cast<float>(mFrameBuffer->texture().height());

        ImGui::GetWindowDrawList()->AddImage(texturePtr, pos, ImVec2(pos.x + textureWidth, pos.y + textureHeight),
            ImVec2(0, 1), ImVec2(1, 0));

        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor();
    }

    glm::mat4 PreviewWindow::fitDisplayQuadToFrame(const uint32_t frameWidth, const uint32_t frameHeight) const
    {
        const auto frameAspectRatio = static_cast<float>(frameWidth) / frameHeight;

        auto displayQuadWidth = static_cast<float>(mWindowSize.aspectRatio());
        auto displayQuadHeight = displayQuadWidth / frameAspectRatio;

        if (displayQuadHeight > 1.0f)
        {
            displayQuadWidth /= displayQuadHeight;
            displayQuadHeight = 1.0f;
        }

        return glm::scale(glm::mat4(1.0f), glm::vec3(displayQuadWidth, displayQuadHeight, 1.0f));
    }
}
