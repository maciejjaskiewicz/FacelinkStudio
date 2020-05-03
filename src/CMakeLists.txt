set(FLS_ROOT ${PROJECT_SOURCE_DIR}/src)

add_subdirectory(Editor)

set (FLS_SOURCES_ROOT
    ${FLS_ROOT}/FacelinkStudio.cpp
    ${FLS_ROOT}/FacelinkStudio.hpp
)

source_group("" FILES ${FLS_SOURCES_ROOT})
source_group("Editor" FILES ${FLS_SOURCES_EDITOR})

set (FLS_SOURCES
    ${FLS_SOURCES_ROOT}
    ${FLS_SOURCES_EDITOR}
)

add_executable(FacelinkStudio ${FLS_SOURCES})
target_link_libraries(FacelinkStudio PRIVATE
    Xenon::Options
    Xenon::Warnings
    Xenon::Engine
)

include(Settings)
set_target_output(FacelinkStudio)

target_include_directories(FacelinkStudio PRIVATE ${FLS_ROOT})

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