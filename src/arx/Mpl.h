#ifndef __ARX_MPL_H__
#define __ARX_MPL_H__

#include "config.h"
#include "Preprocessor.h"

#ifdef ARX_USE_BOOST
#  include <boost/mpl/bool.hpp>
#  include <boost/mpl/int.hpp>
#  include <boost/mpl/long.hpp>
#  include <boost/mpl/integral_c.hpp>
#  include <boost/type_traits.hpp>
#  include <boost/mpl/if.hpp>
#  include <boost/mpl/logical.hpp>
#  include <boost/mpl/identity.hpp>
#  include <boost/mpl/vector.hpp>
#  include <boost/mpl/at.hpp>
#  include <boost/mpl/size.hpp>

namespace arx {
  using boost::mpl::bool_;
  using boost::mpl::int_;
  using boost::mpl::long_;
  using boost::mpl::integral_c;
  using boost::mpl::true_;
  using boost::mpl::false_;

  using boost::is_same;

  using boost::mpl::if_;
  using boost::mpl::if_c;

  using boost::mpl::and_;
  using boost::mpl::or_;
  using boost::mpl::not_;

  using boost::mpl::identity;

  using boost::mpl::size;
  using boost::mpl::at;

  using boost::mpl::vector1;
  using boost::mpl::vector2;
  using boost::mpl::vector3;
  using boost::mpl::vector4;
  using boost::mpl::vector5;
  using boost::mpl::vector6;
  using boost::mpl::vector7;
  using boost::mpl::vector8;
  using boost::mpl::vector9;
}
#else // ARX_USE_BOOST

namespace arx {
// -------------------------------------------------------------------------- //
// Integral constant types
// -------------------------------------------------------------------------- //
#define ARX_DEFINE_INTEGRAL_CONST_TYPE2(TYPE, WRAPPER, TEMPLATE)                \
  TEMPLATE struct WRAPPER {                                                     \
    enum {value = param};                                                       \
    typedef WRAPPER type;                                                       \
    typedef TYPE value_type;                                                    \
    operator TYPE () const {return this->value;}                                \
  };                                                                            \

#define ARX_DEFINE_INTEGRAL_CONST_TYPE(TYPE, WRAPPER)                           \
  ARX_DEFINE_INTEGRAL_CONST_TYPE2(TYPE, WRAPPER, template<TYPE param>)          \

  ARX_DEFINE_INTEGRAL_CONST_TYPE(bool, bool_)
  ARX_DEFINE_INTEGRAL_CONST_TYPE(int, int_)
  ARX_DEFINE_INTEGRAL_CONST_TYPE(long, long_)
  ARX_DEFINE_INTEGRAL_CONST_TYPE2(T, integral_c, template<class T ARX_COMMA() T param>)
#undef ARX_DEFINE_INTEGRAL_CONST_TYPE
#undef ARX_DEFINE_INTEGRAL_CONST_TYPE2

  typedef bool_<true> true_;
  typedef bool_<false> false_;


// -------------------------------------------------------------------------- //
// is_same
// -------------------------------------------------------------------------- //
  template<class A, class B> struct is_same {
    enum {value = false};
  };
  template<class T> struct is_same<T, T> {
    enum {value = true};
  };


// -------------------------------------------------------------------------- //
// if
// -------------------------------------------------------------------------- //
  template<bool Condition, class ThenType, class ElseType> struct if_c {
    typedef ThenType type;
  };

  template<class ThenType, class ElseType> struct if_c<false, ThenType, ElseType> {
    typedef ElseType type;
  };

  template<class Condition, class ThenType, class ElseType> struct if_ {
    typedef typename if_c<!!Condition::value, ThenType, ElseType>::type type;
  };

// -------------------------------------------------------------------------- //
// logical ops
// -------------------------------------------------------------------------- //
#define ARX_DEFINE_METAFUNC(TEMPLATE, NAME, TYPE, VALUE)                        \
  TEMPLATE struct NAME: TYPE<(VALUE)> {};

#define ARX_DEFINE_METAFUNC_1(NAME, TYPE, OP)                                   \
  ARX_DEFINE_METAFUNC(template<class T0>, NAME, TYPE, OP T0::value)

#define ARX_DEFINE_METAFUNC_2(NAME, TYPE, OP)                                   \
  ARX_DEFINE_METAFUNC(template<class T0 ARX_COMMA() class T1>, NAME, TYPE, (T0::value) OP (T1::value))

#define ARX_DEFINE_METAFUNC_N(NAME, TYPE, OP, DEFVALUE)                         \
  ARX_DEFINE_METAFUNC(template<class T0 ARX_COMMA() class T1 ARX_COMMA() class T2 = DEFVALUE ARX_COMMA() class T3 = DEFVALUE ARX_COMMA() class T4 = DEFVALUE>, NAME, TYPE, T0::value OP T1::value OP T2::value OP T3::value OP T4::value)

