set(FLS_SRCROOT ${PROJECT_SOURCE_DIR}/src/Resource)

set (FLS_SOURCES_RESOURCE
    ${FLS_SRCROOT}/UserResource.hpp
    ${FLS_SRCROOT}/ResourceManager.cpp
    ${FLS_SRCROOT}/ResourceManager.hpp
    ${FLS_SRCROOT}/ResourcePreprocessor.cpp
    ${FLS_SRCROOT}/ResourcePreprocessor.hpp
    PARENT_SCOPE
)

set (FLS_SOURCES_RESOURCES_EVENTS
    ${FLS_SOURCES_RESOURCES_EVENTS_L}
    PARENT_SCOPE
)