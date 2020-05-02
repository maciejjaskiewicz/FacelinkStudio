#include "FacelinkStudio.hpp"

#include <Xenon/Graphics.hpp>

#include <imgui.h>
#include <glm/gtc/type_ptr.hpp>

namespace Fls
{
    FacelinkStudio::FacelinkStudio(const Xenon::ApplicationConfiguration& config)
        : Application(config)
    {
        Services::EventBus::ref().subscribe<Xenon::KeyReleasedEvent>(
            [this](const Xenon::KeyReleasedEvent& event)
        {
            if (event.keyCode == XN_KEY_ESCAPE)
            {
                XN_INFO("Closing from Facelink Studio");
                mRunning = false;
            }
        });
    }

    void FacelinkStudio::init()
    {
        XN_INFO("Facelink Studio initialized");
    }

    void FacelinkStudio::update(const Xenon::DeltaTime& deltaTime)
    {
        Xenon::RenderCmd::setClearColor(mClearColor);
        Xenon::RenderCmd::clear();
    }

    void FacelinkStudio::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        ImGui::ShowMetricsWindow();
    }
}