#pragma once

#include <Xenon/Graphics.hpp>
#include "Resource/UserResource.hpp"

namespace Fls
{
    class ResourceWindow
    {
    public:
        explicit ResourceWindow();

        void setResources(const std::vector<UserResource*>& resources);

        void updateGui(const Xenon::DeltaTime& deltaTime);
    private:
        void drawWindow(const Xenon::DeltaTime& deltaTime);

        std::vector<UserResource*> mResources;
    };
}
