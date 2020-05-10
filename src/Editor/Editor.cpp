#include "Editor.hpp"

#include "Events/OpenFileEvent.hpp"

#include <Xenon/Graphics.hpp>
#include <Xenon/Core/Gui/Gui.hpp>
#include <Xenon/Core/ApplicationServices.hpp>
#include <Xenon/Core/Events/WindowEvent.hpp>

#include <imgui.h>
#include <imgui_internal.h>
#include <ImGuiFileDialog.hpp>

namespace Fls
{
    std::unique_ptr<DockSpace> Editor::dockSpace = std::make_unique<DockSpace>("FacelinkStudioDockSpace");
    std::unique_ptr<PreviewWindow> Editor::previewWindow = std::make_unique<PreviewWindow>();
    std::unique_ptr<ResourceWindow> Editor::resourceWindow = std::make_unique<ResourceWindow>();
    std::unique_ptr<InspectorWindow> Editor::inspectorWindow = std::make_unique<InspectorWindow>();

    void Editor::init(Xenon::Gui& gui)
    {
        setCustomStyle("assets/fonts/Ruda-Bold.ttf", gui);

        shaderCache.set<Xenon::ShaderLoader>("flat", Xenon::ShaderLoaderArgs(
            "assets/shaders/FlatColorVertex.glsl",
            "assets/shaders/FlatColorFragment.glsl"
        ));

        shaderCache.set<Xenon::ShaderLoader>("texture", Xenon::ShaderLoaderArgs(
            "assets/shaders/TextureVertex.glsl",
            "assets/shaders/TextureFragment.glsl"
        ));

        shaderCache.set<Xenon::ShaderLoader>("detectionBox", Xenon::ShaderLoaderArgs(
            "assets/shaders/DetectionBoxVertex.glsl",
            "assets/shaders/DetectionBoxFragment.glsl"
        ));

        dockSpace->setInitialWindowsConfiguration([](const ImGuiID mainNodeId)
        {
            ImGuiID rightNodeId, rightDownNodeId;

            const auto leftNodeId = ImGui::DockBuilderSplitNode(mainNodeId, ImGuiDir_Left,
                0.75f, nullptr, &rightNodeId);
            const auto rightUpNodeId = ImGui::DockBuilderSplitNode(rightNodeId, ImGuiDir_Up,
                0.65f, nullptr, &rightDownNodeId);

            ImGui::DockBuilderDockWindow("Preview", leftNodeId);
            ImGui::DockBuilderDockWindow("Inspector", rightUpNodeId);
            ImGui::DockBuilderDockWindow("Resources", rightDownNodeId);
        });

        previewWindow->init(shaderCache);
        inspectorWindow->init();
    }

    void Editor::shutdown()
    {
        previewWindow.reset();
        resourceWindow.reset();
        inspectorWindow.reset();

        dockSpace.reset();

        shaderCache.clear();
    }

    void Editor::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        drawMenuBar();

        dockSpace->begin();

        //ImGui::ShowMetricsWindow();
        //ImGui::ShowDemoWindow();

        previewWindow->updateGui(deltaTime);
        resourceWindow->updateGui(deltaTime);
        inspectorWindow->updateGui(deltaTime);

        dockSpace->end();

        updateFileDialog(deltaTime);
    }

    void Editor::drawMenuBar()
    {
        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(1.0f, 5.0f));

        if (ImGui::BeginMainMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open File"))
                {
                    igfd::ImGuiFileDialog::Instance()->OpenModal("ChooseFileDlgKey", "Choose File", 
                        ".jpg\0.png\0\0", ".");
                }

                if(ImGui::MenuItem("Exit"))
                {
                    Xenon::ApplicationServices::EventBus::ref().enqueue<Xenon::WindowCloseEvent>();
                }

                ImGui::EndMenu();
            }

            if (ImGui::MenuItem("Database"))
            {
            }

            if (ImGui::MenuItem("About"))
            {
            }

            ImGui::EndMainMenuBar();
        }

        ImGui::PopStyleVar();
    }

    void Editor::updateFileDialog(const Xenon::DeltaTime& deltaTime)
    {
        const auto fileDialogOpen = igfd::ImGuiFileDialog::Instance()->FileDialog("ChooseFileDlgKey",
            ImGuiWindowFlags_NoCollapse, ImVec2(500, 300));

        if (fileDialogOpen)
        {
            if (igfd::ImGuiFileDialog::Instance()->IsOk == true)
            {
                auto path = igfd::ImGuiFileDialog::Instance()->GetFilepathName();

                Xenon::ApplicationServices::EventBus::ref().enqueue<OpenFileEvent>(path);
            }

            igfd::ImGuiFileDialog::Instance()->CloseDialog("ChooseFileDlgKey");
        }
    }

    void Editor::setCustomStyle(const std::string& fontPath, Xenon::Gui& gui)
    {
        auto& io = ImGui::GetIO();

        auto* font = io.Fonts->AddFontFromFileTTF(fontPath.c_str(), 15.0f);
        gui.setFont(font);

        ImGui::GetStyle().FrameRounding = 4.0f;
        ImGui::GetStyle().GrabRounding = 4.0f;

        ImVec4* colors = ImGui::GetStyle().Colors;
        colors[ImGuiCol_Text] = ImVec4(0.95f, 0.96f, 0.98f, 1.00f);
        colors[ImGuiCol_TextDisabled] = ImVec4(0.36f, 0.42f, 0.47f, 1.00f);
        colors[ImGuiCol_WindowBg] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_ChildBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
        colors[ImGuiCol_Border] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_BorderShadow] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.12f, 0.20f, 0.28f, 1.00f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.09f, 0.12f, 0.14f, 1.00f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.12f, 0.14f, 0.65f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.08f, 0.10f, 0.12f, 1.00f);
        colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
        colors[ImGuiCol_MenuBarBg] = ImVec4(0.15f, 0.18f, 0.22f, 1.00f);
        colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.39f);
        colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.18f, 0.22f, 0.25f, 1.00f);
        colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.09f, 0.21f, 0.31f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.37f, 0.61f, 1.00f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.56f, 1.00f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.53f, 0.98f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.20f, 0.25f, 0.29f, 0.55f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_Separator] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
        colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
        colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.59f, 0.98f, 0.25f);
        colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.67f);
        colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.59f, 0.98f, 0.95f);
        colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.59f, 0.98f, 0.80f);
        colors[ImGuiCol_TabActive] = ImVec4(0.20f, 0.25f, 0.29f, 1.00f);
        colors[ImGuiCol_TabUnfocused] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.11f, 0.15f, 0.17f, 1.00f);
        colors[ImGuiCol_PlotLines] = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
        colors[ImGuiCol_PlotLinesHovered] = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
        colors[ImGuiCol_PlotHistogram] = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
        colors[ImGuiCol_PlotHistogramHovered] = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
        colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
        colors[ImGuiCol_DragDropTarget] = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
        colors[ImGuiCol_NavHighlight] = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
        colors[ImGuiCol_NavWindowingDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
        colors[ImGuiCol_ModalWindowDimBg] = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);
    }
}
