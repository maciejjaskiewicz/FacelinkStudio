#pragma once

#include "UserResource.hpp"

#include <unordered_map>

namespace Fls
{
    class ResourceManager
    {
    public:
        void init();

        void load(const std::string& pathStr);
        void select(const std::string& id);

        XN_NODISCARD bool dirty() const;
        XN_NODISCARD UserResource* selectedResource() const;
        XN_NODISCARD std::vector<UserResource*> resources();
    private:
        bool mDirty{ true };

        std::unordered_map<std::string, UserResource> mResources;
        UserResource* mSelectedResource{ nullptr };
    };
}
