#ifndef __SIFT_TRAITS_H__
#define __SIFT_TRAITS_H__

#include "config.h"
#include "KeyPoint.h"
#include "KeyPointList.h"
#include "Extractor.h"

namespace prec {
// -------------------------------------------------------------------------- //
// SIFTTraits
// -------------------------------------------------------------------------- //
  template<class Tag> class SIFTTraits {
  protected:
    typedef detail::KeyPointData<SIFTTraits> key_data_type;
    typedef arx::ArrayList<key_data_type> key_data_list_type;
    typedef detail::ExtractorImpl<SIFTTraits> extractor_impl_type;

  public:
    typedef Tag tag_type;
    typedef KeyPoint<SIFTTraits> key_type;
    typedef KeyPointList<SIFTTraits> key_list_type;
    typedef Extractor<SIFTTraits> extractor_type;
    typedef KeyPointPtrComparer comparer_type;
  };

} // namespace prec

#endif __SIFT_TRAITS_H__