#pragma once

#include "Persistence/FlsDatabase.hpp"

#include <Xenon/Utils/ImmutableServiceLocator.hpp>

namespace Fls
{
    struct FlsServices
    {
        using Database = Xenon::ImmutableServiceLocator<Fls::FlsDatabase>;
    };
}
