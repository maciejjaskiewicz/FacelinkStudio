#include "FacelinkStudio.hpp"

#include "Editor/Editor.hpp"

#include <Xenon/Graphics.hpp>
#include <imgui.h>

namespace Fls
{
    FacelinkStudio::FacelinkStudio(const Xenon::ApplicationConfiguration& config)
        : Application(config)
    {
        mResourceManager = std::make_unique<ResourceManager>();

        Editor::init(*mGui);
    }

    FacelinkStudio::~FacelinkStudio()
    {
        Editor::shutdown();
    }

    void FacelinkStudio::init()
    {
        mResourceManager->init();

        mResourceManager->load("assets/textures/squirviel_hd.jpg");

        XN_INFO("Facelink Studio initialized");
    }

    void FacelinkStudio::update(const Xenon::DeltaTime& deltaTime)
    {
        Xenon::RenderCmd::setClearColor(mClearColor);
        Xenon::RenderCmd::clear();

        const auto* selectedResource = mResourceManager->selectedResource();
        if(selectedResource)
        {
            Editor::previewWindow->draw(selectedResource->frame);
        }

        if(mResourceManager->dirty())
            Editor::resourceWindow->setResources(mResourceManager->resources());
    }

    void FacelinkStudio::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        Editor::updateGui(deltaTime);
    }
}