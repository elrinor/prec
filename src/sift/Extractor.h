#ifndef __SIFT_EXTRACTOR_H__
#define __SIFT_EXTRACTOR_H__ 

#include "config.h"
#include <utility>
#include <arx/Collections.h>
#include <arx/LinearAlgebra.h>
#include "Image.h"
#include "Octave.h"

using namespace std;

namespace prec {
  namespace detail {
// -------------------------------------------------------------------------- //
// ExtractorImpl
// -------------------------------------------------------------------------- //
    template<class Traits>
    class ExtractorImpl: Traits {
    private:
      typedef arx::array<arx::array<arx::array<float, ORI_SIZE>, INDEX_SIZE>, INDEX_SIZE> index_type;
      typedef typename Traits::key_data_type key_data_type;
      typedef typename Traits::key_data_list_type key_data_list_type;
      typedef typename Traits::key_list_type key_list_type;
      typedef typename key_data_type::FeatureVector FeatureVector;

      template<class T> class ScalePoint {
      public:
        T x, y, s;
        ScalePoint(T x, T y, T s) : x(x), y(y), s(s) {}
        ScalePoint() {}
      };

      /**
       * Increment appropriate locations in the index to incorporate
       * the image sample.
       *
       * @param index index array to work on
       * @param mag magnitude of the sample
       * @param ori orientation of the sample (in radians)
       * @param fx x sample coordinate in the index 
       * @param fy y sample coordinate in the index
       */
      void placeInIndex(index_type& index, float mag, float ori, float fx, float fy) {
        float fo = ORI_SIZE * ori / (2 * PI);

        /* Round down to next integer. */
        int ix = (int) fx;
        int iy = (int) fy;
        int io = (int) fo;

        /* Fractional part of location. */
        float xFrac = fx - ix;
        float yFrac = fy - iy;
        float oFrac = fo - io;

        /* Put appropriate fraction in each of 8 buckets around this point
         * in the (x,y,o) dimensions. */
        for(int y = 0; y < 2; y++) {
          int yIndex = y + iy;
          if(yIndex < 0 || yIndex >= INDEX_SIZE)
            continue;
          float yWeight = mag * ((y == 0) ? (1.0f - yFrac) : (yFrac));
          for(int x = 0; x < 2; x++) {
            int xIndex = x + ix;
            if(xIndex < 0 || xIndex >= INDEX_SIZE)
              continue;
            float xWeight = yWeight * ((x == 0) ? (1.0f - xFrac) : (xFrac));
            for(int o = 0; o < 2; o++) {
              int oIndex = o + io;
              if(oIndex < 0 || oIndex >= ORI_SIZE)
                continue;
              float oWeight = xWeight * ((o == 0) ? (1.0f - oFrac) : (oFrac));
              index[xIndex][yIndex][oIndex] += oWeight;
            }
          }
        }
      }

