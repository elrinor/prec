#ifndef __OCTAVE_H__
#define __OCTAVE_H__

#include "config.h"
#include <vector>
#include <arx/smart_ptr.h>
#include "Image.h"

namespace prec {
// -------------------------------------------------------------------------- //
// Octave class
// -------------------------------------------------------------------------- //
  class Octave {
  private:
    struct OctaveData {
      int scales;
      float initSigma;
      std::vector<Image1f> blur;
      std::vector<Image1f> dogs;
    };
    arx::shared_ptr<OctaveData> data;

  public:
    Octave() {}
    
    Octave(Image1f image, int scales, float initSigma): data(new OctaveData()) {
      this->data->scales = scales;
      this->data->initSigma = initSigma;
      float sigmaRatio = pow(2.0f, 1.0f / scales);
      float lastSigma = initSigma;

      this->data->blur.push_back(image);
      for(int i = 1; i < scales + 3; i++) {
        float dSigma = lastSigma * sqrt(sigmaRatio * sigmaRatio - 1.0f);
        this->data->blur.push_back(this->data->blur[i - 1].gaussianBlur(dSigma));
        lastSigma *= sigmaRatio;
      }

      for(int i = 0; i < scales + 2; i++)
        this->data->dogs.push_back(this->data->blur[i].sub(this->data->blur[i + 1]));
    }

    Image1f get2xBlurredImage() {
      return this->getBlur(this->getScales());
    }

    int getScales() const { return this->data->scales; }
    float getInitSigma() const { return this->data->initSigma; }
    Image1f getBlur(int index) const { return this->data->blur[index]; }
    Image1f getDoG(int index) const { return this->data->dogs[index]; }
    int getWidth() const { return this->getBlur(0).getWidth(); }
    int getHeight() const { return this->getBlur(0).getHeight(); }
  };

} // namespace prec


#endif
