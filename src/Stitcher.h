#ifndef __STITCHER_H__
#define __STITCHER_H__

#include "config.h"
#include "arx/smart_ptr.h"
#include "arx/Collections.h"
#include "Image.h"
#include "Panorama.h"

namespace prec {

  class Stitcher {
  private:

  public:
    Stitcher() {};

    Image4f stitch(Panorama p);
  };

} // namespace prec

#endif