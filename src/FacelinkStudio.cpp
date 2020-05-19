#include "FacelinkStudio.hpp"

#include "FacelinkStudioServices.hpp"
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
        mFaceAligner = std::make_unique<FaceAligner>();
        mFaceEmbedder = std::make_unique<FaceEmbedder>();
        mFaceClassifier = std::make_unique<FaceClassifier>();
    }

    FacelinkStudio::~FacelinkStudio()
    {
        Editor::shutdown();
    }

    void FacelinkStudio::init()
    {
        FlsServices::Database::createAndSet();
        FlsServices::Database::ref().open("fls.db");
        FlsServices::Database::ref().ensureCreated();

        Editor::init(*mGui);      

        mResourceManager->init();
        mFaceDetector->init(
            "assets/models/res10_300x300_ssd_iter_140000.prototxt",
            "assets/models/res10_300x300_ssd_iter_140000.caffemodel"
        );
        mFaceAligner->init("assets/models/shape_predictor_68_face_landmarks.dat");
        mFaceEmbedder->init("assets/models/openface_nn4.small2.v1.t7");
        mFaceClassifier->init(FlsServices::Database::get());

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
            const auto classificationResult = mFaceClassifier->classify(selectedResource, embeddingResult);

            Editor::previewWindow->drawFrame(selectedResource->id, selectedResource->frame);
            Editor::previewWindow->drawDetectionResult(detectionResult);
            Editor::previewWindow->drawAlignmentResult(alignmentResult);

            Editor::facesWindow->setFaces(selectedResource->id, alignmentResult, embeddingResult, classificationResult);
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
