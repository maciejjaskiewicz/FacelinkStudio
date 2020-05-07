#include "PreviewWindow.hpp"

#include "Utils/Math.hpp"

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

        float quadVertices[4 * 3] = {
            -1.0f, -1.0f, 0.0f,
             1.0f, -1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f,
             1.0f,  1.0f, 0.0f
        };

        float textureQuadVertices[4 * 5] = {
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f
        };

        uint32_t indices[3 * 2] = {
            0, 1, 2,
            2, 1, 3
        };

        mQuadVertexBuffer = Xenon::VertexBuffer::create(quadVertices, sizeof quadVertices);
        mTextureQuadVertexBuffer = Xenon::VertexBuffer::create(textureQuadVertices, sizeof textureQuadVertices);

        mIndexBuffer = Xenon::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));

        const Xenon::BufferLayout quadBufferLayout = {
            { Xenon::DataType::Float3, "aPosition" },
        };
        const Xenon::BufferLayout textureQuadBufferLayout = {
            { Xenon::DataType::Float3, "aPosition" },
            { Xenon::DataType::Float2, "aTextureCoordinate" }
        };

        mQuadVertexBuffer->setLayout(quadBufferLayout);
        mTextureQuadVertexBuffer->setLayout(textureQuadBufferLayout);

        mQuadVertexArray = Xenon::VertexArray::create();
        mQuadVertexArray->pushVertexBuffer(mQuadVertexBuffer);
        mQuadVertexArray->bindIndexBuffer(mIndexBuffer);

        mTextureQuadVertexArray = Xenon::VertexArray::create();
        mTextureQuadVertexArray->pushVertexBuffer(mTextureQuadVertexBuffer);
        mTextureQuadVertexArray->bindIndexBuffer(mIndexBuffer);

        mFlatShader = mShaderCache.load<Xenon::ShaderLoader>("flat", Xenon::ShaderLoaderArgs(
            "assets/shaders/FlatColorVertex.glsl",
            "assets/shaders/FlatColorFragment.glsl"
        ));

        mTextureShader = mShaderCache.load<Xenon::ShaderLoader>("texture", Xenon::ShaderLoaderArgs(
            "assets/shaders/TextureVertex.glsl",
            "assets/shaders/TextureFragment.glsl"
        ));

        mCamera = std::make_shared<Xenon::OrthographicCamera>(Xenon::OrthographicCameraProjConfiguration{
            -1.0f, 1.0f, -1.0f, 1.0f
        });
    }

    void PreviewWindow::begin() const
    {
        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        mFrameBuffer->bind();
        renderer.beginScene(*mCamera);

        Xenon::RenderCmd::setClearColor(glm::vec4(0.15f, 0.15f, 0.15f, 1.0f));
        Xenon::RenderCmd::clear();
    }

    void PreviewWindow::end() const
    {
        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        renderer.endScene();
        mFrameBuffer->unbind();
    }

    void PreviewWindow::drawFrame(const std::shared_ptr<Xenon::Texture2D>& frame)
    {
        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        mCurrentFrameSize = Xenon::WindowResolution(frame->width(), frame->height());

        glm::mat4 transform = glm::mat4(1.0f);
        transform *= fitDisplayQuadToFrame(frame->width(), frame->height());

        frame->bind();
        mTextureShader->bind();
        mTextureShader->setInt("uTexture", 0);
        renderer.submit(mTextureShader, mTextureQuadVertexArray, transform);
    }

    void PreviewWindow::drawDetectionResult(const std::vector<FaceDetectionResult>& detectionResult) const
    {
        if(detectionResult.empty())
            return;

        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        mFlatShader->bind();
        mFlatShader->setFloat4("uColor", glm::vec4(0.1f, 0.8f, 0.1f, 0.3f));

        for(const auto& detection : detectionResult)
        {
            const auto transform = mapDetectionToDisplayQuad(detection, mCurrentFrameSize.width, mCurrentFrameSize.height);
            renderer.submit(mFlatShader, mQuadVertexArray, transform);
        }
    }

    void PreviewWindow::updateGui(const Xenon::DeltaTime& deltaTime)
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
        const auto displayQuadAspect = calculateDisplayQuadAspect(frameWidth, frameHeight);

        return glm::scale(glm::mat4(1.0f), glm::vec3(displayQuadAspect.x, displayQuadAspect.y, 1.0f));
    }

    glm::mat4 PreviewWindow::mapDetectionToDisplayQuad(const FaceDetectionResult& detectionResult, 
        const uint32_t frameWidth, const uint32_t frameHeight) const
    {
        const auto detQuadCenterX = static_cast<float>(detectionResult.x1 + detectionResult.x2) / 2;
        const auto detQuadCenterY = static_cast<float>(detectionResult.y1 + detectionResult.y2) / 2;

        const auto detQuadWidth = static_cast<float>(detectionResult.x2 - detectionResult.x1);
        const auto detQuadHeight = static_cast<float>(detectionResult.y2 - detectionResult.y1);

        const auto displayQuadAspect = calculateDisplayQuadAspect(frameWidth, frameHeight);

        const auto detQuadNdcX = Math::interpolate(detQuadCenterX, 
            0.0f, static_cast<float>(frameWidth),
            -displayQuadAspect.x, displayQuadAspect.x);

        const auto detQuadNdcY = Math::interpolate(detQuadCenterY,
            0.0f, static_cast<float>(frameHeight),
            -displayQuadAspect.y, displayQuadAspect.y);

        const auto detQuadNdcWidth = Math::interpolate(detQuadWidth, 
            0.0f, static_cast<float>(frameWidth), 
            0.0f, displayQuadAspect.x);
        const auto detQuadNdcHeight = Math::interpolate(detQuadHeight,
            0.0f, static_cast<float>(frameHeight), 
            0.0f, displayQuadAspect.y);

        const auto transform = glm::translate(glm::mat4(1.0f), glm::vec3(detQuadNdcX, -detQuadNdcY, 0.0f));

        return transform * glm::scale(glm::mat4(1.0f), glm::vec3(detQuadNdcWidth, detQuadNdcHeight, 0.0f));
    }

    glm::vec2 PreviewWindow::calculateDisplayQuadAspect(const uint32_t frameWidth, const uint32_t frameHeight) const
    {
        const auto frameAspectRatio = static_cast<float>(frameWidth) / frameHeight;

        auto displayQuadWidth = static_cast<float>(mWindowSize.aspectRatio());
        auto displayQuadHeight = displayQuadWidth / frameAspectRatio;

        if (displayQuadHeight > 1.0f)
        {
            displayQuadWidth /= displayQuadHeight;
            displayQuadHeight = 1.0f;
        }

        return glm::vec2(displayQuadWidth, displayQuadHeight);
    }
}
