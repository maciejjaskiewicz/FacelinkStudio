#pragma once

#include "UserResource.hpp"

#include <unordered_map>
#include <future>

namespace Fls
{
    class ResourceManager
    {
    public:
        void init();

        void load(const std::string& pathStr);
        void select(const int64& id);

        XN_NODISCARD bool dirty() const;
        XN_NODISCARD UserResource* selectedResource() const;
        XN_NODISCARD std::vector<UserResource*> resources();
    private:
        std::atomic_bool mDirty{ true };

        std::unordered_map<int64, UserResource> mResources;
        std::atomic<UserResource*> mSelectedResource{ nullptr };

        std::mutex mResourcesMutex;
        std::list<std::future<void>> mLoadFutures;
    };
}
