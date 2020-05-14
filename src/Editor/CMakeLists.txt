set(FLS_SRCROOT ${PROJECT_SOURCE_DIR}/src/Editor)

set (FLS_SOURCES_EDITOR
    ${FLS_SRCROOT}/DockSpace.cpp
    ${FLS_SRCROOT}/DockSpace.hpp
    ${FLS_SRCROOT}/Editor.cpp
    ${FLS_SRCROOT}/Editor.hpp
    ${FLS_SRCROOT}/PreviewWindow.cpp
    ${FLS_SRCROOT}/PreviewWindow.hpp
    ${FLS_SRCROOT}/PreviewCameraController.cpp
    ${FLS_SRCROOT}/PreviewCameraController.hpp
    ${FLS_SRCROOT}/ResourceWindow.cpp
    ${FLS_SRCROOT}/ResourceWindow.hpp
    ${FLS_SRCROOT}/InspectorWindow.cpp
    ${FLS_SRCROOT}/InspectorWindow.hpp
    ${FLS_SRCROOT}/FacesWindow.cpp
    ${FLS_SRCROOT}/FacesWindow.hpp
    PARENT_SCOPE
)