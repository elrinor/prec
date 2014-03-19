#ifndef __SIFT_KEYPOINTLIST_H__
#define __SIFT_KEYPOINTLIST_H__

#include "config.h"
#include <arx/Collections.h>

namespace prec {
// -------------------------------------------------------------------------- //
// KeyPointList
// -------------------------------------------------------------------------- //
  template<class Traits>
  class KeyPointList: public arx::ArrayList<typename Traits::key_type>, Traits {
  private:
    typedef typename Traits::key_type key_type;
    typedef typename Traits::key_data_type key_data_type;
    typedef typename Traits::key_data_list_type key_data_list_type;

    using ArrayList::add;
    using ArrayList::indexOf;
    using ArrayList::lastIndexOf;
    using ArrayList::push_back;
    using ArrayList::remove;
    using ArrayList::resize;
    using ArrayList::erase;
    using ArrayList::clear;

    arx::ArrayList<key_data_type> keyStorage;

  protected:
    explicit KeyPointList(key_data_list_type keyStorage): keyStorage(keyStorage) {
      for(key_data_list_type::iterator i = this->keyStorage.begin(); i != this->keyStorage.end(); i++)
        ArrayList::push_back(key_type(&*i));
    }

    friend Traits::extractor_impl_type;

  public:
    KeyPointList() {}
  };

} // namespace prec

#endif // __SIFT_KEYPOINTLIST_H__