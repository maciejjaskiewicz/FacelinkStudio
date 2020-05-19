#pragma once

#include <Xenon/Services/Event/Event.hpp>

namespace Fls
{
    struct ResourceSelectedEvent final : Xenon::Event
    {
        int64 id;

        explicit ResourceSelectedEvent(const int64 id)
            : id(id)
        { }
    };
}
