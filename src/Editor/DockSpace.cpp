#include "DockSpace.hpp"

#include <imgui_internal.h>

namespace Fls
{
    DockSpace::DockSpace(const std::string& name)
        : mDockSpaceName(name), mDockSpaceWindowName(name + "Window")
    { }

    void DockSpace::setInitialWindowsConfiguration(const Xenon::Action<ImGuiID> cfgCallback)
    {
        mWindowsConfigurationCallback = cfgCallback;
    }

    void DockSpace::begin()
    {
        mDockSpaceId = ImGui::GetID(mDockSpaceName.c_str());

        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        auto viewport = ImGui::GetMainViewport();

        ImGui::SetNextWindowPos(viewport->GetWorkPos());
        ImGui::SetNextWindowSize(viewport->GetWorkSize());
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);

        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        if (mDockSpaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
            windowFlags |= ImGuiWindowFlags_NoBackground;

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin(mDockSpaceWindowName.c_str(), &mDockSpaceWindowOpen, windowFlags);
        ImGui::PopStyleVar();
        ImGui::PopStyleVar(2);

        if(!mWindowsConfigured && mWindowsConfigurationCallback)
        {
            ImGui::DockBuilderRemoveNode(mDockSpaceId);
            ImGui::DockBuilderAddNode(mDockSpaceId, ImGuiDockNodeFlags_DockSpace);
            ImGui::DockBuilderSetNodeSize(mDockSpaceId, viewport->Size);

            mWindowsConfigurationCallback(mDockSpaceId);

            ImGui::DockBuilderFinish(mDockSpaceId);
            mWindowsConfigured = true;
        }

        ImGui::DockSpace(mDockSpaceId, ImVec2(0.0f, 0.0f), mDockSpaceFlags);
    }

    void DockSpace::end()
    {
        ImGui::End();
    }
}
