#include "config.h"
#include "Image.h"
#include <fstream>

#ifdef INCLUDE_OPENCV
#  include <highgui.h>
#endif

#ifdef min
#  undef min
#endif

#ifdef max
#  undef max
#endif

using namespace std;

namespace prec {

// -------------------------------------------------------------------------- //
// BMP loading & saving
// -------------------------------------------------------------------------- //
#pragma pack(push, 1)
  struct BMPHeader {
    short type;           /**< File type = 0x4D42 */
    int size;       
    short reserved1;
    short reserved2;
    int offset;           /**< Offset from file start to bitmap data */
  };

  struct BMPInfoHeader {
    int size;             /**< Size of this structure in bytes */
    int width;
    int height;
    short planes;         /**< Should be equal to 1 */
    short bitsPerPixel;
    unsigned compression; /**< Compression flags ( 0 - no compression ) */
    unsigned imageSize;   /**< Size of image in bytes */
    int xPelsPerMeter;      
    int yPelsPerMeter;   
    int clrUsed;
    int clrImportant;
  };

  struct BMPPaletteItem {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
    unsigned char unused;
  };
#pragma pack(pop)

  template<class Derived>
  GenericImage<Color3b, Derived> loadBmp24bit(const std::string& fileName) {
    BMPHeader hdr;
    BMPInfoHeader infoHdr;

    ifstream f(fileName.c_str(), ios_base::in | ios_base::binary);
    if(!f.is_open())
      throw std::runtime_error("Could not open image file \"" + fileName + "\"");

    string errorMsg = "Error while reading image file \"" + fileName + "\": ";

    if(f.read((char *) &hdr, sizeof(hdr)).gcount() != sizeof(hdr))
      throw std::runtime_error(errorMsg + "could not read bitmap header");

    if(hdr.type != 0x4D42)
      throw std::runtime_error(errorMsg + "not a bitmap file");

    if(f.read((char *) &infoHdr, sizeof(infoHdr)).gcount() != sizeof(infoHdr))
      throw std::runtime_error(errorMsg + "could not read bitmap info header");

    if(infoHdr.bitsPerPixel != 24)
      throw std::runtime_error(errorMsg + "non-truecolor bitmaps are not supported");

    if(infoHdr.compression != 0)
      throw std::runtime_error(errorMsg + "compressed bitmaps are not supported");

    f.seekg(hdr.offset);
    if(f.fail())
      throw std::runtime_error(errorMsg + "seek failed");

    GenericImage<Color3b, Derived> result(infoHdr.width, infoHdr.height);

    int bmpStep = (result.getWidth() * 3 + 3) & -4;

    /* read bottom-up BMP */
    char *ptr = (char *) result.getRow(result.getHeight() - 1);
    for(int i = 0; i < result.getHeight(); i++)
      if(f.read(ptr - i * result.getWStep(), bmpStep).gcount() != bmpStep)
        throw std::runtime_error(errorMsg + "read failed");

    return result;
  }

  template<class Derived>
  void saveBmp24bit(GenericImage<Color3b, Derived> image, const std::string& fileName) {
    BMPHeader hdr;
    BMPInfoHeader infoHdr;

    ofstream f(fileName.c_str(), ios_base::out | ios_base::binary);
    if(!f.is_open())
      throw std::runtime_error("Could not open image file \"" + fileName + "\"");

    int bmpStep = (image.getWidth() * 3 + 3) & -4;
    int imageSize = bmpStep * image.getHeight() + sizeof(BMPHeader) + sizeof(BMPInfoHeader);
    hdr.type = 0x4D42;
    hdr.size = imageSize;
    hdr.reserved1 = 0;
    hdr.reserved2 = 0;
    hdr.offset = sizeof(BMPHeader) + sizeof(BMPInfoHeader);

    string errorMsg = "Error while reading image file \"" + fileName + "\": ";

    f.write((char *) &hdr, sizeof(hdr));

    infoHdr.size = sizeof(BMPInfoHeader);
    infoHdr.width = image.getWidth();
    infoHdr.height = image.getHeight();
    infoHdr.planes = 1;
    infoHdr.bitsPerPixel = 24;
    infoHdr.compression = 0;
    infoHdr.imageSize = imageSize;
    infoHdr.xPelsPerMeter = 0;
    infoHdr.yPelsPerMeter = 0;
    infoHdr.clrUsed = 0;
    infoHdr.clrImportant = 0;

    f.write((char *) &infoHdr, sizeof(infoHdr));

    /* write bottom-up BMP */
    char *ptr = (char *) image.getRow(image.getHeight() - 1);
    for(int i = 0; i < image.getHeight(); i++) 
      f.write(ptr - i * image.getWStep(), bmpStep);
  }


// -------------------------------------------------------------------------- //
// Image3b
// -------------------------------------------------------------------------- //
  Image3b Image3b::loadFromFile(const std::string& fileName) {
#ifdef USE_OPENCV
    IplImage *iplImage = cvLoadImage(fileName.c_str(), CV_LOAD_IMAGE_COLOR);
    if(iplImage == NULL)
      throw std::runtime_error("Could not open image file \"" + fileName + "\"");
    return Image3b(iplImage->width, iplImage->height, iplImage->widthStep, iplImage->imageData, new IplDeallocator(iplImage));
#else
    return loadBmp24bit<Image3b>(fileName);
#endif
  }

