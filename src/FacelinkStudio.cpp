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
        mFaceDetector = std::make_unique<FaceDetector>();
    }

    FacelinkStudio::~FacelinkStudio()
    {
        Editor::shutdown();
    }

    void FacelinkStudio::init()
    {
        Editor::init(*mGui);

        mResourceManager->init();
        mFaceDetector->init(
            "assets/models/res10_300x300_ssd_iter_140000.prototxt",
            "assets/models/res10_300x300_ssd_iter_140000.caffemodel"
        );

        mResourceManager->load("assets/textures/nasa_crew.jpg");

        XN_INFO("Facelink Studio initialized");
    }

    void FacelinkStudio::update(const Xenon::DeltaTime& deltaTime)
    {
        Xenon::RenderCmd::setClearColor(mClearColor);
        Xenon::RenderCmd::clear();

        Editor::previewWindow->begin();

        const auto* selectedResource = mResourceManager->selectedResource();
        if(selectedResource)
        {
            const auto detectionResult = mFaceDetector->detect(selectedResource);

            Editor::previewWindow->drawFrame(selectedResource->frame);
            Editor::previewWindow->drawDetectionResult(detectionResult);            
        }

        Editor::previewWindow->end();

        if(mResourceManager->dirty())
            Editor::resourceWindow->setResources(mResourceManager->resources());
    }

    void FacelinkStudio::updateGui(const Xenon::DeltaTime& deltaTime)
    {
        Editor::updateGui(deltaTime);
    }
}