      /**
       * Create a descriptor vector for the given keypoint.
       *
       * @param key keypoint to create feature vector for
       * @param magnitude gradient magnitude image
       * @param direction gradient orientation image
       * @param p keypoint location in magnitude and direction images 
       *   (it differs from its location in original image given by key.getX() 
       *   and key.getY() !)
       */
      void createDescriptor(key_data_type& key, Image1f magnitude, Image1f direction, ScalePoint<float> p) {
        /* The spacing of index samples in terms of pixels at this scale. */
        float spacing = p.s * MAG_FACTOR;

        /* Radius of index sample region must extend to diagonal corner of
         * index patch plus half sample for interpolation. */
        int radius = (int)(1.414f * spacing * (INDEX_SIZE + 1) / 2.0f + 0.5f);

        /* Precompute a sigma for gaussian weightening.
         * Sigma is relative to half-width of index. */
        float sigma = INDEX_SIGMA * 0.5f * INDEX_SIZE;

        /* The angle to rotate with */
        float angle = key.angle;

        /* Integer peak position */
        int ipx = (int) (p.x + 0.5f);
        int ipy = (int) (p.y + 0.5f);

        /* Index array */
        index_type index;
        for(size_t i = 0; i < INDEX_SIZE; i++)
          for(size_t j = 0; j < INDEX_SIZE; j++)
            for(size_t k = 0; k < ORI_SIZE; k++)
              index[i][j][k] = 0.0f;

        /* Examine all points from the gradient image that could lie within the index square. */
        // TODO: minX, maxX, etc...
        for(int dy = -radius; dy <= radius; dy++) {
          for(int dx = -radius; dx <= radius; dx++) {
            /* Compute absolute coordinates within image */
            int x = ipx + dx;
            int y = ipy + dy;

            /* Ignore border pixels and pixels outside the image boundary */
            if(x < 1 || x >= magnitude.getWidth() - 1 || y < 1 || y >= magnitude.getHeight() - 1)
              continue;

            /* Rotate and scale. Also, make subpixel correction. */
            float dxr = (cos(angle) * dx - sin(angle) * dy - (p.x - ipx)) / spacing;
            float dyr = (sin(angle) * dx + cos(angle) * dy - (p.y - ipy)) / spacing;

            /* Compute location of sample in terms of real-valued index array
             * coordinates.  Subtract 0.5 so that ix of 1.0 means to put full
             * weight on index[1]. */
            float ix = dxr + INDEX_SIZE / 2.0f - 0.5f;
            float iy = dyr + INDEX_SIZE / 2.0f - 0.5f;

            /* Test whether this sample falls within boundary of index patch. */
            if(ix <= -1.0f || ix >= (float)INDEX_SIZE || iy <= -1.0f || iy >= (float)INDEX_SIZE)
              continue;

            /* Compute magnitude weighted by a gaussian as function of radial
             * distance from center. */
            float mag = magnitude.getPixel(x, y) * exp(- (sqr(dxr) + sqr(dyr)) / (2.0f * sqr(sigma)));

            /* Subtract keypoint orientation to give ori relative to keypoint. */
            float ori = direction.getPixel(x, y) - key.angle;

            /* Put orientation in range [0, 2*PI] */
            while(ori > 2 * PI)
              ori -= 2*PI;
            while(ori < 0.0f)
              ori += 2*PI;

            /* Modify index */
            placeInIndex(index, mag, ori, ix, iy);
          }
        }

        /* Unwrap the 3D index values into 1D descriptor. */
        Vector<float, VEC_LENGTH>& vec = *reinterpret_cast<Vector<float, VEC_LENGTH>*>(&index); 

        /* This one was dangerous => protected with STATIC_ASSERT. */
        STATIC_ASSERT((sizeof(index) == sizeof(vec) && sizeof(vec) == sizeof(float) * VEC_LENGTH));

        /* Normalize feature vector. */
        vec.normalize();

        /* Now that normalization has been done, threshold elements of
         * index vector to decrease emphasis on large gradient magnitudes. */
        bool changed = false;
        for(int i = 0; i < VEC_LENGTH; i++) {
          if(vec[i] > MAX_INDEX_VAL) {
            vec[i] = MAX_INDEX_VAL;
            changed = true;
          }
        }
        if(changed)
          vec.normalize();

        /* Convert to integer, assuming each element of feature vector 
         * is less than 0.5. */
        for(int i = 0; i < VEC_LENGTH; i++) {
          int intVal = (int) (512.0f * vec[i]);
          key.vec[i] = (unsigned char) min(255, intVal);
        }
      }

      /**
       * Fit a parabola to the three points (-1.0 ; left), (0.0 ; middle) and
       * (1.0 ; right) and return a number in the range [-1, 1] that represents 
       * the peak location. The center value is assumed to be greater than or
       * equal to the other values if positive, or less than if negative.
       * 
       * @param left
       * @param middle
       * @param right
       * @returns the peak location
       */
      float interpolatePeak(float left, float middle, float right) {
        if(middle < 0.0f) {
          left = -left; 
          middle = -middle; 
          right = -right;
        }
        assert(middle >= left && middle >= right);
        return 0.5f * (left - right) / (left - 2.0f * middle + right);
      }

