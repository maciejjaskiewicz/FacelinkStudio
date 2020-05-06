#pragma once

#include "Xenon/Services/Event/Event.hpp"

namespace Fls
{
    struct OpenFileEvent final : Xenon::Event
    {
        std::string path;

        explicit OpenFileEvent(std::string path)
            : path(std::move(path))
        { }
    };
}