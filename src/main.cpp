#include "config.h"
#include "PanoImage.h"
#include "matching/Matcher.h"
#include "Stitcher.h"
#include "Optimizer.h"

using namespace std;
using namespace arx;
using namespace prec;

/*
  TODO:
  
  sift coords *= 1 / sqrt(w * h)
    => set image-relative error

  pano -> optimizer -> stitcher
*/


void synopsis() {
  cout << "prec - Panorama Recognizer v" << VERSION << endl;
  cout << endl;
  cout << "USAGE:" << endl;
  cout << "  prec filename [options]" << endl;
  cout << endl;
  cout << "Possible options:" << endl;
  cout << "  -r  Mark all keypoints on a given image and output it into \"result.bmp\"" << endl;
  cout << "  -q  Quiet" << endl;
}

Image3f drawKeyPoints(ArrayList<SIFT> keys, Image3f image) {
  Image3f img = image.clone();
  for(unsigned int i = 0; i < keys.size(); i++) {
    float scale = keys[i].getScale();
    float angle = keys[i].getAngle();
    float x1 = keys[i].getX();
    float y1 = keys[i].getY();
    float x2 = x1 + 5.0f * scale * cos(angle);
    float y2 = y1 + 5.0f * scale * sin(angle);
    img.drawLine((int) x1, (int) y1, (int) x2, (int) y2, Color3f(1.0f, 0.0f, 0.0f));

    float x3, y3;
    x3 = x2 + 1.0f * scale * cos(angle - PI * 0.75f);
    y3 = y2 + 1.0f * scale * sin(angle - PI * 0.75f);
    img.drawLine((int) x2, (int) y2, (int) x3, (int) y3, Color3f(1.0f, 0.0f, 0.0f));

    x3 = x2 + 1.0f * scale * cos(angle + PI * 0.75f);
    y3 = y2 + 1.0f * scale * sin(angle + PI * 0.75f);
    img.drawLine((int) x2, (int) y2, (int) x3, (int) y3, Color3f(1.0f, 0.0f, 0.0f));
  }
  return img;
}

Image3f drawMatches(ArrayList<Match> matches, Image3f image0, Image3f image1) {
  Image3f img(image0.getWidth() + image1.getWidth(), max(image0.getHeight(), image1.getHeight()));
  int d = image0.getWidth();

  img.draw(image0, 0, 0);
  img.draw(image1, d, 0);

  for(unsigned int i = 0; i < matches.size(); i++)
    img.drawLine((int) matches[i].getKey(0).getX(), (int) matches[i].getKey(0).getY(), (int) matches[i].getKey(1).getX() + d, (int) matches[i].getKey(1).getY(), Color3f(1.0f, 0.0f, 0.0f));
  
  return img;
}

/*
int main(int argc, char** argv) {
  ArrayList<RGBImage> imageList;
  RGBImage image = RGBImage::loadFromFile("4a.jpg");
  imageList.push_back(RGBImage::loadFromFile("1a.jpg"));
  imageList.push_back(RGBImage::loadFromFile("2a.jpg"));
  imageList.push_back(RGBImage::loadFromFile("3a.jpg"));
  imageList.push_back(RGBImage::loadFromFile("5a.jpg"));
  imageList.push_back(RGBImage::loadFromFile("6a.jpg"));
  imageList.push_back(RGBImage::loadFromFile("7a.jpg"));
  ArrayList<unsigned int> result = Matcher(5, 50, true).matchSingleImage(image, imageList);
  for(unsigned int i = 0; i < result.size(); i++)
    cout << result[i] << " ";
  return 0;
}
*/

int main(int argc, char** argv) {
//  clock_t start = clock();

  arx::ArrayList<PanoImage> images;
  for(int i = 1; i < argc; i++)
    images.push_back(PanoImage(argv[i], 800, 600));

//  cout << (float) (clock() - start) / CLOCKS_PER_SEC << " secs" << endl;

  /*for(size_t i = 0; i < images.size(); i++)
    drawKeyPoints(images[i].getKeyPointList(), images[i].getOriginal()).saveToFile(images[i].getFileName() + ".keys.bmp");*/

  Matcher matcher(8, 20, true);
  ArrayList<Panorama> pans = matcher.matchImages(images);
  if(pans.size() == 0)
    return 0;


  for(size_t i = 0; i < pans.size(); i++) {
    Panorama p = pans[i];

    Optimizer optimizer;
    optimizer.optimize(p);

    
    Stitcher stitcher;
    stitcher.stitch(p).saveToFile(string() + "result_" + (char) ('a' + i) + ".jpg");
  }


/*
  Matcher m(8, 50, true);
  ArrayList<Component> comps = m.matchImages(keypoints);
  if(comps.size() == 0)
    return 0;
  Component c = comps[0];
  drawMatches(c.getMatches()[0].getMatches(), list[c.getMatches()[0].getImage0Index()], list[c.getMatches()[0].getImage1Index()]).saveToFile("r_ransac.bmp");

  BundleAdjuster a = BundleAdjuster();
  ArrayList<AdjustedImage> adjusted = a.adjust(c, list);

  Stitcher s;
  s.stitch(adjusted).saveToFile("result_levmar.bmp");

  s.stitch(c, list).saveToFile("result_ransac.bmp");
*/
  return 0;
}