  void Image3b::saveToFile(const std::string& fileName) const {
#ifdef USE_OPENCV
    IplImage *iplImage = cvCreateImageHeader(cvSize(this->getWidth(), this->getHeight()), IPL_DEPTH_8U, 3);
    iplImage->widthStep = this->getWStep();
    iplImage->imageData = (char *) this->getPixelData();
    cvSaveImage(fileName.c_str(), iplImage);
    cvReleaseImageHeader(&iplImage);
#else
    saveBmp24bit(*this, fileName);
#endif
  }

  
// -------------------------------------------------------------------------- //
// Image1f
// -------------------------------------------------------------------------- //
#ifdef USE_IPPI
  Image1f Image1f::gaussianBlur(float sigma) const {
    int kernelSize = getGaussianKernelSize(sigma);
    Image1f result(this->getWidth(), this->getHeight());

    int bufferSize;
    ippiFilterGaussGetBufferSize_32f_C1R(this->getIppiSize(), kernelSize, &bufferSize);
    unsigned char *buffer = new unsigned char[bufferSize];
    ippiFilterGaussBorder_32f_C1R(this->getPixelData(), this->getWStep(), result.getPixelData(), result.getWStep(), this->getIppiSize(), kernelSize, sigma, ippBorderRepl, 0, buffer);
    delete[] buffer;

    return result;
  }

  Image1f Image1f::sub(const Image1f& that) const {
    if(this->getWidth() != that.getWidth() || this->getHeight() != that.getHeight())
      throw new std::runtime_error("Could not subtract images with different sizes");
    Image1f result(this->getWidth(), this->getHeight());
    ippiSub_32f_C1R(that.getPixelData(), that.getWStep(), this->getPixelData(), this->getWStep(), result.getPixelData(), result.getWStep(), this->getIppiSize());
    return result;
  }

  Image1f Image1f::resize(float widthRatio, float heightRatio) const {
    Image1f result((int) (this->getWidth() * widthRatio), (int) (this->getHeight() * heightRatio));
    ippiResize_32f_C1R(this->getPixelData(), this->getIppiSize(), this->getWStep(), this->getIppiRect(),
      result.getPixelData(), result.getWStep(), result.getIppiSize(), widthRatio, heightRatio, IPPI_INTER_LINEAR);
    return result;
  }

  void Image1f::fill(const color_type& value) {
    ippiSet_32f_C1R(value, this->getPixelData(), this->getWStep(), this->getIppiSize());
  }
#endif // USE_IPPI

  void Image1f::gradientMagAndDir(Image1f& magnitude, Image1f& direction) const {
    magnitude = Image1f(this->getWidth(), this->getHeight());
    direction = Image1f(this->getWidth(), this->getHeight());
    int w = this->getWStep() / sizeof(float);
    for(int y = 0; y < this->getHeight(); y++) {
      for(int x = 0; x < this->getWidth(); x++) {
        float xGrad, yGrad;
        if(x == 0)
          xGrad = 2.0f * (this->getPixel(x + 1, y) - this->getPixel(x, y));
        else if(x == this->getWidth() - 1)
          xGrad = 2.0f * (this->getPixel(x, y) - this->getPixel(x - 1, y));
        else
          xGrad = this->getPixel(x + 1, y) - this->getPixel(x - 1, y);
        if(y == 0)
          yGrad = 2.0f * (this->getPixel(x, y + 1) - this->getPixel(x, y));
        else if(y == this->getHeight() - 1)
          yGrad = 2.0f * (this->getPixel(x, y) - this->getPixel(x, y - 1));
        else
          yGrad = this->getPixel(x, y + 1) - this->getPixel(x, y - 1);
        magnitude.setPixel(x, y, sqrt(xGrad * xGrad + yGrad * yGrad));
        direction.setPixel(x, y, atan2(yGrad, xGrad));
      }
    }
  }



// -------------------------------------------------------------------------- //
// Image3f
// -------------------------------------------------------------------------- //
#ifdef USE_IPPI
  void Image3f::draw(const Image3f& that, int x, int y) {
    ippiCopy_32f_C3R(that.getPixelData(), that.getWStep(), this->getPixelDataAt(x, y), this->getWStep(), 
      ippiSize(min(that.getWidth(), this->getWidth() - x), min(that.getHeight(), this->getHeight() - y)));
  }

  void Image3f::draw(const Image3f& that, const arx::Matrix3f& m) {
    if(abs(m[2][0]) < 1.0e-7 && abs(m[2][1]) < 1.0e-7 && abs(m[2][2] - 1) < 1.0e-7) {
      /* It's affine transformation! */
      double coeffs[2][3];
      for(unsigned int r = 0; r < 2; r++)
        for(unsigned int c = 0; c < 3; c++)
          coeffs[r][c] = m[r][c];
      ippiWarpAffine_32f_C3R(that.getPixelData(), that.getIppiSize(), that.getWStep(), that.getIppiRect(), 
        this->getPixelData(), this->getWStep(), this->getIppiRect(), coeffs, IPPI_INTER_CUBIC | IPPI_SMOOTH_EDGE);
    } else {
      /* Perspective transformation... */
      double coeffs[3][3];
      for(unsigned int r = 0; r < 3; r++)
        for(unsigned int c = 0; c < 3; c++)
          coeffs[r][c] = m[r][c];
      ippiWarpPerspective_32f_C3R(that.getPixelData(), that.getIppiSize(), that.getWStep(), that.getIppiRect(), 
        this->getPixelData(), this->getWStep(), this->getIppiRect(), coeffs, IPPI_INTER_CUBIC | IPPI_SMOOTH_EDGE);
    }
  }
#endif

} // namespace prec

