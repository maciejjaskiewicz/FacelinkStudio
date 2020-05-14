#include "PreviewWindow.hpp"

#include "Utils/Math.hpp"
#include "Events/FaceDetectionSettingChangedEvent.hpp"

#include <Xenon/Core/ApplicationServices.hpp>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
#include "Events/FaceRecognitionSettingChangedEvent.hpp"
#include "Events/GeneralSettingsChangedEvent.hpp"

namespace Fls
{
    // TODO: EditorWindow
    PreviewWindow::PreviewWindow() = default;

    void PreviewWindow::init(Xenon::ResourceCache<Xenon::Shader>& shaderCache)
    {
        mFrameBuffer = Xenon::FrameBuffer::create(mWindowSize.width, mWindowSize.height);

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

        mQuadVertexBuffer = Xenon::VertexBuffer::create(textureQuadVertices, sizeof textureQuadVertices);
        mIndexBuffer = Xenon::IndexBuffer::create(indices, sizeof(indices) / sizeof(uint32_t));

        const Xenon::BufferLayout textureQuadBufferLayout = {
            { Xenon::DataType::Float3, "aPosition" },
            { Xenon::DataType::Float2, "aTextureCoordinate" }
        };

        mQuadVertexBuffer->setLayout(textureQuadBufferLayout);

        mQuadVertexArray = Xenon::VertexArray::create();
        mQuadVertexArray->pushVertexBuffer(mQuadVertexBuffer);
        mQuadVertexArray->bindIndexBuffer(mIndexBuffer);

        mDetectionBoxShader = shaderCache.load("detectionBox");
        mTextureShader = shaderCache.load("texture");
        mQuadShader = shaderCache.load("flat");

        mCameraController = std::make_unique<PreviewCameraController>(mWindowSize.width / mWindowSize.height);

        Xenon::ApplicationServices::EventBus::ref().subscribe<FaceDetectionSettingChangedEvent>(
            [this](const FaceDetectionSettingChangedEvent& event)
        {
            mShowDetectionBoxes = event.showDetectionBox;
            mDetectionBoxOutlineWidth = event.outlineThickness;
            mDetectionBoxOutline = event.outlineColor;
            mDetectionBoxFill = event.fillColor;
        });

        Xenon::ApplicationServices::EventBus::ref().subscribe<FaceRecognitionSettingChangedEvent>(
            [this](const FaceRecognitionSettingChangedEvent& event)
        {
            mShowFaceLandmarks = event.showLandmarks;
            mFaceLandmarkSize = event.landmarkSize;
            mFaceLandmarkColor = event.landmarkColor;
        });

        Xenon::ApplicationServices::EventBus::ref().subscribe<GeneralSettingsChangedEvent>(
            [this](const GeneralSettingsChangedEvent& event)
        {
            if (!event.triggeredByCamera)
                mBackgroundColor = event.backgroundColor;
        });
    }

    void PreviewWindow::begin(const Xenon::DeltaTime& deltaTime) const
    {
        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        mCameraController->update(deltaTime);

        mFrameBuffer->bind();
        renderer.beginScene(mCameraController->camera());

        Xenon::RenderCmd::setClearColor(mBackgroundColor);
        Xenon::RenderCmd::clear();
    }

    void PreviewWindow::end() const
    {
        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        renderer.endScene();
        mFrameBuffer->unbind();
    }

    void PreviewWindow::drawFrame(const int64 frameId, const std::shared_ptr<Xenon::Texture2D>& frame)
    {
        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        if(mLastFrameId != frameId)
        {
            mCameraController->reset();
            mLastFrameId = frameId;
        }

        mCurrentFrameSize = Xenon::WindowResolution(frame->width(), frame->height());
        mCurrentDisplayQuadAspect = calculateDisplayQuadAspect(frame->width(), frame->height());

        glm::mat4 transform = glm::mat4(1.0f);
        transform *= fitDisplayQuadToFrame();

        frame->bind();
        mTextureShader->bind();
        mTextureShader->setInt("uTexture", 0);
        renderer.submit(mTextureShader, mQuadVertexArray, transform);
    }

