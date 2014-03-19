#ifndef __ARX_UTILITY_H__
#define __ARX_UTILITY_H__

#include "config.h"
#include <functional>

#ifdef ARX_USE_BOOST
#  include <boost/noncopyable.hpp>
namespace arx {
  using boost::noncopyable;
}

#else // ARX_USE_BOOST

namespace arx {
  namespace noncopyable_adl_protected { // protection from unintended ADL
    /**
    * noncopyable class
    */
    class noncopyable {
    protected:
      noncopyable() {}
      ~noncopyable() {}
    private:
      noncopyable( const noncopyable& );
      const noncopyable& operator=( const noncopyable& );
    };
  }

  typedef noncopyable_adl_protected::noncopyable noncopyable;
}
#endif // ARX_USE_BOOST


namespace arx {
  /**
  * ValueType template is used to determine the type of the result of operator[] of type T
  */
  template<class T> class ValueType {
  public:
    typedef typename T::value_type type;
  };
  template<class T> class ValueType<T*> {
  public:
    typedef T type;
  };
  template<class T, int N> class ValueType<T[N]> {
  public:
    typedef T type;
  };


  /**
   * Square template
   */
  template<class T> T sqr(T x) {
    return x * x;
  }

  /**
   * UnorderedPair stores an unordered pair of two values of the same type.
   */
  template<class T, class Comparator = std::less<T> > class UnorderedPair: private Comparator {
  public:
    T first;
    T second;

    UnorderedPair() {}

    UnorderedPair(const T& a, const T& b, const Comparator& c = Comparator()): 
      Comparator(c), first(Comparator::operator()(a, b) ? a : b), second(Comparator::operator()(a, b) ? b : a) {}

    bool operator<(const UnorderedPair& that) const {
      return Comparator::operator()(this->first, that.first) ||
        (!Comparator::operator()(that.first, this->first) && Comparator::operator()(this->second, that.second));
    }

    bool operator>(const UnorderedPair& that) const {
      return that < *this;
    }

    bool operator>=(const UnorderedPair& that) const {
      return !(*this < that);
    }

    bool operator<=(const UnorderedPair& that) const {
      return !(*this > that);
    }

    bool operator==(const UnorderedPair& that) const {
      return *this >= that && *this <= that;
    }

    bool operator!=(const UnorderedPair& that) const {
      return !(*this == that);
    }
  };

  template<class T, class Comparator> 
  UnorderedPair<T, Comparator> make_upair(const T& a, const T& b, const Comparator& c) {
    return UnorderedPair<T, Comparator>(a, b, c);
  }

  template<class T> 
  UnorderedPair<T> make_upair(const T& a, const T& b) {
    return UnorderedPair<T>(a, b);
  }

} // namespace arx

#endif // __ARX_UTILITY_H__