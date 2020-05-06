#include "ResourceWindow.hpp"

#include "Events/ResourceSelectedEvent.hpp"

#include <Xenon/Core/ApplicationServices.hpp>

#include <imgui.h>
#include <ImGuiFileDialog.hpp>
#include <filesystem>

namespace Fls
{
    ResourceWindow::ResourceWindow() = default;

    void ResourceWindow::setResources(const std::vector<UserResource*>& resources)
    {
        mResources = resources;
    }

    void ResourceWindow::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        drawWindow(deltaTime);
    }

    void ResourceWindow::drawWindow(const Xenon::DeltaTime& deltaTime)
    {
        ImGui::Begin("Resources");

        const auto contentWidth = ImGui::GetContentRegionAvailWidth();
        const auto contentHeight = ImGui::GetContentRegionAvail().y;

        ImGui::BeginChild("##ContentRegion", { contentWidth, contentHeight }, true);

        const auto itemSize = 96.0f;

        auto columns = static_cast<int>(contentWidth / itemSize);
        columns = columns < 1 ? 1 : columns;
        ImGui::Columns(columns, nullptr, false);

        uint32_t idx{ 0 };

        for (const auto* res : mResources)
        {
            if (!res || !res->thumbnail)
                continue;

            const auto thumbWidth = static_cast<float>(res->thumbnail->width());
            const auto thumbHeight = static_cast<float>(res->thumbnail->height());

            const auto maxDimension = std::max(res->thumbnail->width(), res->thumbnail->height());
            const auto aspectRatio = itemSize / maxDimension;

            const auto textureWidth = res->thumbnail->width() * aspectRatio;
            const auto textureHeight = res->thumbnail->height() * aspectRatio;

            ImGui::PushID(idx);

            auto* texturePtr = reinterpret_cast<void*>(static_cast<intptr_t>(res->thumbnail->id()));

            ImGui::BeginGroup();

            ImGui::ImageButton(texturePtr, { textureWidth - 20, textureHeight - 16 });

            if (ImGui::IsItemHovered())
            {
                ImGui::BeginTooltip();
                ImGui::Image(texturePtr, { thumbWidth, thumbHeight });
                ImGui::TextWrapped(res->path.c_str());
                ImGui::EndTooltip();
            }

            if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
            {
                Xenon::ApplicationServices::EventBus::ref().enqueue<ResourceSelectedEvent>(res->id);
            }

            ImGui::TextWrapped(res->name.c_str());

            ImGui::EndGroup();

            ImGui::PopID();
            ImGui::NextColumn();
            idx++;
        }

        ImGui::EndChild();

        ImGui::End();
    }
}
