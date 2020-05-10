set(FLS_ROOT ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(Editor)
add_subdirectory(Resource)
add_subdirectory(Events)
add_subdirectory(Utils)
add_subdirectory(Detectors)

set (FLS_SOURCES_ROOT
    ${FLS_ROOT}/FacelinkStudio.cpp
    ${FLS_ROOT}/FacelinkStudio.hpp
)

source_group("" FILES ${FLS_SOURCES_ROOT})
source_group("Editor" FILES ${FLS_SOURCES_EDITOR})
source_group("Resource" FILES ${FLS_SOURCES_RESOURCE})
source_group("Events" FILES ${FLS_SOURCES_EVENTS})
source_group("Utils" FILES ${FLS_SOURCES_UTILS})
source_group("Detectors" FILES ${FLS_SOURCES_DETECTORS})

set (FLS_SOURCES
    ${FLS_SOURCES_ROOT}
    ${FLS_SOURCES_EDITOR}
    ${FLS_SOURCES_RESOURCE}
    ${FLS_SOURCES_EVENTS}
    ${FLS_SOURCES_UTILS}
    ${FLS_SOURCES_DETECTORS}
)

add_executable(FacelinkStudio ${FLS_SOURCES})
target_link_libraries(FacelinkStudio PRIVATE
    Xenon::Options
    Xenon::Warnings
    Xenon::Engine
    ${CUDA_LIBRARIES}
    ${OpenCV_LIBS}
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