#include "config.h"

#include <cstdlib>
#include <iostream>
#include <ctime>
#include <cmath>
#include <string>
#include <vector>
#include <map>
#include <iomanip>
#include <algorithm>
#include <fstream>
//#include "arx/Mpl.h"
//#include "arx/Utility.h"
//#include "arx/KDTree.h"
//#include "arx/static_assert.h"

//#include "Image.h"

#include "arx/LinearAlgebra.h"
#include "arx/Collections.h"

//#include "LevMar.h"

#include "ippimage/bmp.h"
#include "ippimage/stdfileout.h"

#include "Image.h"

//#include "PanoImage.h"

#include <boost/preprocessor/facilities/is_empty.hpp>

#define DIM 128
#define COUNT 50000
#define TESTS 1000
#define LIST_ITEMS 6
#define BBF_ITERATIONS 150

namespace prec {

}

using namespace std;
using namespace arx;
using namespace prec;

class Point {
private:
  unsigned char* v;
public:
  static const int static_size = DIM;
  typedef unsigned char value_type;

  const unsigned char& operator[] (int index) const {
    return v[index];
  }
  unsigned char& operator[] (int index) {
    return v[index];
  }
  void random() {
    for(int j = 0; j < DIM; j++)
      v[j] = (unsigned char) ((256 * rand()) / (RAND_MAX + 1));
  }
  float dist(const Point& p) {
    float result;
    for(int j = 0; j < DIM; j++)
      result += sqr(v[j] - p.v[j]);
    return sqrt(result);
  }
  void allocate() {
    v = new unsigned char[DIM];
  }
  void deallocate() {
    delete[] v;
  }
};
/*
class MyLevMarModel {
public:
  typedef float value_type;

  int getParamNumber() const {
    return 4;
  }

  int getResidualNumber() const {
    return 6;
  }

  void start(RTVector<value_type>& p) {
    p[0] = 0;
    p[1] = 0;
    p[2] = 0;
    p[3] = 0;
  }

  void newIteration(const RTVector<value_type>& p) {
    return;
  }

  void getJacobian(RTMatrix<value_type>& target, const RTVector<value_type> p) {
    for(unsigned int r = 0; r < 6; r++)
      for(unsigned int c = 0; c < 4; c++)
        target[r][c] = 0;
    
    target[0][0] = 1;
    target[0][1] = 1;
    target[1][2] = 1;
    target[1][3] = 1;

    target[2][0] = 2;
    target[2][1] = 1;
    target[3][2] = 2;
    target[3][3] = 1;

    target[4][0] = 3;
    target[4][1] = 3;
    target[5][2] = 3;
    target[5][3] = 3;
  }

  void getResiduals(RTVector<value_type>& target, const RTVector<value_type> p) {
    target[0] = p[0] * 1 + p[1] * 1 - 3;
    target[1] = p[2] * 1 + p[3] * 1 - 3;
    target[2] = p[0] * 2 + p[1] * 1 + 1;
    target[3] = p[2] * 2 + p[3] * 1 - 3;
    target[4] = p[0] * 3 + p[1] * 3 - 9;
    target[5] = p[2] * 3 + p[3] * 3 - 9;
  }
};
*/
/*
class A {
public:
  int a() {
    return 1;
  }
};

class B:protected A {
public:
  int a() {
    return A::a();
  }
};


int main() {
  StaticVector<float, 4> v;
  v[0] = 10;

  return 0;
}*/