      /**
       * Assign zero or more orientations to given peak location and create a 
       * keypoint for each orientation.
       *
       * @param magnitude gradient magnitude image
       * @param direction gradient orientation image
       * @param pixelSize a size of the pixel in the given octave relative to the 
       *   original picture's pixel size
       * @param interpPoint interpolated peak position
       * @param keys a list of keypoints to add new keypoints to
       * @returns a slice of keys that contains newly added keypoints
       */
      void generateKeypoints(Image1f magnitude, Image1f direction, float pixelSize, ScalePoint<float> interpPoint, key_data_list_type keys) {
        ScalePoint<int> p((int) (interpPoint.x + 0.5f), (int) (interpPoint.y + 0.5f), max((int) (interpPoint.s + 0.5f), 1)); // TODO: why max?
        array<float, ORI_BINS> bins;

        bins.assign(0.0f);

        /* Calculate sigma and radius of a gaussian window 
         * used for orientation histogram construction */
        float sigma = ORI_SIGMA * p.s;
        int radius = (int) (sigma * 3.0f + 0.5f);

        /* Determine the lookup window size */
        int xMin = max(p.x - radius, 1);
        int xMax = min(p.x + radius, magnitude.getWidth() - 1);
        int yMin = max(p.y - radius, 1);
        int yMax = max(p.y - radius, magnitude.getHeight() - 1);

        /* Fill the direction histogram */
        for(int y = yMin; y < yMax; y++) {
          for(int x = xMin; x < xMax; x++) {
            /* discard "flat" points */
            if(magnitude.getPixel(x, y) <= 0)
              continue;

            float distSqr = sqr(x - interpPoint.x) + sqr(y - interpPoint.y);

            /* discard all pixels that lie outside the circle boundaries */
            if(distSqr > sqr(radius) + 0.5f)
              continue;

            float gaussianWeight = exp(-distSqr / (2.0f * sigma * sigma));
            int bin = (int) (ORI_BINS * (direction.getPixel(x, y) + PI + 0.0001f) / (2.0f * PI));
            if(bin >= ORI_BINS)
              bin = 0;
            bins[bin] += magnitude.getPixel(x, y) * gaussianWeight;
          }
        }

        /* Smooth the direction histogram using a [1/3 1/3 1/3] kernel.
         * Why do we need smoothing? Consider the following situation:
         * [..., 0.2, 0.8, 0.7, 0.8, 0.2, ...]
         *                 ^ the peak is here!  */
        // TODO: what number of steps is good enought? Lowe uses 6, libsift uses 4... */
        for(int step = 0; step < 4; step++) {
          float prev, temp;
          prev = bins[bins.size() - 1];
          for(unsigned int i = 0; i < bins.size(); i++) {
            temp = bins[i];
            bins[i] = (prev + bins[i] + bins[(i + 1 == bins.size()) ? 0 : i + 1]) / 3.0f;
            prev = temp;
          }
        }

        /* Find the maximum peak at the histogram */
        float maxPeak = 0.0;
        for(unsigned int i = 0; i < bins.size(); i++)
          if(bins[i] > maxPeak)
            maxPeak = bins[i];

        /* Look for each local peak in histogram.  If value is within
         * the given threshold of maximum value, then generate a keypoint. */
        for(unsigned int i = 0; i < bins.size(); i++) {
          /* discard the peaks that don't fulfill the threshold */
          if(bins[i] < maxPeak * ORI_HIST_THRESH)
            continue;

          int prevI = (i == 0 ? (int) bins.size() - 1 : i - 1);
          int nextI = (i == (int) bins.size() - 1 ? 0 : i + 1);

          /* discard all non-peak points */
          if(bins[i] < bins[prevI] || bins[i] < bins[nextI])
            continue;

          /* Ok, it's time to use parabolic fit to interpolate peak location */
          float binCorrection = interpolatePeak(bins[prevI], bins[i], bins[nextI]);
          // TODO: why do we need "+ 0.5f" here?
          float angle = 2.0f * PI * (i + 0.5f + binCorrection) / ORI_BINS - PI;
          assert(angle >= -PI && angle <= PI);

          /* Create a keypoint with this orientation. 
           * NOTE: the coordinates of keypoint must be calculated relative to 
           * the original image, therefore we need to multiply the given peak 
           * coordinates by factor of pixelSize */
          keys.push_back(key_data_type(pixelSize * interpPoint.x, pixelSize * interpPoint.y, pixelSize * interpPoint.s, angle));

          /* Create descriptor for newly added keypoint */
          createDescriptor(*keys.rbegin(), magnitude, direction, interpPoint);
        }
      }

