#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "arx/config.h"

// -------------------------------------------------------------------------- //
// General
// -------------------------------------------------------------------------- //
/** Use OpenCV for image saving / loading? If not defined, only 24-bit bmp files will be supported */
#define USE_OPENCV

/** Use ippi for image processing? If not defined, hand-coded routines will be used
 * instead of ippi ones. This may lead to significant slowdown. */
#define USE_IPPI

/** Define CIppImage-compatible interfaces? */
#define USE_CIPPIMAGE

/** Use ippiMalloc for image allocation? */
#define USE_IPPI_MALLOC

/** Use aligned allocation even if compiling without ippi? */
#define USE_ALIGNED_IMAGE_ALLOCATION

/** Debug output on/off. */
// #define DEBUG


// -------------------------------------------------------------------------- //
// SIFT Config
// -------------------------------------------------------------------------- //
/** Double image size before keypoint search? */
#define DOUBLE_IMAGE_SIZE false

/** Initial smoothing level applied before any other processing */
#define INIT_SIGMA 1.6f

/** Peaks in the DOG function must be at least BORDER_DIST samples away from the image border */
#define BORDER_DIST 5

/** number of discrete smoothing levels within each octave */
#define SCALES 3

/** Magnitude of difference-of-Gaussian value at a keypoint must be above PEAK_THRESH.  */
#define PEAK_THRESH_INIT 0.04f
#define PEAK_THRESH (PEAK_THRESH_INIT / SCALES)

/** EDGE_EIGEN_RATIO is used to eliminate keypoints that lie on an edge in the image without 
 * their position being accurately determined along the edge.  
 * This can be determined by checking the ratio of eigenvalues of a Hessian matrix 
 * of the DOG function computed at the keypoint.  
 * An EDGE_EIGEN_RATIO of 10 means that all keypoints with 
 * a ratio of principle curvatures greater than 10 are discarded. */
#define EDGE_EIGEN_RATIO 10.0f

/** Truncate gaussian kernel at sigma*GAUSS_TRUNCATE pixels away from center */
#define GAUSS_TRUNCATE 4.0f

/** Max number of move iterations during keypoint localization */
#define MAX_KEYPOINT_INTERP_MOVES 5

/** If USE_HISTOGRAM_ORI is true, then the histogram method is used
 * to determine keypoint orientation.  Otherwise, just use average
 * gradient direction within surrounding region (which has been found
 * to be less stable).  If using histogram, then ORI_BINS gives the
 * number of bins in the histogram (36 gives 10 degree spacing of
 * bins). */
#define USE_HISTOGRAM_ORI true
#define ORI_BINS 36

/** Size of Gaussian used to select orientations as multiple of scale
 * of smaller Gaussian in DOG function used to find keypoint.
 * Best values: 1.0 for UseHistogramOri = FALSE; 1.5 for TRUE. */
#define ORI_SIGMA 1.5f

/** All local peaks in the orientation histogram are used to generate
 * keypoints, as long as the local peak is within OriHistThresh of
 * the maximum peak.  A value of 1.0 only selects a single orientation
 * at each location. */
#define ORI_HIST_THRESH 0.8f

/** This constant specifies how large a region is covered by each index
 * vector bin.  It gives the spacing of index samples in terms of
 * pixels at this scale (which is then multiplied by the scale of a
 * keypoint).  It should be set experimentally to as small a value as
 * possible to keep features local (good values are in range 3 to 5). */
#define MAG_FACTOR 3

/** Width of Gaussian weighting window for index vector values.  It is
 * given relative to half-width of index, so value of 1.0 means that
 * weight has fallen to about half near corners of index patch.  A
 * value of 1.0 works slightly better than large values (which are
 * equivalent to not using weighting).  Value of 0.5 is considerably
 * worse. */
#define INDEX_SIGMA 1.0f

/** Index values are thresholded at this value so that regions with
 * high gradients do not need to match precisely in magnitude.
 * Best value should be determined experimentally.  Value of 1.0
 * has no effect.  Value of 0.2 is significantly better. */
#define MAX_INDEX_VAL 0.2f

/** Set SkipInterp to TRUE to skip the quadratic fit for accurate peak
 * interpolation within the pyramid.  This can be used to study the value
 * versus cost of interpolation. */
#define SKIP_INTERP false

/** These constants specify the size of the index vectors that provide
 * a descriptor for each keypoint.  The region around the keypoint is
 * sampled at OriSize orientations with IndexSize by IndexSize bins.
 * VecLength is the length of the resulting index vector. */
#define ORI_SIZE 8
#define INDEX_SIZE 4
#define VEC_LENGTH (INDEX_SIZE * INDEX_SIZE * ORI_SIZE)


// ------------------------------------------------------------------------- //
// WARNING: YOU ARE NOT SUPPOSED TO CHANGE ANYTHING BELOW THIS LINE! CHANGES //
// MAY LEAD TO COMPILE OR RUNTIME ERRORS!                                    //
// ------------------------------------------------------------------------- //
#if defined(USE_IPPI) || defined(USE_IPPI_MALLOC) || defined(USE_CIPPIMAGE)
#  define INCLUDE_IPPI
#endif

#if defined(USE_OPENCV)
#  define INCLUDE_OPENCV
#endif

#if defined(USE_CIPPIMAGE)
#  define INCLUDE_CIPPIMAGE
#endif


#if defined(__INTEL_COMPILER) || defined(__ICL) || defined(__ICC) || defined(__ECC)
#  pragma comment(lib, "libguide40.lib")
#endif

#ifdef INCLUDE_OPENCV
#  pragma comment(lib, "highgui.lib")
#  pragma comment(lib, "cv.lib")
#  pragma comment(lib, "cxcore.lib")
#endif

#ifdef INCLUDE_IPPI
#  pragma comment(lib, "ippi.lib")
#  pragma comment(lib, "ippcore.lib")
#  pragma comment(lib, "ippcc.lib")
#  pragma comment(lib, "ippcv.lib")
#endif

#ifdef ARX_MSVC
#  ifndef _SCL_SECURE_NO_DEPRECATE
#    define _SCL_SECURE_NO_DEPRECATE
#  endif
#  ifndef _CTR_SECURE_NO_DEPRECATE
#    define _CTR_SECURE_NO_DEPRECATE
#  endif
#endif

#define VERSION "0.1.7"

#ifndef PI
#  define PI 3.1415926535897932384626433832795f
#endif

#define EPS 1.0e-6f

#endif

