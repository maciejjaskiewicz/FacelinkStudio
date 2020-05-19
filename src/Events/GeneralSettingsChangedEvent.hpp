#pragma once

#include <Xenon/Services/Event/Event.hpp>
#include <glm/vec4.hpp>

namespace Fls
{
    struct GeneralSettingsChangedEvent final : Xenon::Event
    {
        glm::vec4 backgroundColor;
        bool cameraEnabled;
        float cameraZoom;
        bool triggeredByCamera{ false }; //TODO: find better solution

        explicit GeneralSettingsChangedEvent(const glm::vec4 backgroundColor, const bool cameraEnabled, 
            const float cameraZoom, const bool triggeredByCamera = false)
            : backgroundColor(backgroundColor), cameraEnabled(cameraEnabled),
              cameraZoom(cameraZoom), triggeredByCamera(triggeredByCamera)
        { }
    };
}
