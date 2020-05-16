set(FLS_SRCROOT ${PROJECT_SOURCE_DIR}/src/Detectors)

set (FLS_SOURCES_DETECTORS
    ${FLS_SRCROOT}/FaceDetector.cpp
    ${FLS_SRCROOT}/FaceDetector.hpp
    ${FLS_SRCROOT}/FaceDetectionResult.hpp
    ${FLS_SRCROOT}/FaceAligner.cpp
    ${FLS_SRCROOT}/FaceAligner.hpp
    ${FLS_SRCROOT}/FaceAlignmentResult.hpp
    ${FLS_SRCROOT}/FaceEmbedder.cpp
    ${FLS_SRCROOT}/FaceEmbedder.hpp
    ${FLS_SRCROOT}/FaceClassifier.cpp
    ${FLS_SRCROOT}/FaceClassifier.hpp
    ${FLS_SRCROOT}/FaceClassificationResult.hpp
    PARENT_SCOPE
)
