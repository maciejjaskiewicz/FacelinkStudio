#pragma once

#include "Xenon/Services/Event/Event.hpp"

namespace Fls
{
    struct PreviewCameraResetEvent final : Xenon::Event
    {
        explicit PreviewCameraResetEvent() = default;
    };
}