    void PreviewWindow::drawDetectionResult(const std::vector<FaceDetectionResult>& detectionResult) const
    {
        if(detectionResult.empty() || !mShowDetectionBoxes)
            return;

        auto& renderer = Xenon::ApplicationServices::Renderer::ref();
        
        mDetectionBoxShader->bind();
        mDetectionBoxShader->setFloat("uBoxOutlineWidth", mDetectionBoxOutlineWidth);
        mDetectionBoxShader->setFloat4("uBoxOutlineColor", mDetectionBoxOutline);
        mDetectionBoxShader->setFloat4("uBoxFillColor", mDetectionBoxFill);

        for(const auto& detection : detectionResult)
        {
            const auto ndcDetectionBox = translateBoxToNdc(glm::vec2(detection.x1, detection.y1), glm::vec2(detection.x2, detection.y2));

            auto transform = glm::translate(glm::mat4(1.0f),
                glm::vec3(ndcDetectionBox.x, -ndcDetectionBox.y, 0.0f));

            transform *= glm::scale(glm::mat4(1.0f), glm::vec3(ndcDetectionBox.width, ndcDetectionBox.height, 0.0f));

            renderer.submit(mDetectionBoxShader, mQuadVertexArray, transform);
        }
    }

    void PreviewWindow::drawAlignmentResult(const std::vector<std::shared_ptr<FaceAlignmentResult>>& alignmentResult) const
    {
        if (alignmentResult.empty() || !mShowFaceLandmarks)
            return;

        auto& renderer = Xenon::ApplicationServices::Renderer::ref();

        mQuadShader->bind();
        mQuadShader->setFloat4("uColor", mFaceLandmarkColor);

        for(const auto& face : alignmentResult)
        {
            const auto minFaceDimension = std::min(face->srcFaceSize.x, face->srcFaceSize.y);
            const auto landmarkQuadSize = minFaceDimension * mFaceLandmarkSize;

            for (const auto& landmark : face->landmarks)
            {
                const auto ndcQuad = translateBoxToNdc(glm::vec2(landmark.x, landmark.y),
                    glm::vec2(landmark.x + landmarkQuadSize, landmark.y + landmarkQuadSize));

                auto transform = glm::translate(glm::mat4(1.0f),
                    glm::vec3(ndcQuad.x, -ndcQuad.y, 0.0f));

                transform *= glm::scale(glm::mat4(1.0f), glm::vec3(ndcQuad.width, ndcQuad.height, 0.0f));

                renderer.submit(mQuadShader, mQuadVertexArray, transform);
            }
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

            mCameraController->setAspect(windowSize.aspectRatio());
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

    glm::mat4 PreviewWindow::fitDisplayQuadToFrame() const
    {
        return glm::scale(glm::mat4(1.0f), glm::vec3(mCurrentDisplayQuadAspect.x, mCurrentDisplayQuadAspect.y, 1.0f));
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

    PreviewWindow::BoxNdc PreviewWindow::translateBoxToNdc(const glm::vec2 point1, const glm::vec2 point2) const
    {
        const auto detQuadCenterX = static_cast<float>(point1.x + point2.x) / 2;
        const auto detQuadCenterY = static_cast<float>(point1.y + point2.y) / 2;

        const auto detQuadWidth = static_cast<float>(point2.x - point1.x);
        const auto detQuadHeight = static_cast<float>(point2.y - point1.y);

        const auto displayQuadAspect = mCurrentDisplayQuadAspect;

        const auto detQuadNdcX = Math::interpolate(detQuadCenterX,
            0.0f, static_cast<float>(mCurrentFrameSize.width),
            -displayQuadAspect.x, displayQuadAspect.x);

        const auto detQuadNdcY = Math::interpolate(detQuadCenterY,
            0.0f, static_cast<float>(mCurrentFrameSize.height),
            -displayQuadAspect.y, displayQuadAspect.y);

        const auto detQuadNdcWidth = Math::interpolate(detQuadWidth,
            0.0f, static_cast<float>(mCurrentFrameSize.width),
            0.0f, displayQuadAspect.x);
        const auto detQuadNdcHeight = Math::interpolate(detQuadHeight,
            0.0f, static_cast<float>(mCurrentFrameSize.height),
            0.0f, displayQuadAspect.y);

        return BoxNdc
        {
            detQuadNdcX,
            detQuadNdcY,
            detQuadNdcWidth,
            detQuadNdcHeight
        };
    }
}
