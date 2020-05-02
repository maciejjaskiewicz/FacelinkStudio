set(FLS_ROOT ${PROJECT_SOURCE_DIR}/src)

set (FLS_SOURCES
    ${FLS_ROOT}/FacelinkStudio.cpp
    ${FLS_ROOT}/FacelinkStudio.hpp
)

source_group("" FILES ${FLS_SOURCES})

add_executable(FacelinkStudio ${FLS_SOURCES})
target_link_libraries(FacelinkStudio PRIVATE
    Xenon::Options
    Xenon::Warnings
    Xenon::Engine
)

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
        ${PROJECT_BINARY_DIR}/assets/
    )
endif()