#ifndef __MATCHER_H__
#define __MATCHER_H__

#include "config.h"
#include <arx/smart_ptr.h>
#include <arx/Collections.h>
#include "PanoImage.h"
#include "Panorama.h"

#ifdef USE_CIPPIMAGE
#  include "ippimage/ippimage.h"
#endif

namespace prec {
  namespace detail {
    class MatcherImpl;
  }

  /**
   * 
   */
  class Matcher {
  private:
    arx::shared_ptr<detail::MatcherImpl> impl;
  
  public:
    Matcher(unsigned int minimumMatches, unsigned int maximumMatches, bool useRANSAC = true);
    arx::ArrayList<Panorama> matchImages(arx::ArrayList<PanoImage> imageList);
    
#if 0
    arx::ArrayList<unsigned int> matchSingleImage(Image3f image, arx::ArrayList<Image3f> imageList);
    arx::ArrayList<unsigned int> matchSingleImage(arx::ArrayList<KeyPoint> keys, arx::ArrayList<arx::ArrayList<KeyPoint> > keyList);
#ifdef USE_CIPPIMAGE
    arx::ArrayList<unsigned int> matchSingleImage(CIppImage* image, arx::ArrayList<CIppImage*> imageList);
#endif
#endif
  };

} // namespace prec

#endif
