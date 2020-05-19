#pragma once

#include "Xenon/Core/Api.hpp"

namespace Fls
{
    struct EntityModel
    {
        int id{ -1 };

        XN_NODISCARD bool isNew() const
        {
            return id == -1;
        }
    };
}
