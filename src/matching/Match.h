#ifndef __MATCH_H__
#define __MATCH_H__

#include "config.h"
#include <functional>
#include <arx/Collections.h>

namespace prec {
// -------------------------------------------------------------------------- //
// Match
// -------------------------------------------------------------------------- //
  class Match {
  private:
    arx::array<SIFT, 2> keys;
    int distSqr; /**< Distance between 2 keypoints. TODO: remove. */

  public:
    Match() {}
    Match(SIFT key0, SIFT key1, int distSqr): distSqr(distSqr) {
      /* Match class must store KeyPoints in some fixed order on the basis of keypoint owner. 
       * I.e. in case we have several matches between two images, calls to getKey method of these 
       * matches with the same parameters must never return keypoints from different images. 
       *
       * Here we use tag field of keypoint. Unique PanoImage identifier is stored in tag, and 
       * therefore it provides exactly what we need. */
      if(key0.getTag() > key1.getTag()) {
        this->keys[0] = key0;
        this->keys[1] = key1;
      } else {
        this->keys[0] = key1;
        this->keys[1] = key0;
      }
    }
    
    arx::UnorderedPair<SIFT, KeyPointPtrComparer> getKeys() const { return arx::make_upair(this->keys[0], this->keys[1], KeyPointPtrComparer()); }
    SIFT getKey(std::size_t n) const { return this->keys[n]; }
    int getDistSqr() const { return this->distSqr; }
    bool isNull() const { return this->keys[0].isNull(); }
  };


// -------------------------------------------------------------------------- //
// MatchDistComparer
// -------------------------------------------------------------------------- //
  class MatchDistComparer: public std::binary_function<Match, Match, bool> {
  public:
    bool operator()(const Match& l, const Match& r) const {
      return l.getDistSqr() < r.getDistSqr();
    }
  };


} // namespace prec

#endif

