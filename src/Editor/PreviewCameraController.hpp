#pragma once

#include <Xenon/Graphics/Camera/CameraController.hpp>
#include <Xenon/Graphics/Camera/OrthographicCamera.hpp>

namespace Fls
{
    class PreviewCameraController final : public Xenon::CameraController
    {
    public:
        explicit PreviewCameraController(float aspectRatio);

        void setAspect(float aspectRatio);
        void update(const Xenon::DeltaTime& deltaTime) override;
        void reset();

        XN_NODISCARD const Xenon::Camera& camera() const override;
    protected:
        float mZoomLevel{ 1.0f }, mZoomSpeed{ 0.1f }, mZoomMin{ 1.0f }, mZoomMax{ 0.2f };
        glm::vec3 mCameraPosition{ 0.0f, 0.0f, 0.0f };

        float mCameraSpeed{ 3.0f };
        float mCameraRotationSpeed{ 180.0f };

        bool mSettingsChanged{ false };
        bool mLocked{ true };

        std::unique_ptr<Xenon::OrthographicCamera> mCamera;
    private:
        XN_NODISCARD Xenon::OrthographicCameraProjConfiguration calculateCameraProjConfiguration() const;
    };
}
