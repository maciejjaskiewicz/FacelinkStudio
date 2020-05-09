#pragma once

#include "DockSpace.hpp"
#include "PreviewWindow.hpp"
#include "ResourceWindow.hpp"

#include <string>
#include <memory>

namespace Xenon {
    class DeltaTime;
    class Gui;
}

namespace Fls
{
    class Editor
    {
    public:
        static void init(Xenon::Gui& gui);
        static void shutdown();

        static void updateGui(const Xenon::DeltaTime& deltaTime);

        static std::unique_ptr<PreviewWindow> previewWindow;
        static std::unique_ptr<ResourceWindow> resourceWindow;
    private:
        static void drawMenuBar();
        static void updateFileDialog(const Xenon::DeltaTime& deltaTime);

        static void setCustomStyle(const std::string& fontPath, Xenon::Gui& gui);

        inline static Xenon::ResourceCache<Xenon::Shader> shaderCache;
        static std::unique_ptr<DockSpace> dockSpace;
    };
}