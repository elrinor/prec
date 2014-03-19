#ifndef __SIFT_KEYPOINT_H__
#define __SIFT_KEYPOINT_H__

#include "config.h"
#include <functional>
#include <arx/Collections.h>

namespace prec {
  namespace detail {
// -------------------------------------------------------------------------- //
// KeyPointData
// -------------------------------------------------------------------------- //
    template<class Traits>
    struct KeyPointData: Traits {
      typedef arx::array<unsigned char, VEC_LENGTH> FeatureVector;
      typedef typename Traits::tag_type tag_type;

      FeatureVector vec;
      float x, y;
      float scale;
      float angle;
      tag_type tag;

      KeyPointData(float x, float y, float scale, float angle): x(x), y(y), scale(scale), angle(angle) {};
    };
  } // namespace detail


// -------------------------------------------------------------------------- //
// KeyPoint
// -------------------------------------------------------------------------- //
  template<class Traits>
  class KeyPoint: Traits {
  private:
    typedef typename Traits::key_data_type key_data_type;
    typedef typename Traits::tag_type tag_type;
    typedef typename key_data_type::FeatureVector FeatureVector;

    key_data_type* data;

    KeyPoint(key_data_type* data): data(data) {}

    friend Traits::key_list_type;
    friend Traits::comparer_type;

  public:
    KeyPoint(float x, float y): data(new key_data_type(x, y, 0, 0)) { }


    float getX() const { return this->data->x; }
    float getY() const { return this->data->y; }
    void setX(float x) { this->data->x = x; }
    void setY(float y) { this->data->y = y; }

    arx::Vector2f getXY() const { return arx::Vector2f(this->data->x, this->data->y); }
    void setXY(arx::Vector2f xy) { this->data->x = xy[0]; this->data->y = xy[1]; }

    float getScale() const {return this->data->scale; }
    float getAngle() const { return this->data->angle; }
    
    tag_type getTag() const { return this->data->tag; }
    void setTag(const tag_type& tag) { this->data->tag = tag; }

    /* Stl-like array interface */
    typedef typename FeatureVector::value_type value_type;
    typedef typename FeatureVector::size_type size_type;
    enum {static_size = FeatureVector::static_size};

    const value_type& operator[] (size_type index) const { return this->data->vec[index]; }
    size_type size() { return this->data->vec.size(); }

    KeyPoint(): data(NULL) {};
    bool isNull() const { return this->data == NULL; }
    bool operator==(const KeyPoint& that) const { return this->data == that.data; }
  };


// -------------------------------------------------------------------------- //
// KeyPointPtrComparer
// -------------------------------------------------------------------------- //
  class KeyPointPtrComparer: public std::binary_function<void, void, bool> {
  public:
    template<class Tag>
    bool operator()(const KeyPoint<Tag>& l, const KeyPoint<Tag>& r) const {
      return l.data < r.data;
    }
  };

} // namespace prec

#endif // __SIFT_KEYPOINT_H__