      /**
       * Apply the method developed by Matthew Brown (see BMVC 02 paper) to
       * fit a 3D quadratic function through the DOG function values around
       * the location p, at which a peak has been detected. 
       *
       * @param oct a scale space octave in which peak was found
       * @param p a peak location
       * @param peak (out) interpolated DOG magnitude at this peak
       * @returns the interpolated peak position relative to the given position p
       */ 
      ScalePoint<float> getAdjustment(Octave oct, ScalePoint<int> p, float& peak) {
        Image1f below = oct.getDoG(p.s - 1);
        Image1f current = oct.getDoG(p.s);
        Image1f above = oct.getDoG(p.s + 1);

        int x = p.x;
        int y = p.y;

        Matrix3f H;
        H[0][0] = below.getPixel(x, y) - 2 * current.getPixel(x, y) + above.getPixel(x, y);
        H[0][1] = H[1][0] = 0.25f * (above.getPixel(x, y + 1) - above.getPixel(x, y - 1) - (below.getPixel(x, y + 1) - below.getPixel(x, y - 1)));
        H[0][2] = H[2][0] = 0.25f * (above.getPixel(x + 1, y) - above.getPixel(x - 1, y) - (below.getPixel(x + 1, y) - below.getPixel(x - 1, y)));
        H[1][1] = current.getPixel(x, y - 1) - 2 * current.getPixel(x, y) + current.getPixel(x, y + 1);
        H[1][2] = H[2][1] = 0.25f * (current.getPixel(x + 1, y + 1) - current.getPixel(x - 1, y + 1) - (current.getPixel(x + 1, y - 1) - current.getPixel(x - 1, y - 1)));
        H[2][2] = current.getPixel(x - 1, y) - 2 * current.getPixel(x, y) + current.getPixel(x + 1, y);

        Vector3f g;
        g[0] = 0.5f * (above.getPixel(x, y) - below.getPixel(x, y));
        g[1] = 0.5f * (current.getPixel(x, y + 1) - current.getPixel(x, y - 1));
        g[2] = 0.5f * (current.getPixel(x + 1, y) - current.getPixel(x - 1, y));

        Vector3f offset = -g;
        solveLinearSystem(H, offset);

        peak = offset.dot(g) * 0.5f + current.getPixel(x, y);
        return ScalePoint<float>(offset[2], offset[1], offset[0]);
      }