  ARX_DEFINE_METAFUNC_N(and_, bool_, &&, true_)
  ARX_DEFINE_METAFUNC_N(or_,  bool_, ||, false_)
  ARX_DEFINE_METAFUNC_1(not_, bool_, !)

  ARX_DEFINE_METAFUNC_N(plus,    long_, +, int_<0>)
  ARX_DEFINE_METAFUNC_2(minus,   long_, -)
  ARX_DEFINE_METAFUNC_N(times,   long_, *, int_<1>)
  ARX_DEFINE_METAFUNC_2(divides, long_, /)
  ARX_DEFINE_METAFUNC_2(modulus, long_, %)

  ARX_DEFINE_METAFUNC_2(equal_to,      bool_, ==)
  ARX_DEFINE_METAFUNC_2(not_equal_to,  bool_, !=)
  ARX_DEFINE_METAFUNC_2(less,          bool_, <)
  ARX_DEFINE_METAFUNC_2(greater,       bool_, >)
  ARX_DEFINE_METAFUNC_2(less_equal,    bool_, <=)
  ARX_DEFINE_METAFUNC_2(greater_equal, bool_, >=)
#undef ARX_DEFINE_LOGICAL_METAFUNC
#undef ARX_DEFINE_LOGICAL_METAFUNC_1
#undef ARX_DEFINE_LOGICAL_METAFUNC_N


// -------------------------------------------------------------------------- //
// Compile-time vector
// -------------------------------------------------------------------------- //
  template<long n> struct vector_tag;
  template<class Sequence> struct at_impl;
  template<class Sequence> struct size_impl;
  template<class Sequence> struct transform_impl;
  template<class Sequence> struct push_back_impl;
  template<class Sequence> struct pop_front_impl;
  template<class Sequence> struct remove_if_impl;

  template<class Vector, long n> struct v_at;
  template<class Vector, class Lambda, long n> struct v_transform;
  template<class Vector, class T, long n> struct v_push_back;
  template<class Vector, long n> struct v_pop_front;

  template<class Sequence, class N> struct at: at_impl<typename Sequence::tag>::template apply<Sequence, N> {};
  template<class Sequence> struct size: size_impl<typename Sequence::tag>::template apply<Sequence>::type {};
  template<class Sequence, class Lambda> struct transform: transform_impl<typename Sequence::tag>::template apply<Sequence, Lambda> {};
  template<class Sequence, class T> struct push_back: push_back_impl<typename Sequence::tag>::template apply<Sequence, T> {};
  template<class Sequence> struct pop_front: pop_front_impl<typename Sequence::tag>::template apply<Sequence> {};
  template<class Sequence, class Lambda> struct remove_if: remove_if_impl<typename Sequence::tag>::template apply<Sequence, Lambda> {};

#define ENUM_PARAMS_I(INDEX, ARGS) class ARX_JOIN(ARX_TUPLE_ELEM(2, 1, ARGS), INDEX) ARX_COMMA_IF(ARX_NOT_EQUAL(ARX_INC(INDEX), ARX_TUPLE_ELEM(2, 0, ARGS)))
#define ENUM_PARAMS(SIZE, PREFIX) ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(SIZE), ENUM_PARAMS_I, (SIZE, PREFIX))
#define ENUM_TYPEDEFS_I(INDEX, ARGS) typedef ARX_JOIN(ARX_TUPLE_ELEM(2, 0, ARGS), INDEX) ARX_JOIN(ARX_TUPLE_ELEM(2, 1, ARGS), INDEX);
#define ENUM_TYPEDEFS(SIZE, PREFIXA, PREFIXB) ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(SIZE), ENUM_TYPEDEFS_I, (PREFIXA, PREFIXB))
#define DEF_VECTOR_N(SIZE, ARG)                                                 \
  template<ENUM_PARAMS(SIZE, T) ARX_IF(ARX_EQUAL(SIZE, 0), class Dummy = void, ARX_EMPTY())> \
  struct ARX_JOIN(vector, SIZE) {                                               \
    typedef vector_tag<SIZE> tag;                                               \
    typedef ARX_JOIN(vector, SIZE) type;                                        \
    ENUM_TYPEDEFS(SIZE, T, item)                                                \
  };

#define DEF_V_AT_N(INDEX, ARG)                                                  \
  template<class V> struct v_at<V, INDEX> {                                     \
    typedef typename V::ARX_JOIN(item, INDEX) type;                             \
  };

#define TRANSFORM_ONE(INDEX, SIZE)                                              \
  typename Lambda::apply<typename at<V, long_<INDEX> >::type>::type ARX_COMMA_IF(ARX_NOT_EQUAL(ARX_INC(INDEX), SIZE))
#define DEF_V_TRANSFORM_N(SIZE, ARG)                                            \
  template<class V, class Lambda> struct v_transform<V, Lambda, SIZE> {         \
    typedef ARX_JOIN(vector, SIZE)<ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(SIZE), TRANSFORM_ONE, SIZE)> type; \
  };

