#include "FaceModel.hpp"

#include "Utils/DataUtils.hpp"

namespace Fls
{
    std::shared_ptr<Xenon::Texture2D> FaceModel::texture()
    {
        if (mTexture) 
            return mTexture;

        mTexture = DataUtils::imgToTexture(image);

        return mTexture;
    }

    void FaceModel::setTexture(const std::shared_ptr<Xenon::Texture2D>& texture)
    {
        mTexture = texture;
;   }
}
