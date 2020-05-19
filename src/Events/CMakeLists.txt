set(FLS_SRCROOT ${PROJECT_SOURCE_DIR}/src/Events)

set (FLS_SOURCES_EVENTS
    ${FLS_SRCROOT}/OpenFileEvent.hpp
    ${FLS_SRCROOT}/ResourceSelectedEvent.hpp
    ${FLS_SRCROOT}/GeneralSettingsChangedEvent.hpp
    ${FLS_SRCROOT}/PreviewCameraResetEvent.hpp
    ${FLS_SRCROOT}/FaceDetectionSettingChangedEvent.hpp
    ${FLS_SRCROOT}/FaceRecognitionSettingChangedEvent.hpp
    ${FLS_SRCROOT}/FaceSelectedEvent.hpp
    ${FLS_SRCROOT}/OpenDatabaseEvent.hpp
    ${FLS_SRCROOT}/RetrainFaceClassifierEvent.hpp
    PARENT_SCOPE
)