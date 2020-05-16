#include "FacelinkStudio.hpp"

#include "Editor/Editor.hpp"

#include <Xenon/Graphics.hpp>
#include <imgui.h>

namespace Fls
{
    FacelinkStudio::FacelinkStudio(const Xenon::ApplicationConfiguration& config)
        : Application(config)
    {
        mDatabase = std::make_unique<FlsDatabase>();
        mResourceManager = std::make_unique<ResourceManager>();
        mFaceDetector = std::make_unique<FaceDetector>();
        mFaceAligner = std::make_unique<FaceAligner>();
        mFaceEmbedder = std::make_unique<FaceEmbedder>();
    }

    FacelinkStudio::~FacelinkStudio()
    {
        Editor::shutdown();
    }

    void FacelinkStudio::init()
    {
        Editor::init(*mGui);

        mDatabase->open("fls.db");
        mDatabase->ensureCreated();

        mResourceManager->init();
        mFaceDetector->init(
            "assets/models/res10_300x300_ssd_iter_140000.prototxt",
            "assets/models/res10_300x300_ssd_iter_140000.caffemodel"
        );
        mFaceAligner->init("assets/models/shape_predictor_68_face_landmarks.dat");
        mFaceEmbedder->init("assets/models/openface_nn4.small2.v1.t7");

        mResourceManager->load("assets/textures/nasa_crew.jpg");

        XN_INFO("Facelink Studio initialized");
    }

    void FacelinkStudio::update(const Xenon::DeltaTime& deltaTime)
    {
        Xenon::RenderCmd::setClearColor(mClearColor);
        Xenon::RenderCmd::clear();

        Editor::previewWindow->begin(deltaTime);

        const auto* selectedResource = mResourceManager->selectedResource();
        if(selectedResource)
        {
            const auto detectionResult = mFaceDetector->detect(selectedResource);
            const auto alignmentResult = mFaceAligner->align(selectedResource, detectionResult);
            const auto embeddingResult = mFaceEmbedder->embed(selectedResource, alignmentResult);

            Editor::previewWindow->drawFrame(selectedResource->id, selectedResource->frame);
            Editor::previewWindow->drawDetectionResult(detectionResult);
            Editor::previewWindow->drawAlignmentResult(alignmentResult);

            Editor::facesWindow->setFaces(selectedResource->id, alignmentResult);
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