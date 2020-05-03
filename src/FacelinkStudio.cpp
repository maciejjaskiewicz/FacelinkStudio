#include "FacelinkStudio.hpp"

#include <Xenon/Graphics.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/gtc/type_ptr.hpp>

#include "Editor/Editor.hpp"

namespace Fls
{
    FacelinkStudio::FacelinkStudio(const Xenon::ApplicationConfiguration& config)
        : Application(config)
    {
        Editor::setStyle("assets/fonts/Ruda-SemiBold.ttf");

        mDockSpace = std::make_unique<DockSpace>("FacelinkStudioDockSpace");
    }

    void FacelinkStudio::init()
    {
        mDockSpace->setInitialWindowsConfiguration([](const ImGuiID mainNodeId)
        {
            ImGuiID rightNodeId, rightDownNodeId;

            const auto leftNodeId = ImGui::DockBuilderSplitNode(mainNodeId, ImGuiDir_Left, 
                0.7f, nullptr, &rightNodeId);
            const auto rightUpNodeId = ImGui::DockBuilderSplitNode(rightNodeId, ImGuiDir_Up,
                0.65f, nullptr, &rightDownNodeId);

            ImGui::DockBuilderDockWindow("Scene", leftNodeId);
            ImGui::DockBuilderDockWindow("Dear ImGui Demo", rightUpNodeId);
            ImGui::DockBuilderDockWindow("Dear ImGui Metrics", rightDownNodeId);
        });

        mPreviewWindow = std::make_unique<PreviewWindow>();

        mTexture = mTextureCache.load<Xenon::Texture2DLoader>("test", Xenon::Texture2DLoaderArgs(
            "assets/textures/squirviel_hd.jpg"
        ));

        XN_INFO("Facelink Studio initialized");
    }

    void FacelinkStudio::update(const Xenon::DeltaTime& deltaTime)
    {
        Xenon::RenderCmd::setClearColor(mClearColor);
        Xenon::RenderCmd::clear();

        mPreviewWindow->draw(mTexture);
    }

    void FacelinkStudio::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        mDockSpace->begin();

        ImGui::ShowMetricsWindow();
        ImGui::ShowDemoWindow();

        mPreviewWindow->updateGui(deltaTime);

        mDockSpace->end();
    }
}
