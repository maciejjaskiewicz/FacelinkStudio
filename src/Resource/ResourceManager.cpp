#include "ResourceManager.hpp"

#include "ResourcePreprocessor.hpp"
#include "Utils/Random.hpp"
#include "Events/OpenFileEvent.hpp"
#include "Events/ResourceSelectedEvent.hpp"

#include <Xenon/Core/ApplicationServices.hpp>
#include <filesystem>
#include <opencv2/imgcodecs.hpp>

namespace Fls
{
    void ResourceManager::init()
    {
        Xenon::ApplicationServices::EventBus::ref().subscribe<OpenFileEvent>(
        [this](const OpenFileEvent& event)
        {
            const std::filesystem::path path(event.path);

            //TODO: declare somewhere supported extensions
            if (path.extension() == ".jpg" || path.extension() == ".png")
            {
                load(path.string());
            }
        });

        Xenon::ApplicationServices::EventBus::ref().subscribe<ResourceSelectedEvent>(
            [this](const ResourceSelectedEvent& event)
        {
            select(event.id);
        });
    }

    void ResourceManager::load(const std::string& pathStr)
    {
        const std::filesystem::path path(pathStr);
        XN_ASSERT(std::filesystem::exists(path));

        const auto absolute = std::filesystem::absolute(path);

        UserResource resource{
            Random::generateHex(16),
             absolute.filename().string(),
            absolute.string()
        };

        const auto pixels = cv::imread(absolute.string(), cv::IMREAD_UNCHANGED);

        ResourcePreprocessor::preprocess(pixels, &resource);

        auto it = mResources.emplace(resource.id, resource).first;
        mSelectedResource = &it->second;

        mDirty = true;
    }

    void ResourceManager::select(const std::string& id)
    {
        const auto resourceIt = mResources.find(id);

        if(resourceIt != mResources.end())
        {
            mSelectedResource = &resourceIt->second;
        }
    }

    bool ResourceManager::dirty() const
    {
        return mDirty;
    }

    UserResource* ResourceManager::selectedResource() const
    {
        return mSelectedResource;
    }

    std::vector<UserResource*> ResourceManager::resources()
    {
        std::vector<UserResource*> resources;

        for(auto& res : mResources)
        {
            resources.push_back(&res.second);
        }

        mDirty = false;
        return resources;
    }
}
