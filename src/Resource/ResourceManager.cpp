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

        auto loadFuture = std::async(std::launch::async, 
            [this, absolute]()
        {
            UserResource resource;

            resource.id = cv::getTickCount();
            resource.name = absolute.filename().string();
            resource.path = absolute.string();

            resource.pixels = cv::imread(absolute.string(), cv::IMREAD_COLOR);
            resource.gpuPixels = cv::cuda::GpuMat(resource.pixels);

            std::lock_guard<std::mutex> lock(mResourcesMutex);

            auto it = mResources.emplace(resource.id, resource).first;
            mSelectedResource.store(&it->second);

            mDirty.store(true);
        });

        mLoadFutures.push_back(std::move(loadFuture));
    }

    void ResourceManager::select(const int64& id)
    {
        std::lock_guard<std::mutex> lock(mResourcesMutex);

        const auto resourceIt = mResources.find(id);

        if(resourceIt != mResources.end())
        {
            mSelectedResource.store(&resourceIt->second);
        }
    }

    bool ResourceManager::dirty() const
    {
        return mDirty.load();
    }

    UserResource* ResourceManager::selectedResource() const
    {
        auto* selectedResource = mSelectedResource.load();

        if(selectedResource && !selectedResource->preprocessed)
        {
            ResourcePreprocessor::preprocess(selectedResource);
            selectedResource->preprocessed = true;
        }

        return mSelectedResource.load();
    }

    std::vector<UserResource*> ResourceManager::resources()
    {
        std::lock_guard<std::mutex> lock(mResourcesMutex);
        std::vector<UserResource*> resources;

        for(auto& res : mResources)
        {
            resources.push_back(&res.second);
        }

        mDirty.store(false);
        return resources;
    }
}
