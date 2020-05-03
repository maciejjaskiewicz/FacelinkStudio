#pragma once

#include <Xenon/Xenon.hpp>
#include <glm/glm.hpp>

#include "Editor/DockSpace.hpp"
#include "Editor/PreviewWindow.hpp"

namespace Fls
{
    class FacelinkStudio final : public Xenon::Application
    {
    public:
        explicit FacelinkStudio(const Xenon::ApplicationConfiguration& config);

        void init() override;

        void update(const Xenon::DeltaTime& deltaTime) override;
        void updateGui(const Xenon::DeltaTime& deltaTime) override;

    private:
        std::unique_ptr<DockSpace> mDockSpace;
        std::unique_ptr<PreviewWindow> mPreviewWindow;

        glm::vec4 mClearColor{ 0.15f, 0.15f, 0.15f, 1.0f };

        Xenon::ResourceCache<Xenon::Texture2D> mTextureCache;
        std::shared_ptr<Xenon::Texture2D> mTexture;
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
        cfg.setVSync(false);
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