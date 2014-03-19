#ifndef __ARX_STATIC_ASSERT_H__
#define __ARX_STATIC_ASSERT_H__

#include "config.h"
#include "Preprocessor.h"

#ifdef ARX_USE_BOOST
#  include <boost/static_assert.hpp>
#  define STATIC_ASSERT BOOST_STATIC_ASSERT
#else

namespace arx {
  namespace detail {
    template<bool x> struct STATIC_ASSERTION_FAILURE;
    template<> struct STATIC_ASSERTION_FAILURE<true> { enum {value = 1}; };
  }
}

#define STATIC_ASSERT(B)                                                        \
  typedef char ARX_JOIN(arx_static_assert_typedef_, __LINE__)                   \
  [::arx::detail::STATIC_ASSERTION_FAILURE<(bool)(B)>::value]                   \

#endif // ARX_USE_BOOST

#endif