#define PUSHBACK_ELEM(INDEX, SIZE)                                              \
  typename Vector::ARX_JOIN(item, INDEX) ARX_COMMA_IF(ARX_NOT_EQUAL(ARX_INC(INDEX), SIZE))
#define DEF_V_PUSH_BACK_N(SIZE, ARG)                                            \
  template<class Vector, class T> struct v_push_back<Vector, T, SIZE> {         \
    typedef ARX_JOIN(vector, ARX_INC(SIZE))<ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(SIZE), PUSHBACK_ELEM, SIZE) ARX_COMMA_IF(ARX_NOT_EQUAL(SIZE, 0)) T> type; \
  };

#define POPFRONT_ELEM(INDEX, SIZE)                                              \
  typename Vector::ARX_JOIN(item, ARX_INC(INDEX)) ARX_COMMA_IF(ARX_NOT_EQUAL(ARX_INC(INDEX), SIZE))
#define DEF_V_POP_FRONT_N(SIZE, ARG)                                            \
  template<class Vector> struct v_pop_front<Vector, SIZE> {                     \
    typedef ARX_JOIN(vector, ARX_DEC(SIZE))<ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(ARX_DEC(SIZE)), POPFRONT_ELEM, ARX_DEC(SIZE))> type; \
  };

  ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(25), DEF_VECTOR_N, ~)
  ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(25), DEF_V_AT_N, ~)
  ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(25), DEF_V_TRANSFORM_N, ~)
  ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(24), DEF_V_PUSH_BACK_N, ~)
  ARX_ARRAY_FOREACH(ARX_INDEX_ARRAY(25), DEF_V_POP_FRONT_N, ~)

  template<long n> struct at_impl<vector_tag<n> > {
    template<class Vector, class N> struct apply: v_at<Vector, N::value> {};
  };

  template<long n> struct size_impl<vector_tag<n> > {
    template<class Vector> struct apply: long_<n> {};
  };

  template<long n> struct transform_impl<vector_tag<n> > {
      template<class Vector, class Lambda> struct apply: v_transform<Vector, Lambda, size<Vector>::value> {};
  };

  template<long n> struct push_back_impl<vector_tag<n> > {
      template<class Vector, class T> struct apply: v_push_back<Vector, T, size<Vector>::value> {};
  };

  template<long n> struct pop_front_impl<vector_tag<n> > {
      template<class Vector> struct apply: v_pop_front<Vector, size<Vector>::value> {};
  };

