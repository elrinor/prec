#ifndef __PREC_SIFT_H__
#define __PREC_SIFT_H__

#include "config.h"
#include "sift/SIFT.h"

namespace prec {
  typedef SIFTTraits<int>::key_type SIFT;
  typedef SIFTTraits<int>::extractor_type SIFTExtractor;
  typedef SIFTTraits<int>::key_list_type SIFTList;
  typedef SIFTTraits<int>::comparer_type SIFTPtrComparer;
}

#endif // __PREC_SIFT_H__
