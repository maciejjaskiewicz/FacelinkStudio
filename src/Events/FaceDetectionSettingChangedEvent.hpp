#pragma once

#include "Xenon/Services/Event/Event.hpp"

#include <glm/vec4.hpp>

namespace Fls
{
    struct FaceDetectionSettingChangedEvent final : Xenon::Event
    {
        bool showDetectionBox;
        float confidenceThreshold;
        float outlineThickness;
        glm::vec4 outlineColor;
        glm::vec4 fillColor;

        explicit FaceDetectionSettingChangedEvent(const bool showDetectionBox, const float confidenceThreshold, 
            const float outlineThickness, const glm::vec4 outlineColor, const glm::vec4 fillColor)
            : showDetectionBox(showDetectionBox), confidenceThreshold(confidenceThreshold), outlineThickness(outlineThickness),
              outlineColor(outlineColor), fillColor(fillColor)
        { }
    };
}
