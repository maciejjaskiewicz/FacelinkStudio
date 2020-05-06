#pragma once

#include <utility>

#include "Xenon/Services/Event/Event.hpp"

namespace Fls
{
    struct ResourceSelectedEvent final : Xenon::Event
    {
        std::string id;

        explicit ResourceSelectedEvent(std::string id)
            : id(std::move(id))
        { }
    };
}