      /**
       * Find the subpixel position of a given keypoint.
       *
       * @param oct a scale space octave in which keypoint was found
       * @param mask an image mask which is used to prevent keypoint duplicates
       * @param p a keypoint position
       * @param remainingMoves number of remaining interpolation steps
       * @param (out) interpolatedPoint the subpixel keypoint position
       * @returns true if everything went OK, false otherwise. The return value 
       *   of false means that the given keypoint is not suitable and must be discarded.
       */
      bool localizeKeyPoint(Octave oct, Image1f mask, ScalePoint<int> p, int remainingMoves, ScalePoint<float>& interpolatedPoint) {
        float peak;
        ScalePoint<float> offset = getAdjustment(oct, p, peak);

        int x = p.x, y = p.y;
        if(offset.x > 0.6 && x < oct.getWidth() - 3)
          x++;
        if(offset.x < -0.6 && x > 3)
          x--;
        if(offset.y > 0.6 && y < oct.getHeight() - 3)
          y++;
        if(offset.y < -0.6 && y > 3)
          y--;
        if(remainingMoves > 0 && (x != p.x || y != p.y))
          return localizeKeyPoint(oct, mask, ScalePoint<int>(x, y, p.s), remainingMoves - 1, interpolatedPoint);

        if(abs(offset.x) > 1.5 || abs(offset.y) > 1.5 || abs(offset.s) > 1.5 || abs(peak) < PEAK_THRESH)
          return false;

        if(mask.getPixel(x, y) > 0.0)
          return false;
        mask.setPixel(x, y, 1.0);

        /* The scale relative to this octave is given by octScale. The scale
         * units are in terms of sigma for the smallest of the Gaussians in the
         * DOG used to identify that scale. */
        float octScale = oct.getInitSigma() * pow(2.0f, (p.s + offset.s) / (float) oct.getScales());

        interpolatedPoint.x = x + offset.x;
        interpolatedPoint.y = y + offset.y;
        interpolatedPoint.s = octScale;
        return true;
      }

      /**
       * Supplementary function that determines whether there is a local 
       * minimum or maximum at a given position in a given image.
       * 
       * @param img an image to check for minimum / maximum
       * @param val a value of a minimum / maximum
       * @param x
       * @param y
       * @returns true if there is a maximum or minimum at the given position, 
       *   false otherwise
       */
      bool isLocalMinMax3x3(Image1f img, float val, int x, int y) {
        int w = img.getWStep() / sizeof(float);
        float *p = img.getPixelData() + y * w + x;
        if(val > 0.0) {
          if(p[0] > val || p[w] > val || p[-w] > val || p[1] > val || p[-1] > val || p[w + 1] > val || p[w - 1] > val || p[-w + 1] > val || p[-w - 1] > val)
            return false;
        } else {
          if(p[0] < val || p[w] < val || p[-w] < val || p[1] < val || p[-1] < val || p[w + 1] < val || p[w - 1] < val || p[-w + 1] < val || p[-w - 1] < val)
            return false;
        }
        return true;
      }

      /**
       * @param oct a scale space octave
       * @param p a position in scale space to check for minimum / maximum
       * @returns true if there is a maximum or minimum of DoG function at 
       *   the given position p, false otherwise
       */
      bool isLocalMinMax3x3x3(Octave oct, ScalePoint<int> p) {
        float val = oct.getDoG(p.s).getPixel(p.x, p.y);
        return isLocalMinMax3x3(oct.getDoG(p.s), val, p.x, p.y) && 
          isLocalMinMax3x3(oct.getDoG(p.s - 1), val, p.x, p.y) && 
          isLocalMinMax3x3(oct.getDoG(p.s + 1), val, p.x, p.y);
      }

      /**
       * @param oct a scale space octave
       * @param p a position in scale space to check
       * @returns true if the given position p in scale space is too edgelike and 
       *   therefore is not suitable for keypoint creation, false otherwise
       */
      bool isOnEdge(Octave oct, ScalePoint<int> p) {
        Image1f img = oct.getDoG(p.s);
        int& x = p.x;
        int& y = p.y;
        float d00, d11, d01;
        d00 = img.getPixel(x + 1, y) + img.getPixel(x - 1, y) - 2.0f * img.getPixel(x, y);
        d11 = img.getPixel(x, y + 1) + img.getPixel(x, y - 1) - 2.0f * img.getPixel(x, y);
        d01 = 0.25f * ((img.getPixel(x + 1, y + 1) - img.getPixel(x + 1, y - 1)) - (img.getPixel(x - 1, y + 1) - img.getPixel(x - 1, y - 1)));
        float trace = sqr(d00 + d11);
        float det = d00 * d11 - (d01 * d01);
        float inc = sqr(EDGE_EIGEN_RATIO + 1.0f);
        return (trace / det) >= (inc / EDGE_EIGEN_RATIO);
      }


