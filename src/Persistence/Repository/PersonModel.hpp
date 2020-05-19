#pragma once

#include "EntityModel.hpp"
#include "FaceModel.hpp"

#include <string>
#include <vector>

namespace Fls
{
    struct PersonModel : EntityModel
    {
        std::string name;
        std::string birthdate;
        std::string note;
        std::vector<std::shared_ptr<FaceModel>> faces;
    };

    struct PersonGridModel
    {
        int id{ -1 };
        std::string name;
        std::string birthdate;
        std::string note;
        int faces{ 0 };
        std::string createdDate;
    };
}
