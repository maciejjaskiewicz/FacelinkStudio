#pragma once

#include <Xenon/Services/Event/Event.hpp>
#include <glm/vec4.hpp>

namespace Fls
{
    struct FaceRecognitionSettingChangedEvent final : Xenon::Event
    {
        float confidenceThreshold;
        bool showLandmarks;
        float landmarkSize;
        glm::vec4 landmarkColor;

        explicit FaceRecognitionSettingChangedEvent(const float confidenceThreshold, const bool showLandmarks, 
            const float landmarkSize, const glm::vec4 landmarkColor)
            : confidenceThreshold(confidenceThreshold), showLandmarks(showLandmarks),
              landmarkSize(landmarkSize), landmarkColor(landmarkColor)
        { }
    };
}
