#ifndef __PANOIMAGE_H__
#define __PANOIMAGE_H__

#include "config.h"
#include <utility>
#include <functional>
#include <string>
#include "arx/smart_ptr.h"
#include "arx/Thread.h"
#include "SafeIdProvider.h"
#include "Image.h"
#include "SIFT.h"
#include "Homography.h"

namespace prec {
  /**
   * PanoImage represents a single image in a panorama.
   */
  class PanoImage {
  private:
    struct PanoImageData {
      std::string fileName; /**< Original file name. */

      Image3f original; /**< Original image, loaded from file. */

      float keyPointScaleFactor; /**< Scale factor for SIFT keypoints coordinates. */

      Image1f downScaled; /**< Downscaled image, for SIFT calculation. */

      int id; /**< Unique image identifier. */

      SIFTList keyPointList; /**< Exracted keypoints. */

      Homography homography; /**< Image homography for panorama stitching. */
    };

    arx::shared_ptr<PanoImageData> data;

  public:
    PanoImage() {}

    /** Constructor. */
    PanoImage(std::string fileName, int downScaleWidth, int downScaleHeight): data(new PanoImageData()) {
      /* Set file name. */
      data->fileName = fileName;

      /* Load image. */
      data->original = Image3f::loadFromFile(fileName);

      /* Get free id. */
      data->id = SafeIdProvider::getNextFreeId();

      /* Downscale image for keypoint extraction. */
      float originalWidth = (float) data->original.getWidth();
      float originalHeight = (float) data->original.getHeight();
      float downScaleFactor = std::min(1.0f, std::min(downScaleWidth / originalWidth, downScaleHeight / originalHeight));
      data->downScaled = data->original.convert<float>().resize(downScaleFactor, downScaleFactor);
      
      /* Extract keypoints. */
      SIFTExtractor siftExtractor;
      data->keyPointList = siftExtractor.extractKeyPoints(data->downScaled);

      /* Calculate keypoint scale factor relative to original image. */
      data->keyPointScaleFactor = 1.0f / sqrt((float) originalWidth * originalHeight);

      /* Calculate keypoint scale factor relative to downScaled image. */
      float relativeScaleFactor = data->keyPointScaleFactor / downScaleFactor;

      /* Tag & scale keypoints. */
      arx::Vector2f slide = arx::Vector2f(-0.5f * data->downScaled.getWidth(), -0.5f * data->downScaled.getHeight());
      for(std::size_t i = 0; i < data->keyPointList.size(); i++) {
        SIFT sift = data->keyPointList[i];
        sift.setTag(data->id);
        sift.setXY((sift.getXY() + slide) * relativeScaleFactor);
      }
    }

    const Image3f& getOriginal() const { return data->original; }
    const Image1f& getDownScaled() const { return data->downScaled; }
    const SIFTList& getKeyPointList() const { return data->keyPointList; }
    const std::string& getFileName() const { return data->fileName; }
    int getId() const { return data->id; }
    float getKeyPointScaleFactor() const { return data->keyPointScaleFactor; }
    
    const Homography& getHomography() const { return data->homography; }
    void setHomography(const Homography& h) { data->homography = h; }

    bool isNull() const { return data.get() == NULL; }
  };

  struct PanoImageIdComparator: public std::binary_function<PanoImage, PanoImage, bool> {
    bool operator()(const PanoImage& a, const PanoImage& b) const {
      return a.getId() < b.getId();
    }
  };

} // namespace prec

#endif // __PANOIMAGE_H__
