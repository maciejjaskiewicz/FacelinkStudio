#pragma once

#include <Xenon/Utils/Delegate.hpp>

#include <imgui.h>
#include <string>

namespace Fls
{
    class DockSpace
    {
    public:
        explicit DockSpace(const std::string& name);
        void setInitialWindowsConfiguration(Xenon::Action<ImGuiID> cfgCallback);

        void begin();
        void end();
    private:
        ImGuiID mDockSpaceId{};
        std::string mDockSpaceName;
        std::string mDockSpaceWindowName;

        bool mDockSpaceWindowOpen{ true };
        bool mWindowsConfigured{ false };

        Xenon::Action<ImGuiID> mWindowsConfigurationCallback;
        ImGuiDockNodeFlags mDockSpaceFlags{ ImGuiDockNodeFlags_None };
    };
}