      /**
       * Finds all keypoints within the given scale space octave.
       * 
       * @param oct a scale space octave to search keypoints in
       * @param pixelSize a size of the pixel in the given octave relative to the 
       *   original picture's pixel size
       * @param keys (out) list of keypoints
       */
      void getKeyPointsWithinOctave(Octave oct, float pixelSize, key_data_list_type keys) {
        /* Mask - is there a keypoint? */
        Image1f mask(oct.getWidth(), oct.getHeight());
        mask.fill(0.0f);

        ScalePoint<int> p;
        for(p.s = 1; p.s < oct.getScales() + 1; p.s++) {
          /* Get images of gradients and orientations */
          Image1f mag, dir;
          oct.getBlur(p.s).gradientMagAndDir(mag, dir);

          /* Search for extrema */
          for(p.y = BORDER_DIST; p.y < oct.getHeight() - BORDER_DIST; p.y++) {
            for(p.x = BORDER_DIST; p.x < oct.getWidth() - BORDER_DIST; p.x++) {
              /* DOG magnitude must be above 0.8 * PEAK_THRESH threshold */
              if(abs(oct.getDoG(p.s).getPixel(p.x, p.y)) < 0.8 * PEAK_THRESH)
                continue;

              /* Must be local min/max */
              if(!isLocalMinMax3x3x3(oct, p))
                continue;

              /* Must be not on edge */
              if(isOnEdge(oct, p))
                continue;

              /* Localize peak */
              ScalePoint<float> interpPoint;
              if(!localizeKeyPoint(oct, mask, p, MAX_KEYPOINT_INTERP_MOVES, interpPoint))
                continue;

              /* Generate zero or more keypoints from peak location */
              generateKeypoints(mag, dir, pixelSize, interpPoint, keys);
            }
          }
        }
      }

    public:
      key_list_type extractKeyPoints(Image1f img) {
        key_data_list_type keys;
        float pixelSize = 1.0; 
        float curSigma = 0.5; /* assume image from camera has smoothing of sigma = 0.5 */

        if(DOUBLE_IMAGE_SIZE) {
          /* TODO: As I have found out before, ippi scale functions work really strange...
           * So maybe it would be a better idea to upscale the image using a hand-coded
           * upscale function. */
          img = img.resize(2.0f, 2.0f);
          pixelSize *= 0.5;
          curSigma *= 2;
        }

        if(INIT_SIGMA > curSigma) {
          float sigma = sqrt(INIT_SIGMA * INIT_SIGMA - curSigma * curSigma);
          img = img.gaussianBlur(sigma);
        }

        int minSize = BORDER_DIST * 2 + 2;
        while (img.getWidth() > minSize && img.getHeight() > minSize) {
          getKeyPointsWithinOctave(Octave(img, SCALES, curSigma), pixelSize, keys);
          /* NOTE: using scale(0.5, 0.5) with ippi causes some nasty special effects
           * which result in wrong keypoint localization. That's why using hand-coded
           * 2x downscaling here is a MUST. */
          img = img.resizeDownNN<2>();

          pixelSize *= 2;
        }

        return key_list_type(keys);
      }
    };

  } // namespace detail


// -------------------------------------------------------------------------- //
// Extractor
// -------------------------------------------------------------------------- //
  template<class Traits>
  class Extractor: Traits {
  private:
    typedef typename Traits::extractor_impl_type extractor_impl_type;
    typedef typename Traits::key_list_type key_list_type;

    arx::shared_ptr<extractor_impl_type> impl;

  public:
    Extractor(): impl(new extractor_impl_type()) {}

    key_list_type extractKeyPoints(Image1f img) {
      return this->impl->extractKeyPoints(img);
    }

    key_list_type extractKeyPoints(Image3f img) {
      return extractKeyPoints(img.convert<Image1f>());
    }

#ifdef USE_CIPPIMAGE
    key_list_type extractKeyPoints(CIppImage* img) {
      if(img->NChannels() == 1)
        return extractKeyPoints(Image1f(img));
      else
        return extractKeyPoints(Image3f(img));
    }
#endif
  };

} // namespace prec

#endif // __SIFT_EXTRACTOR_H__
