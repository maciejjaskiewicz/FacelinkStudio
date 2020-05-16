set(FLS_SRCROOT ${PROJECT_SOURCE_DIR}/src/Persistence)

set (FLS_SOURCES_PERSISTENCE
    ${FLS_SRCROOT}/Database.cpp
    ${FLS_SRCROOT}/Database.hpp
    ${FLS_SRCROOT}/FlsDatabase.cpp
    ${FLS_SRCROOT}/FlsDatabase.hpp
    ${FLS_SRCROOT}/SqlStatement.cpp
    ${FLS_SRCROOT}/SqlStatement.hpp
    PARENT_SCOPE
)
