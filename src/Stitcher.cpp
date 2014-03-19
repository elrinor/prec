#include "config.h"
#include <utility>
#include "Stitcher.h"

using namespace arx;
using namespace std;

namespace prec {
  class AlphaFallBack: public GenericImageBase<float, AlphaFallBack, false> {
  private:
    int width;
    int height;
    float w;
    float h;

    static float f(float pos, float max) {
      return 1 - abs(2 * pos / max - 1);
    }

  public:
    typedef float color_type;

    AlphaFallBack(int width, int height): width(width), height(height), w((float) (width - 1)), h((float) (height - 1)) {}

    int getWidth() const { return width; }

    int getHeight() const { return height; }

    color_type getPixelInterpolated(float x, float y) const {
      return f(x, w) * f(y, h);
    }

    color_type getPixel(int x, int y) const {
      return f((float) x, w) * f((float) y, h);
    }
  };

  Image4f Stitcher::stitch(Panorama p) {
    Image4f result(4000, 4000);
    result.fill(Color4f(0, 0, 0, 0));

    Matrix3f toCenter = Matrix3f::translation(2000, 2000);

    for(size_t i = 0; i < p.size(); i++) {
      PanoImage image = p.getImage(i);
      result.drawBlended(createImageAlphaComposition<float>(image.getOriginal(), AlphaFallBack(image.getOriginal().getWidth(), image.getOriginal().getHeight())),
        toCenter * 
        Matrix3f::scale(1000) *
        image.getHomography().getInverseMatrix() * 
        Matrix3f::scale(image.getKeyPointScaleFactor()) *
        Matrix3f::translation(image.getOriginal().getWidth() / -2.0f, image.getOriginal().getHeight() / -2.0f),
        BlendFunc::PLUS<true, true, false>());

      /*result.draw(image.getOriginal(),
        toCenter * 
        Matrix3f::scale(1000) *
        image.getHomography().getInverseMatrix() * 
        Matrix3f::scale(image.getKeyPointScaleFactor()) *
        Matrix3f::translation(image.getOriginal().getWidth() / -2.0f, image.getOriginal().getHeight() / -2.0f));*/
    }

    result.drawBlended(createImageConstComposition(result.getWidth(), result.getHeight(), Color4f(0, 0, 0, 0)), 0, 0, BlendFunc::PLUS<false, true, true>());

    return result;
  }

} // namespace prec
