#include "FaceAligner.hpp"

#include "Resource/UserResource.hpp"

#include <dlib/opencv.h>

namespace Fls
{
    void FaceAligner::init(const std::string& landmarksShapePredictorPath)
    {
        mInitializedFuture = std::async(std::launch::async,
            [this, landmarksShapePredictorPath]()
        {
            dlib::deserialize(landmarksShapePredictorPath) >> mShapePredictor;
            mInitialized.store(true);
        });
    }

    std::vector<std::shared_ptr<FaceAlignmentResult>> FaceAligner::align(const UserResource* userResource,
        const std::vector<FaceDetectionResult>& detectedFaces)
    {
        std::vector<std::shared_ptr<FaceAlignmentResult>> result;

        if (!mInitialized.load() || !userResource || userResource->pixels.empty())
            return result;

        if(mLastFrameId != userResource->id)
        {
            dlib::array2d<dlib::rgb_pixel> dlibImage;
            dlib::assign_image(dlibImage, dlib::cv_image<dlib::bgr_pixel>(userResource->pixels));

            std::vector<dlib::chip_details> chipsDetails;

            for (const auto& face : detectedFaces)
            {
                auto faceAlignmentResult = std::make_shared<FaceAlignmentResult>();

                dlib::rectangle faceRectangle(dlib::point(face.x1, face.y1), dlib::point(face.x2, face.y2));
                auto shape = mShapePredictor(dlibImage, faceRectangle);

                faceAlignmentResult->srcFaceSize = glm::vec2(face.x2 - face.x1, face.y2 - face.y1);

                chipsDetails.emplace_back(dlib::get_face_chip_details(shape, 200, 0.2));

                for (uint32_t i = 0; i < shape.num_parts(); i++)
                {
                    const auto landmark = shape.part(i);
                    faceAlignmentResult->landmarks.emplace_back(landmark.x(), landmark.y());
                }

                result.emplace_back(faceAlignmentResult);
            }

            dlib::array<dlib::array2d<dlib::bgr_pixel>, dlib::default_memory_manager> chips;
            dlib::extract_image_chips(dlibImage, chipsDetails, chips);

            uint32_t chipIdx{};
            for (auto& chip : chips)
            {
                result[chipIdx]->alignedFace = std::move(chip);
                chipIdx++;
            }

            chips.clear();

            mLastFrameId = userResource->id;
            mLastFrameAlignmentResult = result;

            return result;
        }

        return mLastFrameAlignmentResult;
    }
}