/*
int main() {

  StaticVector<float, 2> v0;
  DynamicVector<float> v1(2);

  v1 = createVectorAccessor(createVectorCache(v0 * 2));


  v0 = v1;

  StaticMatrix<float, 2, 2> m1, m0;
  DynamicMatrix<float, 2, 2> m2;
  DynamicMatrix<float, 2, 3> m5;
  DynamicMatrix<float> m3(2, 2);

  m1[0][0] = 1;
  m1[0][1] = 0;
  m1[1][0] = 0;
  m1[1][1] = 1;

  m2[0][0] = 0;
  m2[0][1] = 1;
  m2[1][0] = 1;
  m2[1][1] = 0;
  
  m0 = - - -m1;

  m3 = ((m1 * 2 + m2 * -2) / 2 + m2 * 2) / 2;


  StaticVector<float, 10> a, b, c;
  for(int i = 0; i < 10; i++) {
    a[i] = i;
    b[i] = 10 - i;
  }

  c = a + b;
  float k = c * (a + b) / 100;

  c = a;

  StaticVector<float, 3> x, y, z;
  x[0] = 1;
  x[1] = 0;
  x[2] = 0;
  y[0] = 0;
  y[1] = 1;
  y[2] = 0;
  z = (x ^ y) + x + y;

  cout << c[0];

  MyLevMarModel myModel;
  LevMar<MyLevMarModel> levMar;
  RTVector<float> result = levMar.fitModel(myModel);
  return 0;
}*/
/*
Point huh(unsigned char x, unsigned char y) {
  Point result;
  result.allocate();
  result[0] = x;
  result[1] = y;
  return result;
}*/


/*int main() {
  vector<Point> p;
  p.push_back(huh(0, 0));
  p.push_back(huh(1, 1));
  p.push_back(huh(6, 1));
  p.push_back(huh(1, 6));
  p.push_back(huh(2, 7));
  p.push_back(huh(3, 5));
  p.push_back(huh(9, 5));
  p.push_back(huh(7, 8));
  p.push_back(huh(5, 9));
  p.push_back(huh(8, 4));

  KDTree<Point> tree = KDTree<Point>::buildTree(p);

  Point c = huh(5, 5);
  KDTree<Point>::PointList result = tree.nearestNeighbourListBBF(c, 3, tree.estimateGoodBBFSearchDepth());

  return 0;
}*/
/*
int main() {
  {
    const Matrix<float, 2, 3> m(1);
    const Vector3f v(1);
    Vector2f vv = m * v;
  }
  
  {
    Matrix<float, 3, 3> a(0);
    Vector3f b(1, 2, 3);

    //Vector3f axis = (b[0] == 0) ? b : (b / 10.0f);

    b.x() = 0;

    b[2] = -1;
    a(0, 0) = 1;
    a(0, 2) = -1;
    a(1, 0) = 2;
    a(1, 1) = 1;
    a(1, 2) = 1;
    a(2, 2) = 2;

    a *= Matrix3f::identity();

    cout << a << endl << endl;
    cout << b << endl << endl;
    
    solveLinearSystem(a, b);

    cout << b << endl;


    Matrix3f m, m1(0);
    m[0][0] = 0;
    m[0][1] = 0;
    m[0][2] = 0;
    m[1] = m[0];
    m[2] = m[0];

    m1 = m;


    m(0, 0) = 2;
    m(1, 1) = 2;
    m(2, 2) = 2;

    m1.col(2) = m.row(1);

    Matrix3f mm(m);
    DynamicMatrix<float> dm(3, 3, 0), ddm(3, 3, 0), dddm(dm);
    dm[1][2] = 1;

    ddm = dm;

    mm += m;
    
    mm *= 0;

    mm(0, 2) = 1;
    mm(1, 1) = 1;
    mm(2, 0) = 1;

    for(int i = 0; i < 100; i++) {
      mm = (-mm + mm + mm + mm + mm + mm + mm + mm + mm + mm + mm) * mm;
    }

    cout << mm << "\n+\n" << m << "\n+\n" << m1 << "\n-\n" << dm << "\n=\n" << (mm + m + m1 - dm) << endl;

    m.data()[0] = 10000;

    cout << m[0][0];
  }
  return 0;
}
*/
/*
int main() {
  srand((unsigned int) time(NULL));
  
  vector<Point> p;
  for(int i = 0; i < COUNT; i++) {
    Point point;
    point.allocate();
    p.push_back(point);
  }

  cout << "*** Linear random" << endl;
  cout << "Generating " << COUNT << " points..." << endl;
  
  for(int i = 0; i < COUNT; i++)
    p[i].random();

  cout << "Building KD-Tree..." << endl;

  KDTree<Point> tree = KDTree<Point>::buildTree(p);

  Point c;
  c.allocate();

  float midDist;
  clock_t start;

  cout << "* Testing KD-Tree NN Search..." << endl;
  midDist = 0.0f;
  start = clock();
  for(int t = 0; t < TESTS; t++) {
    c.random();
    int outDistSqr;
    tree.nearestNeighbour(c, outDistSqr);
    midDist += sqrt((float) outDistSqr);
  }
  midDist /= TESTS;
  cout << "midDist == " << midDist << endl;
  cout << "time == " << (double)(clock() - start) / CLOCKS_PER_SEC << " secs" << endl;

  cout << "* Testing KD-Tree List Search (" << LIST_ITEMS << " items)..." << endl;
  midDist = 0.0f;
  start = clock();
  for(int t = 0; t < TESTS; t++) {
    c.random();
    KDTree<Point>::PointList list = tree.nearestNeighbourList(c, LIST_ITEMS);
    midDist+= sqrt((float) tree.nearestNeighbourList(c, LIST_ITEMS)[0].getDistSqr());
  }
  midDist /= TESTS;
  cout << "midDist == " << midDist << endl;
  cout << "time == " << (double)(clock() - start) / CLOCKS_PER_SEC << " secs" << endl;

  cout << "* Testing KD-Tree BBF List Search (" << LIST_ITEMS << " items, " << BBF_ITERATIONS << " iterations)..." << endl;
  midDist = 0.0f;
  start = clock();
  for(int t = 0; t < TESTS; t++) {
    c.random();
    midDist += sqrt((float) tree.nearestNeighbourListBBF(c, LIST_ITEMS, BBF_ITERATIONS)[0].getDistSqr());
  }
  midDist /= TESTS;
  cout << "midDist == " << midDist << endl;
  cout << "time == " << (double)(clock() - start) / CLOCKS_PER_SEC << " secs" << endl;

  cout << "* End of tests." << endl;
  string s;
  cin >> s;
}
*/

