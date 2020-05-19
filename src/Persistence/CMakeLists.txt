set(FLS_SRCROOT ${PROJECT_SOURCE_DIR}/src/Persistence)

add_subdirectory(Repository)

set (FLS_SOURCES_PERSISTENCE
    ${FLS_SRCROOT}/Database.cpp
    ${FLS_SRCROOT}/Database.hpp
    ${FLS_SRCROOT}/FlsDatabase.cpp
    ${FLS_SRCROOT}/FlsDatabase.hpp
    ${FLS_SRCROOT}/DatabaseTransaction.cpp
    ${FLS_SRCROOT}/DatabaseTransaction.hpp
    ${FLS_SRCROOT}/SqlStatement.cpp
    ${FLS_SRCROOT}/SqlStatement.hpp
    PARENT_SCOPE
)

set(FLS_SOURCES_PERSISTENCE_REPOSITORY
    ${FLS_SOURCES_PERSISTENCE_REPOSITORY_L}
    PARENT_SCOPE
)