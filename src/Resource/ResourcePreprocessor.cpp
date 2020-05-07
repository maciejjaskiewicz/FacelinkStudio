#include "ResourcePreprocessor.hpp"

#include <opencv2/core.hpp>
#include <opencv2/core/opengl.hpp>
#include <opencv2/cudawarping.hpp>
#include <opencv2/cudaimgproc.hpp>
#include <opencv2/cudaarithm.hpp>

namespace Fls
{
    void ResourcePreprocessor::preprocess(const cv::Mat& pixels, UserResource* userResource)
    {
        cv::ogl::Buffer pixelBuffer(pixels, cv::ogl::Buffer::PIXEL_UNPACK_BUFFER, true);

        cv::cuda::GpuMat dPixels = pixelBuffer.mapDevice();
        cv::cuda::GpuMat dRgbPixels, dThumbnail;

        pixelBuffer.channels() == 3
            ? cv::cuda::cvtColor(dPixels, dRgbPixels, cv::COLOR_BGR2RGB)
            : cv::cuda::cvtColor(dPixels, dRgbPixels, cv::COLOR_BGRA2RGBA);

        cv::cuda::flip(dRgbPixels, dPixels, 0);

        preprocessThumbnail(dRgbPixels, dThumbnail);

        pixelBuffer.unmapDevice();

        preprocessForDetection(pixels, userResource);

        generateFrameTexture(pixelBuffer, userResource);
        generateThumbnailTexture(dThumbnail, userResource);
    }

    void ResourcePreprocessor::preprocessThumbnail(cv::cuda::GpuMat& src, cv::cuda::GpuMat& dst)
    {
        const uint32_t maxDimension = std::max(src.cols, src.rows);
        const auto aspectRatio = static_cast<float>(UserResource::THUMBNAIL_WIDTH) / maxDimension;

        const auto thumbnailWidth = static_cast<uint32_t>(static_cast<float>(src.cols) * aspectRatio);
        const auto thumbnailHeight = static_cast<uint32_t>(static_cast<float>(src.rows) * aspectRatio);

        cv::cuda::resize(src, dst, cv::Size(thumbnailWidth, thumbnailHeight));
    }

    void ResourcePreprocessor::preprocessForDetection(const cv::Mat& pixels, UserResource* userResource)
    {
        cv::Mat resizedPixels;
        cv::resize(pixels, resizedPixels, cv::Size(300, 300));

        userResource->detectionFrame = std::move(resizedPixels);
    }

    void ResourcePreprocessor::generateFrameTexture(const cv::ogl::Buffer& pixels, UserResource* userResource)
    {
        pixels.bind(cv::ogl::Buffer::PIXEL_UNPACK_BUFFER);

        const Xenon::Texture2DConfiguration textureCfg(pixels.cols(), pixels.rows(), 
            resolveFormat(pixels.channels()));
        userResource->frame = Xenon::Texture2D::create(nullptr, textureCfg);

        cv::ogl::Buffer::unbind(cv::ogl::Buffer::PIXEL_UNPACK_BUFFER);
    }

    void ResourcePreprocessor::generateThumbnailTexture(const cv::cuda::GpuMat& thumbnailPixels,
        UserResource* userResource)
    {
        const cv::ogl::Buffer thumbnailBuffer(thumbnailPixels, cv::ogl::Buffer::PIXEL_UNPACK_BUFFER, true);
        thumbnailBuffer.bind(cv::ogl::Buffer::PIXEL_UNPACK_BUFFER);

        const Xenon::Texture2DConfiguration textureCfg(thumbnailPixels.cols, thumbnailPixels.rows, 
            resolveFormat(thumbnailPixels.channels()));
        userResource->thumbnail = Xenon::Texture2D::create(nullptr, textureCfg);

        cv::ogl::Buffer::unbind(cv::ogl::Buffer::PIXEL_UNPACK_BUFFER);
    }

    Xenon::TextureFormat ResourcePreprocessor::resolveFormat(const uint32_t channels)
    {
        switch (channels)
        {
            case 3: return Xenon::TextureFormat::RGB;
            case 4: return Xenon::TextureFormat::RGBA;
        }

        XN_ASSERT(true);
        return Xenon::TextureFormat::RGB;
    }
}