#undef DEF_V_AT_N
#undef DEF_VECTOR_N
#undef DEF_V_TRANSFORM_N
#undef DEF_V_PUSH_BACK_N
#undef PUSHBACK_ELEM
#undef TRANSFORM_ONE
#undef ENUM_TYPEDEFS
#undef ENUM_TYPEDEFS_I
#undef ENUM_PARAMS
#undef ENUM_PARAMS_I

  template<long n> struct remove_if_impl<vector_tag<n> > {
    template<class Vector, class Result, class Lambda, long n> struct iterate {
      typedef typename at<Vector, int_<0> >::type elem;
      typedef typename iterate<typename pop_front<Vector>::type, typename if_<typename Lambda::apply<elem>, Result, typename push_back<Result, elem>::type>::type, Lambda, n - 1>::type type;
    };
    template<class Vector, class Result, class Lambda> struct iterate<Vector, Result, Lambda, 0> {
      typedef Result type;
    };
    template<class Vector, class Lambda> struct apply: public iterate<Vector, vector0<>, Lambda, size<Vector>::value> {};
  };


}
#endif // ARX_USE_BOOST



namespace arx {
// -------------------------------------------------------------------------- //
// is_scalar
// -------------------------------------------------------------------------- //
#define ARX_FOREACH_SIMPLE_TYPE(MACRO)                                          \
  MACRO(char)                                                                   \
  MACRO(short)                                                                  \
  MACRO(int)                                                                    \
  MACRO(long)                                                                   \
  MACRO(long long)                                                              \
  MACRO(unsigned char)                                                          \
  MACRO(unsigned short)                                                         \
  MACRO(unsigned int)                                                           \
  MACRO(unsigned long)                                                          \
  MACRO(unsigned long long)                                                     \
  MACRO(float)                                                                  \
  MACRO(double)                                                                 \
  MACRO(long double)                                                            \
  MACRO(const char)                                                             \
  MACRO(const short)                                                            \
  MACRO(const int)                                                              \
  MACRO(const long)                                                             \
  MACRO(const long long)                                                        \
  MACRO(const unsigned char)                                                    \
  MACRO(const unsigned short)                                                   \
  MACRO(const unsigned int)                                                     \
  MACRO(const unsigned long)                                                    \
  MACRO(const unsigned long long)                                               \
  MACRO(const float)                                                            \
  MACRO(const double)                                                           \
  MACRO(const long double)                                                      \

  template<class T> struct is_scalar {
    typedef false_ type;
    enum {value = false};
  };

#define ARX_IS_SCALAR_TYPE_SPECIALIZE2(_T, _X)                                  \
  template<_T> struct is_scalar<_X> {                                           \
    typedef true_ type;                                                         \
    enum {value = true};                                                        \
  };                                                                            \

#define ARX_IS_SCALAR_TYPE_SPECIALIZE(_X)                                       \
  ARX_IS_SCALAR_TYPE_SPECIALIZE2(ARX_EMPTY(), _X)                               \

ARX_FOREACH_SIMPLE_TYPE(ARX_IS_SCALAR_TYPE_SPECIALIZE)
ARX_IS_SCALAR_TYPE_SPECIALIZE2(class T, T*)
ARX_IS_SCALAR_TYPE_SPECIALIZE2(class T, const T*)
ARX_IS_SCALAR_TYPE_SPECIALIZE2(class T, T* const)
ARX_IS_SCALAR_TYPE_SPECIALIZE2(class T, const T* const)
#undef ARX_IS_SCALAR_TYPE_SPECIALIZE
#undef ARX_IS_SCALAR_TYPE_SPECIALIZE2

// -------------------------------------------------------------------------- //
// StoreModes
// -------------------------------------------------------------------------- //
  template<class T> struct reference_ {
    typedef T& type;
  };
  template<class T> struct const_reference_ {
    typedef T& type;
  };
  template<class T> struct pointer_ {
    typedef T* type;
  };
  template<class T> struct identity_ {
    typedef T type;
  };
  
// -------------------------------------------------------------------------- //
// Min & Max
// -------------------------------------------------------------------------- //
  template<class A, class B> struct max_ {
    enum {value = A::value > B::value ? A::value : B::value};
    typedef int_<value> type;
    typedef int value_type;
    operator int() const {return value;}
  };

  template<class A, class B> struct min_ {
    enum {value = A::value < B::value ? A::value : B::value};
    typedef int_<value> type;
    typedef int value_type;
    operator int() const {return value;}
  };


// -------------------------------------------------------------------------- //
// sizeof
// -------------------------------------------------------------------------- //
  template<class T> struct sizeof_ {
    enum {value = sizeof(T)};
    typedef int_<value> type;
    typedef int value_type;
    operator int() const {return value;}
  };

} // namespace arx

#endif








