#pragma once

#include <Xenon/Xenon.hpp>
#include <glm/glm.hpp>

#include "Resource/ResourceManager.hpp"
#include "Detectors/FaceDetector.hpp"
#include "Detectors/FaceAligner.hpp"
#include "Detectors/FaceEmbedder.hpp"
#include "Detectors/FaceClassifier.hpp"

namespace Fls
{
    class FacelinkStudio final : public Xenon::Application
    {
    public:
        explicit FacelinkStudio(const Xenon::ApplicationConfiguration& config);
        ~FacelinkStudio();

        void init() override;

        void update(const Xenon::DeltaTime& deltaTime) override;
        void updateGui(const Xenon::DeltaTime& deltaTime) override;

    private:
        glm::vec4 mClearColor{ 0.15f, 0.15f, 0.15f, 1.0f };

        std::unique_ptr<ResourceManager> mResourceManager;
        std::unique_ptr<FaceDetector> mFaceDetector;
        std::unique_ptr<FaceAligner> mFaceAligner;
        std::unique_ptr<FaceEmbedder> mFaceEmbedder;
        std::unique_ptr<FaceClassifier> mFaceClassifier;
    };
}
XN_REGISTER_APPLICATION(Fls::FacelinkStudio);

inline void Xenon::configureApplication(IApplicationBuilder<Fls::FacelinkStudio>& applicationBuilder)
{
    applicationBuilder.configureApplicationWindow([](ApplicationWindowConfiguration& cfg)
    {
        cfg.setTitle("Facelink Studio");
        cfg.maximize(true);
        cfg.setResolution(StandardWindowResolution::A_16X9_R_1280X720);
        cfg.setVSync(true);
    });

    applicationBuilder.configureApplicationLogger([](ApplicationLoggerConfiguration& cfg)
    {
        cfg.setLabel("FacelinkStudio");
        cfg.useConsoleLogger();
        cfg.useFileLogger("FacelinkStudio.log", true);
    });

    applicationBuilder.configureApplicationGui([](ApplicationGuiConfiguration& cfg)
    {
        cfg.setStyle(GuiStyle::Dark);
        cfg.useDocking();
    });
}