int main(int argc, char** argv) {
  Image1f image = Image1f::loadFromFile(argv[1]);

  for(int i = 1; i <= 500; i++)
    image.resize(i / 500.0f, i / 400.0f).saveToFile(string() + argv[1] + "." + (char)('0' + i / 100 % 10) + (char)('0' + i / 10 % 10) + (char)('0' + i % 10) + ".jpg");

  bool b = is_image<GenericImage<float, void> >::value;

  bool c = is_image<Color3f>::value;

  Image1f img = Image1f::loadFromFile("00000.jpg");
  //img = img.resize(0.5, 0.5);
  img = img.resizeDownNN<2>();
  img.saveToFile("00001.jpg");

  int gg = (int) 0.9f;

  float a = 1.0f;
  unsigned char c1 = color_cast<unsigned char>(a);
  Color4<float> c2 = 0.5f * color_cast<Color4<float> >(Color3<unsigned char>(255, 128, 0));
  c2 = c2 + c2;
  unsigned char cc = color_cast<unsigned char>(c2);

  unsigned char y = 123;
  unsigned char z = 123;
  unsigned char x = y * z / 200;

  Image3f im = Image3f::loadFromFile("000.jpg");
  Image4f over(im.getWidth() - 200, im.getHeight() - 200);
  over.fill(Color4f(0, 0, 0, 0.3f));
  im.drawBlended(over, 100, 100);
  im.saveToFile("000_0.jpg");


  Color4<unsigned char> v = color_cast<Color4<unsigned char> >(Color4f(0, 1, 0, 0.5));

  Color3<unsigned char> vv = color_cast<Color3<unsigned char> >(Color4f(0, 1, 0, 1));

  return 0;
}