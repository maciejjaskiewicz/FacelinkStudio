#include "PreviewCameraController.hpp"

#include "Events/PreviewCameraResetEvent.hpp"
#include "Events/GeneralSettingsChangedEvent.hpp"

#include <Xenon/Core/ApplicationServices.hpp>
#include <Xenon/Core/Events/MouseEvent.hpp>
#include <Xenon/Core/Input/KeyCode.hpp>
#include <Xenon/Core/Input/Input.hpp>

namespace Fls
{
    PreviewCameraController::PreviewCameraController(const float aspectRatio)
        : CameraController(aspectRatio)
    {
        const auto cameraConfiguration = calculateCameraProjConfiguration();
        mCamera = std::make_unique<Xenon::OrthographicCamera>(cameraConfiguration);

        Xenon::ApplicationServices::EventBus::ref().subscribe<Xenon::MouseScrolledEvent>(
        [this](const Xenon::MouseScrolledEvent& event)
        {
            if (mLocked) return;

            mZoomLevel -= event.yOffset * mZoomSpeed;
            mZoomLevel = std::min(mZoomLevel, mZoomMin);
            mZoomLevel = std::max(mZoomLevel, mZoomMax);
            mCamera->setProjection(calculateCameraProjConfiguration());

            mSettingsChanged = true;
        });

        Xenon::ApplicationServices::EventBus::ref().subscribe<GeneralSettingsChangedEvent>(
            [this](const GeneralSettingsChangedEvent& event)
        {
            if(!event.triggeredByCamera)
            {
                mZoomLevel = 1.0f / event.cameraZoom;
                mCamera->setProjection(calculateCameraProjConfiguration());

                mSettingsChanged = true;
            }
        });

        Xenon::ApplicationServices::EventBus::ref().subscribe<PreviewCameraResetEvent>(
            [this](const PreviewCameraResetEvent& event)
        {
            reset();
        });
    }

    void PreviewCameraController::setAspect(const float aspectRatio)
    {
        mAspectRatio = aspectRatio;
        mCamera->setProjection(calculateCameraProjConfiguration());
    }

    void PreviewCameraController::update(const Xenon::DeltaTime& deltaTime)
    {
        const auto& input = Xenon::ApplicationServices::Input::ref();

        const auto locked = !input.isKeyPressed(Xenon::KeyCode::LeftControl);

        if(mLocked != locked)
        {
            mSettingsChanged = true;
            mLocked = locked;
        }

        if (!mLocked)
        {
            const auto rotation = 0.0f;
            const auto speed = mCameraSpeed * mZoomLevel * deltaTime;

            if (input.isKeyPressed(Xenon::KeyCode::W))
            {
                mCameraPosition.x += static_cast<float>(-sin(glm::radians(rotation)) * speed);
                mCameraPosition.y += static_cast<float>(cos(glm::radians(rotation)) * speed);
            }

            if (input.isKeyPressed(Xenon::KeyCode::S))
            {
                mCameraPosition.x -= static_cast<float>(-sin(glm::radians(rotation)) * speed);
                mCameraPosition.y -= static_cast<float>(cos(glm::radians(rotation)) * speed);
            }

            if (input.isKeyPressed(Xenon::KeyCode::A))
            {
                mCameraPosition.x -= static_cast<float>(cos(glm::radians(rotation)) * speed);
                mCameraPosition.y -= static_cast<float>(sin(glm::radians(rotation)) * speed);
            }

            if (input.isKeyPressed(Xenon::KeyCode::D))
            {
                mCameraPosition.x += static_cast<float>(cos(glm::radians(rotation)) * speed);
                mCameraPosition.y += static_cast<float>(sin(glm::radians(rotation)) * speed);
            }

            mCamera->setPosition(mCameraPosition);
        }

        if(mSettingsChanged)
        {
            Xenon::ApplicationServices::EventBus::ref().trigger<GeneralSettingsChangedEvent>(
                glm::vec4(0.0f), !mLocked, 1.0f / mZoomLevel, true);
            mSettingsChanged = false;
        }
    }

    void PreviewCameraController::reset()
    {
        mZoomLevel = mZoomMin;
        mCameraPosition = glm::vec3(0.0f);

        mCamera->setPosition(mCameraPosition);
        mCamera->setProjection(calculateCameraProjConfiguration());

        mSettingsChanged = true;
    }

    const Xenon::Camera& PreviewCameraController::camera() const
    {
        return *mCamera;
    }

    Xenon::OrthographicCameraProjConfiguration PreviewCameraController::calculateCameraProjConfiguration() const
    {
        const Xenon::OrthographicCameraProjConfiguration cameraConfiguration
        {
            -mAspectRatio * mZoomLevel,
            mAspectRatio * mZoomLevel,
            -mZoomLevel,
            mZoomLevel
        };

        return cameraConfiguration;
    }
}
