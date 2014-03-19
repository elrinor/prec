#ifndef __PANORAMA_H__
#define __PANORAMA_H__

#include "config.h"
#include <arx/Collections.h>
#include <arx/smart_ptr.h>
#include "matching/ImageMatch.h"
#include "PanoImage.h"

namespace prec {
  namespace detail {
    class MatcherImpl;
  }

// -------------------------------------------------------------------------- //
// Panorama
// -------------------------------------------------------------------------- //
  class Panorama {
  private:
    struct PanoramaData {
      arx::ArrayList<PanoImage> images;
      arx::ArrayList<ImageMatch> imageMatches;
    };

    arx::shared_ptr<PanoramaData> data;

    void addImage(PanoImage img) {
      data->images.push_back(img);
    }

    void addMatch(ImageMatch imageMatch) {
      data->imageMatches.push_back(imageMatch);
    }

    friend class detail::MatcherImpl;

  public:
    Panorama(): data(new PanoramaData()) {}

    const arx::ArrayList<PanoImage>& getImages() const { return data->images; }
    arx::ArrayList<PanoImage>& getImages() { return data->images; }
    const PanoImage& getImage(std::size_t index) const { return data->images[index]; }
    PanoImage& getImage(std::size_t index) { return data->images[index]; }

    const arx::ArrayList<ImageMatch>& getImageMatches() const { return data->imageMatches; }
    const ImageMatch& getImageMatch(std::size_t index) const { return data->imageMatches[index]; }

    size_t size() const { return data->images.size(); }

  };

} // namespace prec


#endif // __PANORAMA_H__
