set(FLS_ROOT ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(Editor)
add_subdirectory(Resource)
add_subdirectory(Events)
add_subdirectory(Utils)
add_subdirectory(Detectors)
add_subdirectory(Persistence)

set (FLS_SOURCES_ROOT
    ${FLS_ROOT}/FacelinkStudio.cpp
    ${FLS_ROOT}/FacelinkStudio.hpp
    ${FLS_ROOT}/FacelinkStudioServices.hpp
)

source_group("" FILES ${FLS_SOURCES_ROOT})
source_group("Editor" FILES ${FLS_SOURCES_EDITOR})
source_group("Resource" FILES ${FLS_SOURCES_RESOURCE})
source_group("Events" FILES ${FLS_SOURCES_EVENTS})
source_group("Utils" FILES ${FLS_SOURCES_UTILS})
source_group("Detectors" FILES ${FLS_SOURCES_DETECTORS})
source_group("Persistence" FILES ${FLS_SOURCES_PERSISTENCE})
source_group("Persistence\\Repository" FILES ${FLS_SOURCES_PERSISTENCE_REPOSITORY})

set (FLS_SOURCES
    ${FLS_SOURCES_ROOT}
    ${FLS_SOURCES_EDITOR}
    ${FLS_SOURCES_RESOURCE}
    ${FLS_SOURCES_EVENTS}
    ${FLS_SOURCES_UTILS}
    ${FLS_SOURCES_DETECTORS}
    ${FLS_SOURCES_PERSISTENCE}
    ${FLS_SOURCES_PERSISTENCE_REPOSITORY}
)

add_executable(FacelinkStudio ${FLS_SOURCES})

target_link_libraries(FacelinkStudio PRIVATE
    Xenon::Options
    Xenon::Engine
    ${CUDA_LIBRARIES}
)

include(LinkLibraries)
target_link_libraries_sys_private(FacelinkStudio
    ${OpenCV_LIBS}
    dlib::dlib
)

include(Settings)
set_target_output(FacelinkStudio)

target_include_directories(FacelinkStudio PRIVATE ${FLS_ROOT})
target_include_directories(FacelinkStudio PRIVATE ${CUDA_INCLUDE_DIRS})

add_custom_command(TARGET FacelinkStudio PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${PROJECT_SOURCE_DIR}/assets/
    $<TARGET_FILE_DIR:FacelinkStudio>/assets/
)

if(NOT $<TARGET_FILE_DIR:FacelinkStudio> STREQUAL ${PROJECT_BINARY_DIR})
    add_custom_command(TARGET FacelinkStudio PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${PROJECT_SOURCE_DIR}/assets/
        ${PROJECT_BINARY_DIR}/src/assets/
    )
endif()