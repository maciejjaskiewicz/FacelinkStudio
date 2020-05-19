#pragma once

#include "Persistence/Repository/FaceModel.hpp"

#include <Xenon/Services/Event/Event.hpp>

namespace Fls
{
    struct FaceSelectedEvent final : Xenon::Event
    {
        std::shared_ptr<FaceModel> faceModel;

        explicit FaceSelectedEvent(const std::shared_ptr<FaceModel>& faceModel)
            : faceModel(faceModel)
        { }
    };
}