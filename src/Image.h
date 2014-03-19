/**
 * @file
 *
 * This is ArX Image library file (will be included in next arxlib release).
 * 
 * Currently supported data types for images:
 * <ul>
 * <li> unsigned char
 * <li> float
 * </ul>
 *
 * Currently supported numbers of channels:
 * <ul>
 * <li> C1
 * <li> C3 (through Color3 template)
 * <li> AC4 (through Color4 template)
 * </ul>
 */
#ifndef __IMAGE_H__
#define __IMAGE_H__

#include "config.h"
#include <utility>
#include <exception>
#include <cassert>
#include <string>
#include <arx/smart_ptr.h>
#include <arx/Utility.h>
#include <arx/Mpl.h>
#include <arx/LinearAlgebra.h>

#ifdef INCLUDE_IPPI
#  include <ipp.h>
#endif

#ifdef INCLUDE_OPENCV
#  include <cxcore.h>
#endif

#ifdef INCLUDE_CIPPIMAGE
#  include "ippimage/ippimage.h"
#endif

namespace prec {
// -------------------------------------------------------------------------- //
// Operators
// -------------------------------------------------------------------------- //
  /**
   * Operators struct contains some functors which can ease
   * implementation of overloaded operators for complex data types.
   */
  template<class T>
  struct Operators {
#define DEFINE_BINARY_OPERATOR(OP_NAME, RET_TYPE, OP)                           \
    struct OP_NAME {                                                            \
      RET_TYPE operator()(const T& l, const T& r) const {                       \
        return OP;                                                              \
      }                                                                         \
    };

    DEFINE_BINARY_OPERATOR(Mul, T, l * r)
    DEFINE_BINARY_OPERATOR(Div, T, l / r)
    DEFINE_BINARY_OPERATOR(Add, T, l + r)
    DEFINE_BINARY_OPERATOR(Sub, T, l - r)
#undef DEFINE_BINARY_OPERATOR
  };


// -------------------------------------------------------------------------- //
// Colors
// -------------------------------------------------------------------------- //
  /**
   * This metafunction determines whether the given datatype is supported.
   */
  template<class T> struct is_datatype_supported: public arx::or_<arx::is_same<T, float>, arx::is_same<T, unsigned char> > {};


  /**
   * Defines min() and max() functions for supported simple color types.
   */
  template<class T> struct color_limits;

  template<> struct color_limits<unsigned char> {
    static unsigned char min() { return 0; }
    static unsigned char max() { return 255; }
  };

  template<> struct color_limits<float> {
    static float min() { return 0.0f; }
    static float max() { return 1.0f; }
  };


  /**
   * Data storage class for Color3
   */
  template<class T>
  struct Color3Data {
    enum { static_size = 3, processed_size = 3 };
    union {
      T asArray[3];
      struct {
        T b, g, r;
      };
    };
  };


  /**
   * Data storage class for Color4
   */
  template<class T>
  struct Color4Data {
    enum { static_size = 4, processed_size = 3 };
    union {
      T asArray[4];
      struct {
        T b, g, r, a;
      };
    };
  };


  /**
   * Base class for all composite colors.
   */
  template<class T, template<class> class Base, class Derived>
  struct ColorBase: public Base<T> {
  private:
    STATIC_ASSERT((is_datatype_supported<T>::value));

    template<int i, class Operator, class Y> 
    void performOne(const Y& v, const Operator& op) {
      this->asArray[i] = op(this->asArray[i], v);
    }

    template<int i, class Operator, class Y, class OtherDerived> 
    void performOne(const ColorBase<Y, Base, OtherDerived>& that, const Operator& op) {
      this->asArray[i] = op(this->asArray[i], that.asArray[i]);
    }

    template<int size, class Operator, class Param> 
    struct performer {
      void operator()(ColorBase& l, const Operator& op, const Param& r) {
        l.performOne<size - 1>(r, op);
        performer<size - 1, Operator, Param>()(l, op, r);
      }
    };

    template<class Operator, class Param> 
    struct performer<0, Operator, Param> {
      void operator()(ColorBase& l, const Operator& op, const Param& r) {
        return;
      }
    };

    struct AlphaMul {
      T operator()(const T& l, const T& r) const {
        return l * r / color_limits<T>::max();
      }
    };

  public:
    typedef T data_type;

    template<class Operator, class Param>
    Derived& perform(const Operator& op, const Param& r) {
      performer<processed_size, Operator, Param>()(*this, op, r);
      return static_cast<Derived&>(*this);
    }

    Derived& operator*=(const T& v) { 
      return perform(Operators<T>::Mul(), v);
    }

    Derived& operator/=(const T& v) {
      return perform(Operators<T>::Div(), v);
    }

    Derived& operator+=(const Derived& that) {
      return perform(Operators<T>::Add(), static_cast<const ColorBase&>(that));
    }

    Derived& operator-=(const Derived& that) { 
      return perform(Operators<T>::Sub(), static_cast<const ColorBase&>(that));
    }

    Derived& alphaMul(const data_type& alpha) {
      return perform(AlphaMul(), alpha);
    }
  };

  template<class T, class B, class D>
  D operator+(const ColorBase<T, B, D>& l, const ColorBase<T, B, D>& r) {
    return ColorBase<T, B, D>(l) += static_cast<const D&>(r);
  }

  template<class T, class B, class D>
  D operator-(const ColorBase<T, B, D>& l, const ColorBase<T, B, D>& r) {
    return ColorBase<T, B, D>(l) -= static_cast<const D&>(r);
  }

  template<class T, class B, class D, class Y>
  D operator*(const ColorBase<T, B, D>& l, const Y& r) {
    return ColorBase<T, B, D>(l) *= r;
  }

  template<class T, class B, class D, class Y>
  D operator*(const Y& l, const ColorBase<T, B, D>& r) {
    return ColorBase<T, B, D>(r) *= l;
  }

  template<class T, class B, class D, class Y>
  D operator/(const ColorBase<T, B, D>& l, const Y& r) {
    return ColorBase<T, B, D>(l) /= r;
  }


  /**
   * Class representing RGB triad.
   */
  template<class T>
  struct Color3: public ColorBase<T, Color3Data, Color3<T> > {
    Color3() { r = g = b = color_limits<T>::min(); }

    explicit Color3(const T& v) { r = g = b = v; }

    Color3(const T& cr, const T& cg, const T& cb) { r = cr; g = cg; b = cb; }
  };


  /**
   * Class representing RGBA quad.
   */
  template<class T>
  struct Color4: public ColorBase<T, Color4Data, Color4<T> > {
    Color4(): { r = g = b = color_limits<T>::min(); a = color_limits<T>::max(); }

    explicit Color4(const T& v) { r = g = b = v; a = color_limits<T>::max(); }

    Color4(const T& cr, const T& cg, const T& cb, const T& ca = color_limits<T>::max()) { r = cr; g = cg; b = cb; a = ca; }
  };


  /**
   * This metafunction determines the number of channels in a color type.
   */
  template<class T> struct channels: public arx::int_<1> { STATIC_ASSERT((is_datatype_supported<T>::value)); };
  template<class T> struct channels<Color3<T> >: public arx::int_<3> {};
  template<class T> struct channels<Color4<T> >: public arx::int_<4> {};


  /** 
   * This metafunction determines the datatype of a given color type. For example, for <tt>Color3<float></tt> it will return <tt>float</tt>. 
   */
  template<class T> struct datatype: public arx::identity_<T> { STATIC_ASSERT((is_datatype_supported<T>::value)); };
  template<class T> struct datatype<Color3<T> >: public arx::identity_<T> {};
  template<class T> struct datatype<Color4<T> >: public arx::identity_<T> {};


  /**
   * This metafunction returns color type with the given datatype and number of channels.
   */
  template<class datatype, int channels> struct compose_color;
  template<class datatype> struct compose_color<datatype, 1>: public arx::identity_<datatype> {};
  template<class datatype> struct compose_color<datatype, 3>: public arx::identity_<Color3<datatype> > {};
  template<class datatype> struct compose_color<datatype, 4>: public arx::identity_<Color4<datatype> > {};


  /* Color typedefs. */
  typedef Color3<float> Color3f;
  typedef Color3<unsigned char> Color3b;
  typedef Color4<float> Color4f;
  typedef Color4<unsigned char> Color4b;


  /**
   * This metafunction determines whether the given color type is supported.
   */
  template<class T> struct is_color_supported: public is_datatype_supported<T> {};
  template<class T> struct is_color_supported<Color3<T> >: public is_datatype_supported<T> {};
  template<class T> struct is_color_supported<Color4<T> >: public is_datatype_supported<T> {};

  
// -------------------------------------------------------------------------- //
// Color Conversion
// -------------------------------------------------------------------------- //
  /**
   * Implementation of <tt>color_cast</tt>.
   *
   * @see color_cast
   */
  template<class ToColor, class FromColor>
  struct color_cast_impl {
    typedef typename datatype<ToColor>::type ToDataType;
    typedef typename datatype<FromColor>::type FromDataType;

    struct cast_C1_C1 {
      ToColor operator()(const FromColor& c) {
        return static_cast<ToColor>(c * color_limits<ToDataType>::max() / color_limits<FromDataType>::max());
      }
    };

    struct cast_C4_C4 {
      ToColor operator()(const FromColor& c) {
        return ToColor(color_cast<ToDataType>(c.r), color_cast<ToDataType>(c.g), color_cast<ToDataType>(c.b), color_cast<ToDataType>(c.a));
      }
    };

    struct cast_C1_C34 {
      ToColor operator()(const FromColor& c) {
        return static_cast<ToColor>(0.299f * color_cast<ToDataType>(c.r) + 0.587f * color_cast<ToDataType>(c.g) + 0.114f * color_cast<ToDataType>(c.b));
      }
    };

    struct cast_C34_C1 {
      ToColor operator()(const FromColor& c) {
        return ToColor(color_cast<ToDataType>(c));
      }
    };

    struct cast_C34_C34 {
      ToColor operator()(const FromColor& c) {
        return ToColor(color_cast<ToDataType>(c.r), color_cast<ToDataType>(c.g), color_cast<ToDataType>(c.b));
      }
    };

    struct cast_EQ {
      ToColor operator()(const FromColor& c) {
        return c;
      }
    };

    template<int toChannels, int fromChannels> struct cast;

    template<> struct cast<1, 1>: public cast_C1_C1 {};
    template<> struct cast<1, 3>: public cast_C1_C34 {};
    template<> struct cast<1, 4>: public cast_C1_C34 {};
    template<> struct cast<3, 1>: public cast_C34_C1 {};
    template<> struct cast<3, 3>: public cast_C34_C34 {};
    template<> struct cast<3, 4>: public cast_C34_C34 {};
    template<> struct cast<4, 1>: public cast_C34_C1 {};
    template<> struct cast<4, 3>: public cast_C34_C34 {};
    template<> struct cast<4, 4>: public cast_C4_C4 {};

    ToColor operator()(const FromColor& c) {
      return arx::if_<arx::is_same<ToColor, FromColor>, cast_EQ, cast<channels<ToColor>::value, channels<FromColor>::value> >::type()(c);
    }
  };

  /**
   * Cast operator for colors.
   *
   * @param c                          Color to convert.
   *
   * @param ToColor                    Color type to convert to.
   * @param FromColor                  Color type to convert from.
   */
  template<class ToColor, class FromColor> 
  ToColor color_cast(const FromColor& c) {
    return color_cast_impl<ToColor, FromColor>()(c);
  }


// -------------------------------------------------------------------------- //
// Color Operations
// -------------------------------------------------------------------------- //
  namespace detail {
    /**
     * Implementation of <tt>alpha</tt> function for color types without alpha channel.
     *
     * @see alpha
     */
    template<class Color, bool isColor4>
    struct alpha_impl {
      typename datatype<Color>::type operator()(const Color& c) const {
        return color_limits<datatype<Color>::type>::max();
      }
    };

    /**
     * Implementation of <tt>alpha</tt> function for <tt>Color4</tt>.
     *
     * @see alpha
     */
    template<class Color>
    struct alpha_impl<Color, true> {
      typename datatype<Color>::type operator()(const Color& c) const {
        return c.a;
      }
    };
  } // namespace detail

  /**
   * @return                       Alpha component of a given color, even in case it does not actually have alpha channel.
   */
  template<class Color>
  static typename datatype<Color>::type alpha(const Color& c) {
    return detail::alpha_impl<Color, channels<Color>::value == 4>()(c);
  }


  namespace detail {
    /**
     * Implementation of <tt>withAlpha</tt> for color types without alpha channel.
     *
     * @see withAlpha
     */
    template<class Color, bool isColor4>
    struct with_alpha_impl {
      Color operator()(const Color& c, typename datatype<Color>::type alpha) const {
        return c;
      }
    };

    /**
     * Implementation of <tt>withAlpha</tt> for <tt>Color4</tt>.
     *
     * @see withAlpha
     */
    template<class Color>
    struct with_alpha_impl<Color, true> {
      Color operator()(const Color& c, typename datatype<Color>::type alpha) const {
        return Color(c.r, c.g, c.b, alpha);
      }
    };
  } // namespace detail

  /**
   * Replaces alpha component of a given color with a given value.
   *
   * @param c                      Color to replace alpha component in.
   * @param alpha                  New value for alpha component.
   * @return                       Newly created color object with alpha component set to the value given.
   */
  template<class Color>
  static Color withAlpha(const Color& c, typename datatype<Color>::type alpha) {
    return detail::with_alpha_impl<Color, channels<Color>::value == 4>()(c, alpha);
  }

  namespace detail {
    /**
     * Implementation of <tt>alphaMul</tt> for multi-channel color types.
     *
     * @see alphaMul
     */
    template<class Color, bool isColor1>
    struct alpha_mul_impl {
      Color operator()(const Color& c, typename datatype<Color>::type alpha) const {
        return Color(c).alphaMul(alpha);
      }
    };

    /**
     * Implementation of <tt>alphaMul</tt> for 1-channel color types.
     *
     * @see alphaMul
     */
    template<class Color>
    struct alpha_mul_impl<Color, true> {
      Color operator()(const Color& c, typename datatype<Color>::type alpha) const {
        return c * alpha / color_limits<c>::max();
      }
    };
  } // namespace detail

  /**
   * Implementation of <tt>alphaMul</tt> as a functor.
   *
   * @see alphaMul
   */
  template<class Color>
  struct alpha_mul {
    Color operator()(const Color& c, typename datatype<Color>::type alpha) const {
      return detail::alpha_mul_impl<Color, channels<Color>::value == 1>()(c, alpha);
    }
  };

  /**
   * Multiplies given color value by a given alpha (opacity) value. Takes result normalization into account.
   *
   * @param c                      Color to multiply.
   * @param alpha                  Alpha value.
   */
  template<class Color>
  static Color alphaMul(const Color& c, typename datatype<Color>::type alpha) {
    return alpha_mul<Color>()(c, alpha);
  }




// -------------------------------------------------------------------------- //
// Blend functions
// -------------------------------------------------------------------------- //
  /**
   * BlendFunc structure contains all blending functors.
   */
  struct BlendFunc {
    struct detail {
      /**
       * Conditional functor call.
       */
      template<bool cond, class L, class R, class Op>
      struct op_if {
        L operator()(const L& l, const R& r, const Op& op) const {
          return op(l, r);
        }
      };

      /**
       * Conditional functor call.
       */
      template<class L, class R, class Op>
      struct op_if<false, L, R, Op> {
        L operator()(const L& l, const R& r, const Op& op) const {
          return l;
        }
      };

      /**
       * Performs alpha multiplication if the given condition cond is true.
       *
       * @see alphaMul
       */
      template<bool cond, class Color>
      static Color alphaMulIf(const Color& c, typename datatype<Color>::type alpha) {
        return op_if<cond, Color, typename datatype<Color>::type, alpha_mul<Color> >()(c, alpha, alpha_mul<Color>());
      }
    };

    /**
     * OVER implements occlusion blending - r occludes l. <p>
     *
     * Alpha value: \f$ R_\alpha + (1 - R_\alpha) * L_\alpha \f$ <p>
     *
     * Color value: \f$ (R * R_\alpha + L * (1 - R_\alpha) * L_\alpha) / Result_\alpha \f$ <p>
     */
    template<bool resultPremul = false, bool lPremul = false, bool rPremul = false>
    struct OVER: public detail {
      template<class LeftColor, class RightDataType>
      LeftColor operator()(const LeftColor& l, const Color4<RightDataType>& r) const {
        typedef typename datatype<LeftColor>::type LeftDataType;
        const LeftDataType One            = color_limits<LeftDataType>::max();
        const LeftDataType rAlpha         = color_cast<LeftDataType>(alpha(r));
        const LeftDataType OneMinusRAlpha = One - rAlpha;
        const LeftDataType resultAlpha    = rAlpha + OneMinusRAlpha * alpha(l) / One;

        return 
          withAlpha(
            alphaMulIf<!resultPremul>(
              color_cast<LeftColor>(alphaMulIf<!rPremul>(r, alpha(r))) + alphaMul(alphaMulIf<!lPremul && channels<LeftColor>::value == 4>(l, alpha(l)), OneMinusRAlpha),
              sqr(One) / resultAlpha
            ), 
            resultAlpha
          );
      }
    };

    /**
     * PLUS implements blending without precedence. <p>
     *
     * Alpha value: \f$ R_\alpha + L_\alpha \f$ <p>
     *
     * Color value: \f$ (R * R_\alpha + L * L_\alpha) / Result_\alpha \f$ <p>
     */
    template<bool resultPremul = false, bool lPremul = false, bool rPremul = false>
    struct PLUS: public detail {
      template<class LeftColor, class RightDataType>
      LeftColor operator()(const LeftColor& l, const Color4<RightDataType>& r) const {
        typedef typename datatype<LeftColor>::type LeftDataType;
        const LeftDataType One            = color_limits<LeftDataType>::max();
        const LeftDataType resultAlpha    = alpha(l) + color_cast<LeftDataType>(alpha(r));

        return
          withAlpha(
            alphaMulIf<!resultPremul>(
              alphaMulIf<!lPremul && channels<LeftColor>::value == 4>(l, alpha(l)) + color_cast<LeftColor>(alphaMulIf<!rPremul>(r, alpha(r))),
              sqr(One) / resultAlpha
            ),
            resultAlpha
          );
      }
    };

    /**
     * NOBLEND implements normal drawing without blending. <p>
     *
     * Alpha value: \f$ R_\alpha \f$ <p>
     *
     * Color value: \f$ R \f$ <p>
     */
    struct NOBLEND {
      template<class LeftColor, class RightColor>
      LeftColor operator()(const LeftColor& l, const RightColor& r) const {
        return color_cast<LeftColor>(r);
      }
    };
  };


// -------------------------------------------------------------------------- //
// Forwards
// -------------------------------------------------------------------------- //
  template<class Color, class Derived, bool derivedMaterialized> class GenericImageBase;
  class Image3b;


// -------------------------------------------------------------------------- //
// Protectors
// -------------------------------------------------------------------------- //
  /**
   * Using this metafunction, expression types can protect materialized types from writing during 
   * expression evaluation. This is done by storing them by value, not by reference. By-value storage
   * increments reference counter of materialized type, and therefore prohibits writing to it.
   */
  template<class T> struct WriteProtected {
    typedef typename arx::if_c<T::materialized, T, T&>::type type;
  };

  /**
   * Using this metafunction, expression types can protect materialized types from writing during 
   * expression evaluation basing on run-time conditions. Type returned by this metafunction will not
   * always be readable, but it will always be constructible from T and T-assignable.
   */
  template<class T> struct WriteProtectedUnreadable {
    struct Nothing { Nothing() {} Nothing(const T&) {} Nothing& operator=(const T& that) { return *this; } };
    typedef typename arx::if_c<T::materialized, T, Nothing>::type type;
  };


// -------------------------------------------------------------------------- //
// Expressions
// -------------------------------------------------------------------------- //
  /**
   * Expression for image difference.
   */
  template<class L, class R> class GenericImageDifference: public GenericImageBase<typename L::color_type, GenericImageDifference<L, R>, false> {
  private:
    const L& l;
    const R& r;

    STATIC_ASSERT((arx::is_same<typename L::color_type, typename R::color_type>::value));

  public:
    typedef typename L::color_type color_type;

    GenericImageDifference(const L& l, const R& r): l(l), r(r) {
      if(l.getWidth() != r.getWidth() || l.getHeight() != r.getHeight())
        throw std::runtime_error("Could not subtract images with different sizes.");
    }

    int getWidth() const { return l.getWidth(); }

    int getHeight() const { return l.getHeight(); }

    color_type getPixelInterpolated(float x, float y) const {
      return l.getPixelInterpolated(x, y) - r.getPixelInterpolated(x, y);
    }

    color_type getPixel(int x, int y) const {
      return l.getPixel(x, y) - r.getPixel(x, y);
    }
  };


  /**
   * Expression for image resizing.
   */
  template<class L> class GenericImageResize: public GenericImageBase<typename L::color_type, GenericImageResize<L>, false> {
  private:
    float widthRatio;
    float heightRatio;
    int height;
    int width;

    const L& l;

    /** In case we're upscaling, we need to protect ourselves from the cases like <tt>img = img.resize(2, 2)</tt> when there is enough place in
     * image internal buffer. Such assignment will write pixels ahead, giving a mess instead of a desired result. */
    typename WriteProtectedUnreadable<L>::type protection;

  public:
    typedef typename L::color_type color_type;

    GenericImageResize(const L& l, float widthRatio, float heightRatio): l(l), widthRatio(widthRatio), heightRatio(heightRatio) {
      /* TODO: check these values... */
      static const float epsMod = 1.0e-5f;
      static const float epsCheck = 1.0e-4f;

      if(widthRatio <= 0 || heightRatio <= 0)
        throw std::runtime_error("Could not resize with non-positive scale factor.");

      /* Protect from write-ahead. */
      if(widthRatio > 1 || heightRatio > 1)
        protection = l;

      /* Calculate target size. */
      this->width = std::max(1, (int) (l.getWidth() * widthRatio));
      this->height = std::max(1, (int) (l.getHeight() * heightRatio));

      /* Problem: with calls to getPixelInterpolated we must stay in [0, W) x [0, H) rectangle. 
       * This is resolved by slightly tweaking factors... */
      if(width > 1 && (width - 1) / widthRatio > l.getWidth() - 1 - epsCheck)
        this->widthRatio = (float) (width - 1) / (l.getWidth() - 1) + epsMod;
      if(height > 1 && (height - 1) / heightRatio > l.getHeight() - 1 - epsCheck)
        this->heightRatio = (float) (height - 1) / (l.getHeight() - 1) + epsMod;
    }

    int getWidth() const { return this->width; }

    int getHeight() const { return this->height; }

    color_type getPixelInterpolated(float x, float y) const {
      return l.getPixelInterpolated(x / widthRatio, y / heightRatio); /* TODO: try to replace / with * and test */
    }

    color_type getPixel(int x, int y) const {
      return l.getPixelInterpolated(x / widthRatio, y / heightRatio); /* TODO: try to replace / with * and test */
    }
  };


  /**
   * Expression for image downscaling with constant integer factor.
   */
  template<class L, int n> class GenericImageResizeDownNx: public GenericImageBase<typename L::color_type, GenericImageResizeDownNx<L, n>, false> {
  private:
    const L& l;

    STATIC_ASSERT((n > 0));

  public:
    typedef typename L::color_type color_type;

    GenericImageResizeDownNx(const L& l): l(l) {}

    int getWidth() const { return std::max(1, l.getWidth() / n); }

    int getHeight() const { return std::max(1, l.getHeight() / n); }

    color_type getPixelInterpolated(float x, float y) const {
      return l.getPixelIterpolated(x * n, y * n);
    }

    color_type getPixel(int x, int y) const {
      return l.getPixel(x * n, y * n);
    }
  };

  /**
   * Expression for image color conversion.
   */
  template<class L, class ToColor> class GenericImageConvert: public GenericImageBase<ToColor, GenericImageConvert<L, ToColor>, false> {
  private:
    const L& l;

  public:
    typedef ToColor color_type;

    GenericImageConvert(const L& l): l(l) {}

    int getWidth() const { return l.getWidth(); }

    int getHeight() const { return l.getHeight(); }

    color_type getPixelInterpolated(float x, float y) const {
      return color_cast<color_type>(l.getPixelInterpolated(x, y));
    }

    color_type getPixel(int x, int y) const {
      return color_cast<color_type>(l.getPixel(x, y));
    }
  };


// -------------------------------------------------------------------------- //
// Image Compositions
// -------------------------------------------------------------------------- //
  /**
   * Composition of a color from one image and alpha channel from another.
   */
  template<class DataType, class ColorSource, class AlphaSource> class ImageAlphaComposition: 
    public GenericImageBase<Color4<DataType>, ImageAlphaComposition<DataType, ColorSource, AlphaSource>, false> {
  private:
    const ColorSource& color;
    const AlphaSource& alpha;

    STATIC_ASSERT((channels<typename AlphaSource::color_type>::value == 1));

  public:
    typedef Color4<DataType> color_type;

    ImageAlphaComposition(const ColorSource& color, const AlphaSource& alpha): color(color), alpha(alpha) {
      if(color.getWidth() != alpha.getWidth() || color.getHeight() != alpha.getHeight())
        throw std::runtime_error("Could not compose images with different sizes.");
    }

    int getWidth() const { return alpha.getWidth(); }

    int getHeight() const { return alpha.getHeight(); }

    color_type getPixelInterpolated(float x, float y) const {
      return withAlpha(color_cast<color_type>(color.getPixelInterpolated(x, y)), color_cast<DataType>(alpha.getPixelInterpolated(x, y)));
    }

    color_type getPixel(int x, int y) const {
      return withAlpha(color_cast<color_type>(color.getPixel(x, y)), color_cast<DataType>(alpha.getPixel(x, y)));
    }
  };

  /**
   * Factory function for ImageAlphaComposition.
   *
   * @see ImageAlphaComposition
   */
  template<class DataType, class ColorSource, class AlphaSource>
  ImageAlphaComposition<DataType, ColorSource, AlphaSource> createImageAlphaComposition(const ColorSource& color, const AlphaSource& alpha) {
    return ImageAlphaComposition<DataType, ColorSource, AlphaSource>(color, alpha);
  }

  /**
   * Constant image.
   */
  template<class Color> class ImageConstComposition: public GenericImageBase<Color, ImageConstComposition<Color>, false> {
  private:
    Color color;
    int width;
    int height;

  public:
    typedef Color color_type;

    ImageConstComposition(int width, int height, const Color& color): color(color), width(width), height(height) {}

    int getWidth() const { return width; }

    int getHeight() const { return height; }

    color_type getPixelInterpolated(float x, float y) const {
      return color;
    }

    color_type getPixel(int x, int y) const {
      return color;
    }
  };

  /**
   * Factory function for ImageConstComposition.
   *
   * @see ImageConstComposition
   */
  template<class Color>
  ImageConstComposition<Color> createImageConstComposition(int width, int height, const Color& color) {
    return ImageConstComposition<Color>(width, height, color);
  }


// -------------------------------------------------------------------------- //
// Image Conversion
// -------------------------------------------------------------------------- //
  /**
   * This metafunction returns whether the given class is an image class (i.e. derived from GenericImageBase).
   * It may not work correctly in some cases, however, and is used only to distinguish color and image classes.
   */
  template<class Class>
  struct is_image {
    template<class ToColor, class Derived, bool materialized> 
    static char check(const GenericImageBase<ToColor, Derived, materialized>& that);
    static int check(...);

    static Class c;

    enum { value = sizeof(check(c)) == sizeof(char) };
  };

  /** 
   * Implementation of image_cast for conversion to given color type. Returns expression template, no actual image allocation is performed.
   * Specialize if you need special behavior for conversion between some of the color type combinations.
   */
  template<class FromImage, class ToColor, class FromColor>
  struct image_cast_impl_base {
    typedef GenericImageConvert<FromImage, ToColor> return_type;
    return_type operator()(const FromImage& that) const {
      return return_type(that);
    }
  };

  namespace detail {
    /**
     * Implementation of image_cast for conversion to given image type. Converts given image to ToImage type, allocating memory for it if necessary.
     * Actual implementation calls image_cast to color type.
     */
    template<class ToImage, class FromImage, bool is_image> 
    struct image_cast_impl2 {
      typedef ToImage return_type;
      return_type operator()(const FromImage& that) const {
        return_type result = image_cast<ToImage::color_type>(that);
        return result;
      }
    };

    /**
     * Implementation of image_cast for conversion to given color type. Delegates everything to image_cast_impl_base.
     *
     * @see image_cast_impl_base
     */
    template<class ToColor, class FromImage> 
    struct image_cast_impl2<ToColor, FromImage, false>: public image_cast_impl_base<FromImage, ToColor, typename FromImage::color_type> {};
  } // namespace detail

  /**
   * Default implementation of color_cast - determines whether casting to color or to an image type and delegates everything to image_cast_impl2.
   * Specialize if you need special behavior.
   */
  template<class ToImageOrColor, class FromImage>
  struct image_cast_impl {
    typedef detail::image_cast_impl2<ToImageOrColor, FromImage, is_image<ToImageOrColor>::value> impl;
    typedef typename impl::return_type return_type;
    return_type operator()(const FromImage& that) const {
      return impl()(that);
    }
  };

  /**
   * Cast operator for images. Image can be casted to another image type, or to a color type. If casting to an image type,
   * memory will be allocated if necessary. Cast to color type will return expression template, which means that no allocation will be performed.
   *
   * @param ToImageOrColor             Image or color type to convert to.
   * @param FromImage                  Image type to convert from.
   */
  template<class ToImageOrColor, class FromImage>
  typename image_cast_impl<ToImageOrColor, FromImage>::return_type image_cast(const FromImage& that) {
    return image_cast_impl<ToImageOrColor, FromImage>()(that);
  }


// -------------------------------------------------------------------------- //
// Allocators
// -------------------------------------------------------------------------- //
  /** 
   * Helper functor that wraps several ippiMalloc functions and gives a convenient interface 
   * to access them not by name, but by pixel size. Dispatch is done statically, via template
   * specialization. Default implementation for non-standard pixel sizes is also provided. <p>
   * 
   * @param size                   Pixel size in bytes. 
   */
  template<unsigned int pixel_size> struct ImageAllocator {
    void* operator()(int widthPixels, int heightPixels, int* pStepBytes) const {
#if defined(USE_IPPI) || defined(USE_ALIGNED_IMAGE_ALLOCATION)
      *pStepBytes = (widthPixels * pixel_size + 32 - 1) & -32;
      char *ptr = new char[*pStepBytes * heightPixels + 32 + sizeof(char *) - 1];
      char *pixels = reinterpret_cast<char *>(reinterpret_cast<intptr_t>(ptr + 32 - 1) & -32);
      if(pixels - ptr < sizeof(char*))
        pixels = pixels + 32;
      *(reinterpret_cast<char **>(pixels) - 1) = ptr;
      return pixels;
#else
      *pStepBytes = widthPixels * pixel_size;
      return new char[width * height * pixel_size];
#endif
    }

    void operator()(void* ptr) const {
#if defined(USE_IPPI) || defined(USE_ALIGNED_IMAGE_ALLOCATION)
      delete[] *(reinterpret_cast<char **>(ptr) - 1);
#else
      delete[] ptr;
#endif 
    }
  };

#ifdef USE_IPPI_MALLOC
#  define SPECIALIZE_IPPIMALLOC(SIZE, NAME)                                     \
  template<> struct ImageAllocator<SIZE> {                                      \
    void* operator()(int widthPixels, int heightPixels, int* pStepBytes) {      \
      return static_cast<void*>(NAME(widthPixels, heightPixels, pStepBytes));   \
    }                                                                           \
    void operator()(void* ptr) {                                                \
      ippiFree(ptr);                                                            \
    }                                                                           \
  };
  SPECIALIZE_IPPIMALLOC(1, ippiMalloc_8u_C1)
  SPECIALIZE_IPPIMALLOC(2, ippiMalloc_16u_C1)
  SPECIALIZE_IPPIMALLOC(3, ippiMalloc_8u_C3)
  SPECIALIZE_IPPIMALLOC(4, ippiMalloc_32f_C1)
  SPECIALIZE_IPPIMALLOC(6, ippiMalloc_16u_C3)
  SPECIALIZE_IPPIMALLOC(8, ippiMalloc_32f_C2)
  SPECIALIZE_IPPIMALLOC(12, ippiMalloc_32f_C3)
  SPECIALIZE_IPPIMALLOC(16, ippiMalloc_32f_C4)
#  undef SPECIALIZE_IPPIMALLOC
#endif


// -------------------------------------------------------------------------- //
// Deallocators
// -------------------------------------------------------------------------- //
  /** Abstract base class for image deallocators. */
  class ImageDeallocator {
  public:
    virtual void operator()() = 0;
  };

  /** ImageDeallocator that does nothing - useful for creating "views" into other image types. */
  class EmptyImageDeallocator: public ImageDeallocator {
  public:
    virtual void operator()() {};
  };

  /** 
   * ImageDeallocator that uses the same routine as GenericImageData class.
   *
   * @param size                   Pixel size in bytes. 
   *
   * @see ImageAllocator
   */
  template<int pixel_size>
  class DefaultImageDeallocator {
  private:
    void* pixels;

  public:
    DefaultImageDeallocator(void* pixels): pixels(pixels) {}

    virtual void operator()() {
      /* We're presuming that ImageAllocator is stateless. */
      ImageAllocator<pixel_size>()(this->pixels);
    }
  };

#ifdef USE_OPENCV
  /** ImageDeallocator for IplImage class. Used for implementation of "strict ownership" semantics. */
  class IplDeallocator: public ImageDeallocator {
  private:
    IplImage* iplImage;

  public:
    IplDeallocator(IplImage* iplImage): iplImage(iplImage) {}

    virtual void operator()() {
      cvReleaseImage(&this->iplImage);
    }
  };
#endif

// -------------------------------------------------------------------------- //
// GenericImageData
// -------------------------------------------------------------------------- //
  /**
   * Internal class that stores all image information and handles data allocation and deallocation.
   *
   * @param pixel_size                 Size of a single image pixel in bytes.
   */
  template<int pixel_size>
  struct GenericImageData: public arx::noncopyable {
    void* pixels; /**< Pointer to pixel buffer. */
    int width; /**< Width of an image in pixels. */
    int height; /**< Height of an image in pixels. */
    int bufferWidth; /**< Width of an image buffer in pixels. */
    int bufferHeight; /**< Height of an image buffer in pixels. */
    int wStep; /**< Size of aligned image row in bytes. */
    ImageDeallocator* deallocator; /**< ImageDeallocator to use with this image. */

    /** Constructor. */
    GenericImageData(int width, int height): width(width), height(height), deallocator(NULL) {
      assert(width > 0 && height > 0);

      /* Allocate memory. */
      this->pixels = ImageAllocator<pixel_size>()(width, height, &this->wStep);

      /* Store actual buffer size. */
      this->bufferHeight = this->height;
      this->bufferWidth = this->wStep / pixel_size;
    }

    /** Constructor. */
    GenericImageData(int width, int height, int wStep, void* pixels, ImageDeallocator* deallocator): 
      width(width), height(height), wStep(wStep), pixels(pixels), deallocator(deallocator) {
      assert(width > 0 && height > 0 && wStep >= (width * pixel_size) && pixels != NULL);
    }

    /** Destructor. */
    ~GenericImageData() {
      if(deallocator == NULL) {
        ImageAllocator<pixel_size>()(this->pixels);
      } else {
        (*this->deallocator)();
        delete this->deallocator;
      }
    }
  };


// -------------------------------------------------------------------------- //
// GenericImage
// -------------------------------------------------------------------------- //
  /**
   * Base class for all images. Can be used directly, or derived from.
   *
   * @param Color                      Class representing single pixel in an image. 
   * @param Derived                    Derived class, if any.
   */
  template<class Color, class Derived = void> class GenericImage: 
    public GenericImageBase<Color, typename arx::if_<arx::is_same<Derived, void>, GenericImage<Color, Derived>, Derived>::type, true> {
  public:
    typedef Color color_type; /**< Type of a single pixel. */
    enum { pixel_size = sizeof(color_type) /**< Size in bytes of a single pixel. */ };
    typedef typename datatype<color_type>::type color_data_type; /**< Type of one channel of a pixel. */

  protected:
    typedef GenericImage base_type; /**< Base type for easy access in derived classes. */
    typedef GenericImageData<pixel_size> storage_type; /**< Storage type. */
    typedef typename arx::if_<arx::is_same<Derived, void>, base_type, Derived>::type derived_type; /**< Derived type, or this type, if none. */

  private:
    arx::shared_ptr<storage_type> data; /**< Shared data. */ 

    template<class OtherColor, class OtherDerived> friend class GenericImage;

  protected:
    /** @return reference to pixel at (x, y) */
    color_type& getPixelReference(int x, int y) const {
      assert(x >= 0 && y >= 0 && x < this->data->width && y < this->data->height);
      return *reinterpret_cast<color_type*>(reinterpret_cast<char*>(this->data->pixels) + y * this->data->wStep + x * pixel_size);
    }

    /** Sets the pixel at (x, y) to given value. If the pixel lies outside the image boundaries, does nothing. */
    void checkedSetPixel(int x, int y, const color_type& value) {
      if(x < 0 || x >= this->data->width || y < 0 || y >= this->data->height)
        return;
      setPixel(x, y, value);
    }

#if defined(USE_IPPI) || defined(USE_CIPPIMAGE)
    /** Factory function for IppiSize */
    static IppiSize ippiSize(int width, int height) {
      IppiSize size = {width, height};
      return size;
    }

    /** Factory function for IppiRect */
    static IppiRect ippiRect(int x, int y, int width, int height) {
      IppiRect rect = {x, y, width, height};
      return rect;
    }
#endif

  public:
    /** 
     * Constructor. Creates uninitialized image. 
     */
    GenericImage() {}

    /** 
     * Constructor. Allocates an image of size width x height. 
     *
     * @param width                    Image width in pixels.
     * @param height                   Image height in pixels.
     **/
    GenericImage(int width, int height): data(new storage_type(width, height)) {}

    /** 
     * Constructor. Does not allocate image data buffer, but uses provided one instead. 
     * You can implement ownership semantics by providing appropriate deallocator.
     * Please note that in case deallocator == NULL, standard deallocation routine is used.
     *
     * @param width                    Image width in pixels.
     * @param height                   Image height in pixels.
     * @param wStep                    Size of aligned image row in bytes.
     * @param pixels                   Pointer to image data buffer. 
     * @param deallocator              ImageDeallocator to use with this image instance.
     **/
    GenericImage(int width, int height, int wStep, void* pixels, ImageDeallocator* deallocator): 
      data(new storage_type(width, height, wStep, pixels, deallocator)) {}

    /** @return true if image data is allocated, false otherwise. */
    bool isNull() const { return this->data.get() == NULL; }

    /** @return Image width in pixels. */
    int getWidth() const { return this->data->width; }

    /** @return Image height in pixels.  */
    int getHeight() const { return this->data->height; }

    /** @return Size of aligned image row in bytes. */
    int getWStep() const { return this->data->wStep; }

    /** @return Pointer to image row y. */
    const color_type* getRow(int y) const { return &getPixelReference(0, y); }

    /** @return Pointer to image row y. */
    color_type* getRow(int y) { return &getPixelReference(0, y); }

    /** @return Pointer to image data buffer. */
    const color_data_type* getPixelData() const { return (color_data_type *) &getPixelReference(0, 0); }

    /** @return Pointer to image data buffer. */
    color_data_type* getPixelData() { return (color_data_type *) &getPixelReference(0, 0); }

    /** @return Pointer to pixel at (x, y). */
    const color_data_type* getPixelDataAt(int x, int y) const { return (color_data_type *) &getPixelReference(x, y); }

    /** @return Pointer to pixel at (x, y). */
    color_data_type* getPixelDataAt(int x, int y) { return (color_data_type *) &getPixelReference(x, y); }

#if defined(USE_IPPI) || defined(USE_CIPPIMAGE)
    /** @return IppiSize of this image. */
    IppiSize getIppiSize() const { return ippiSize(this->data->width, this->data->height); }

    /** @return IppiRect containing this image. */
    IppiRect getIppiRect() const { return ippiRect(0, 0, this->data->width, this->data->height); }
#endif

    /** @return Pixel at (x, y), interpolated linearly. */
    color_type getPixelInterpolated(float x, float y) const {
      int ix = (int) x;
      int iy = (int) y;
      float fx = x - ix;
      float fy = y - iy;
      return (1 - fy) * ((1 - fx) * getPixel(ix, iy    ) + fx * getPixel(ix + 1, iy    )) +
                  fy  * ((1 - fx) * getPixel(ix, iy + 1) + fx * getPixel(ix + 1, iy + 1));
    }

    /** @return Pixel at (x, y). */
    const color_type& getPixel(int x, int y) const {
      return getPixelReference(x, y);
    }

    /** Sets the pixel at (x, y) to value. */
    void setPixel(int x, int y, const color_type& value) {
      getPixelReference(x, y) = value;
    }

    /** 
     * Draws a line that connects two points (x1, y1) and (x2, y2) using the given color value. 
     */
    void drawLine(int x1, int y1, int x2, int y2, const color_type& value) {
      int dx, dy;

      if(x1 == x2 && y1 == y2) {
        checkedSetPixel(x1, y1, value);
      } else if(abs(x2 - x1) > abs(y2 - y1)) {
        if(x1 > x2) {
          std::swap(x1, x2);
          std::swap(y1, y2);
        }
        dx = x2 - x1;
        dy = y2 - y1;
        for(int i = x1; i <= x2; i++)
          checkedSetPixel(i, y1 + (i - x1) * dy / dx, value);
      } else {
        if(y1 > y2) {
          std::swap(x1, x2);
          std::swap(y1, y2);
        }
        dx = x2 - x1;
        dy = y2 - y1;
        for(int i = y1; i <= y2; i++)
          checkedSetPixel(x1 + (i - y1) * dx / dy, i, value);
      }
    }

    /**
     * Loads an image from a file.
     */
    static derived_type loadFromFile(const std::string& fileName) {
      return Image3b::loadFromFile(fileName).convert<color_type>(); 
    }

    /** 
     * Sets all pixels of an image to a value.
     * 
     * @param value                    Constant value assigned to each pixel in the image.
     */
    void fill(const color_type& value) {
      for(int y = 0; y < this->getHeight(); y++)
        for(int x = 0; x < this->getWidth(); x++)
          this->setPixel(x, y, value);
    }

    /**
     * Draws the given image on this image, so that the upper-left corner of the given image is drawn at (x, y).
     * Takes alpha component into account.
     * 
     * @param that                     Image to draw.
     * @param x, y                     Coordinates of an upper-left corner of the given image relative to this image.
     * @param blendFunc                Blending function to use.
     */
    template<class OtherColor, class OtherDerived, bool otherMaterialized, class BlendFunction>
    void drawBlended(const GenericImageBase<OtherColor, OtherDerived, otherMaterialized>& givenThat, int x, int y, const BlendFunction& blendFunc) {
      const OtherDerived& that = static_cast<const OtherDerived&>(givenThat);
      int x0 = x, x1 = min(x + that.getWidth(), this->getWidth());
      int y0 = y, y1 = min(y + that.getHeight(), this->getHeight());
      for(int iy = y0; iy < y1; iy++)
        for(int ix = x0; ix < x1; ix++)
          this->setPixel(ix, iy, blendFunc(this->getPixel(ix, iy), that.getPixel(ix - x, iy - y)));
    }

    /** 
     * Draws the given image on this image, so that the upper-left corner of the given image is drawn at (x, y).
     * 
     * @param that                     Image to draw.
     * @param x, y                     Coordinates of an upper-left corner of the given image relative to this image.
     */
    template<class OtherColor, class OtherDerived, bool otherMaterialized>
    void draw(const GenericImageBase<OtherColor, OtherDerived, otherMaterialized>& that, int x, int y) {
      return drawBlended(that, x, y, BlendFunc::NOBLEND());
    }

    /**
     * Convenient overload for standard non-premultiplied occlusion blending. Uses BlendFunc::OVER<> as blending function.
     *
     * @param that                     Image to draw.
     * @param x, y                     Coordinates of an upper-left corner of the given image relative to this image.
     *
     * @see drawBlended
     */
    template<class OtherDataType, class OtherDerived, bool otherMaterialized>
    void drawBlended(const GenericImageBase<Color4<OtherDataType>, OtherDerived, otherMaterialized>& that, int x, int y) {
      drawBlended(that, x, y, BlendFunc::OVER<>());
    }

    /** 
     * Draws the given image on this image using the given matrix for coordinate transformation.
     * Takes alpha component into account.
     *
     * @param that                     Image to draw.
     * @param m                        Transformation matrix.
     * @param blendFunc                Blending function to use.
     */
    template<class OtherColor, class OtherDerived, bool otherMaterialized, class BlendFunction>
    void drawBlended(const GenericImageBase<OtherColor, OtherDerived, otherMaterialized>& givenThat, const arx::Matrix3f& m, const BlendFunction& blendFunc) {
      using namespace arx;
      using namespace std;

      const OtherDerived& that = static_cast<const OtherDerived&>(givenThat);

      Vector3f v[4];
      v[0] = m * Vector3f(0, 0, 1);
      v[1] = m * Vector3f((float) that.getWidth(), 0, 1);
      v[2] = m * Vector3f((float) that.getWidth(), (float) that.getHeight(), 1);
      v[3] = m * Vector3f(0, (float) that.getHeight(), 1);
      for(int i = 0; i < 4; i++)
        v[i] /= v[i][2];
      
      int x0 = (int) max(min(min(v[0][0], v[1][0]), min(v[2][0], v[3][0])), 0.0f);
      int x1 = (int) min(max(max(v[0][0], v[1][0]), max(v[2][0], v[3][0])) + 1.0f, (float) this->getWidth());
      int y0 = (int) max(min(min(v[0][1], v[1][1]), min(v[2][1], v[3][1])), 0.0f);
      int y1 = (int) min(max(max(v[0][1], v[1][1]), max(v[2][1], v[3][1])) + 1.0f, (float) this->getHeight());

      Matrix3f m_1 = m.inverse();

      for(int y = y0; y < y1; y++) {
        for(int x = x0; x < x1; x++) {
          Vector3f v = m_1 * Vector3f((float) x, (float) y, 1);
          v /= v[2];
          if(v[0] >= 0 && v[0] < that.getWidth() - 1 && v[1] >= 0 && v[1] < that.getHeight() - 1)
            this->setPixel(x, y, blendFunc(this->getPixel(x, y), that.getPixelInterpolated(v[0], v[1])));
        }
      }
    }

    /** 
     * Draws the given image on this image using the given matrix for coordinate transformation.
     *
     * @param that                     Image to draw.
     * @param m                        Transformation matrix.
     */
    template<class OtherColor, class OtherDerived, bool otherMaterialized>
    void draw(const GenericImageBase<OtherColor, OtherDerived, otherMaterialized>& that, const arx::Matrix3f& m) {
      drawBlended(that, m, BlendFunc::NOBLEND());
    }

    /**
     * Convenient overload for standard non-premultiplied occlusion blending. Uses BlendFunc::OVER<> as blending function.
     *
     * @param that                     Image to draw.
     * @param m                        Transformation matrix.
     *
     * @see drawBlended
     */
    template<class OtherColor, class OtherDerived, bool otherMaterialized>
    void drawBlended(const GenericImageBase<OtherColor, OtherDerived, otherMaterialized>& that, const arx::Matrix3f& m) {
      drawBlended(that, m, BlendFunc::OVER<>());
    }

    /**
     * Like operator=, but always performs copying.
     */
    template<class OtherDerived, bool otherMaterialized>
    derived_type& assign(const GenericImageBase<color_type, OtherDerived, otherMaterialized>& givenThat) {
      const OtherDerived& that = static_cast<const OtherDerived&>(givenThat);

      /* Check whether it's possible to use our current memory. If not, allocate new. */
      if(!this->data || !this->data.unique() || this->data->bufferWidth < that.getWidth() || this->data->bufferHeight < that.getHeight())
        this->data.reset(new storage_type(that.getWidth(), that.getHeight()));

      /* Assign. */
      for(int y = 0; y < that.getHeight(); y++)
        for(int x = 0; x < that.getWidth(); x++)
          this->setPixel(x, y, that.getPixel(x, y));

      /* Change size. We need to do it AFTER assignment because it can be referenced from inside the expression. 
       * It is not needed in case we have allocated data anew, but I don't see any point in protecting it with if. */
      this->data->width = that.getWidth();
      this->data->height = that.getHeight();

      return this->ref();
    }

    /**
     * Image assignment.
     */
    template<class OtherDerived, bool otherMaterialized>
    derived_type& operator= (const GenericImageBase<color_type, OtherDerived, otherMaterialized>& givenThat) {
      const OtherDerived& that = static_cast<const OtherDerived&>(givenThat);
      /*if(this->getWidth() != that.getWidth() || this->getHeight() != that.getHeight())
        throw std::runtime_error("Could not assign images with different sizes.");*/
      
      if(otherMaterialized) {
        /* If we're both materialized then it's just pointer assignment. */
        this->data = that.materialize().data; /* We use materialize() just to make the compiler happy. It does nothing here. */
      } else {
        /* Else perform data copying. */
        this->assign(that);
      }
      
      return this->ref();
    }

    /**
     * We need to override default operator=.
     */
    derived_type& operator= (const GenericImage& that) {
      return this->operator=<derived_type, true>(that);
    }

    /**
     * Image copy construction.
     */
    template<class OtherDerived, bool otherMaterialized>
    GenericImage(const GenericImageBase<color_type, OtherDerived, otherMaterialized>& that) {
      *this = that;
    }

    /**
     * We need to override default copy constructor.
     */
    GenericImage(const GenericImage& that) {
      *this = that;
    }
  };


// -------------------------------------------------------------------------- //
// GenericImageBase
// -------------------------------------------------------------------------- //
  /**
   * Base class for all image expressions.
   *
   * @param Color                      Class representing single pixel in an image. 
   * @param Derived                    Derived class. Must define getWidth, getHeight, getPixel, getPixelInterpolated.
   * @param derivedMaterialized        Is derived class materialized?
   */
  template<class Color, class Derived, bool derivedMaterialized> class GenericImageBase {
  public:
    /** Type of a single pixel of an image. */
    typedef Color color_type;

    /** Determines whether Derived class provides materialized interface (i.e. direct data manipulation routines). */
    enum { materialized = derivedMaterialized };

  private:
    struct materialize_materialized {
      Derived& operator()(GenericImageBase& that) const {
        assert(materialized);
        return that.ref();
      }
    };

    struct materialize_unmaterialized {
      GenericImage<color_type> operator()(GenericImageBase& that) const {
        assert(!materialized);
        GenericImage<color_type> result = that.ref();
        return result;
      }
    };

    template<class OtherColor, class OtherDerived, bool otherMaterialized> friend class GenericImageBase;

  protected:
    /** Downcasts GenericImageBase to Derived class. */
    Derived& ref() { return *static_cast<Derived*>(this); }

    /** Downcasts GenericImageBase to Derived class. */
    const Derived& ref() const { return *static_cast<const Derived*>(this); }

    typedef typename arx::if_c<materialized, Derived, GenericImage<color_type> >::type materialized_type;

    /* For materialize() - it must return a reference to current image instance. */ 
    typedef typename arx::if_c<materialized, materialized_type&, materialized_type>::type materialized_ref_type;
    typedef typename arx::if_c<materialized, const materialized_type&, materialized_type>::type materialized_const_ref_type;

    /** Returns suitable size of a Gaussian kernel for the given sigma (standard deviation of the Gaussian distribution). */
    static int getGaussianKernelSize(float sigma) {
      return max(3, ((int) (sigma * GAUSS_TRUNCATE)) * 2 + 1);
    }

  public:
    /** 
     * Performs image subtraction.
     *
     * @param that                     Image to subtract.
     * @return                         Newly created image containing difference.
     */
    template<class OtherDerived, bool otherMaterialized>
    GenericImageDifference<Derived, OtherDerived> sub(const GenericImageBase<color_type, OtherDerived, otherMaterialized>& that) const {
      return GenericImageDifference<Derived, OtherDerived>(this->ref(), that.ref());
    }

    /** 
     * Resizes the image.
     * 
     * @param widthRatio               Factor by which the x dimension of the image is changed.
     * @param heightRatio              Factor by which the y dimension of the image is changed.
     * @return                         Newly created resized image.
     */
    GenericImageResize<Derived> resize(float widthRatio, float heightRatio) const {
      return GenericImageResize<Derived>(this->ref(), widthRatio, heightRatio);
    }

    /** 
     * Downscales an image by a factor of 2 using nearest neighbour interpolation.
     *
     * @return                         Newly created downscaled image.
     */
    template<int n>
    GenericImageResizeDownNx<Derived, n> resizeDownNN() const {
      return GenericImageResizeDownNx<Derived, n>(this->ref());
    }

    /** 
     * Filters an image using a Gaussian kernel.
     * 
     * @param sigma                    Standard deviation of the Gaussian distribution.
     * @return                         Newly created filtered image.
     **/
    materialized_type gaussianBlur(float sigma) const {
      materialized_type me = this->ref().materialize(); /* Because we reference each pixel several times. */
      
      typedef typename compose_color<float, channels<color_type>::value>::type pixel_super_type;

      int kernelSize = min(199, getGaussianKernelSize(sigma));

      float kernel[200], sum = 0.0f;
      for(int i = 0; i <= kernelSize; i++) {
        int x = i - kernelSize / 2;
        kernel[i] = exp(- x * x / (2.0f * sigma * sigma));
        sum += kernel[i];
      }
      for(int i = 0; i < kernelSize; i++)
        kernel[i] /= sum;

      materialized_type tmp(me.getWidth(), me.getHeight());
      tmp.fill(color_type());
      for(int y = 0; y < me.getHeight(); y++) {
        for(int x = 0; x < me.getWidth(); x++) {
          float sum = 0.0f;
          int x0 = max(0, x - kernelSize / 2);
          int x1 = min(me.getWidth(), x + kernelSize / 2 + 1);
          pixel_super_type pixel = pixel_super_type();
          for(int xx = x0; xx < x1; xx++) {
            pixel = pixel + pixel_super_type(me.getPixel(xx, y)) * kernel[kernelSize / 2 + xx - x];
            sum += kernel[kernelSize / 2 + xx - x];
          }
          tmp.setPixel(x, y, color_type(pixel / sum));
        }
      }

      materialized_type result(me.getWidth(), me.getHeight());
      result.fill(color_type());
      for(int y = 0; y < me.getHeight(); y++) {
        for(int x = 0; x < me.getWidth(); x++) {
          float sum = 0.0f;
          int y0 = max(0, y - kernelSize / 2);
          int y1 = min(me.getHeight(), y + kernelSize / 2 + 1);
          pixel_super_type pixel = pixel_super_type();
          for(int yy = y0; yy < y1; yy++) {
            pixel = pixel + pixel_super_type(tmp.getPixel(x, yy)) * kernel[kernelSize / 2 + yy - y];
            sum += kernel[kernelSize / 2 + yy - y];
          }
          result.setPixel(x, y, color_type(pixel / sum));
        }
      }

      return result;
    }

    /**
     * Materializes an image, i.e. allocates in image buffer in case it didn't existed to allow direct data manipulation.
     *
     * @return                         Materialized copy of this image in case it wasn't materialized, or image itself otherwise.
     */
    materialized_ref_type materialize() {
      return arx::if_c<materialized, materialize_materialized, materialize_unmaterialized>::type()(*this);
    }

    /**
     * Materializes an image, i.e. allocates in image buffer in case it didn't existed to allow direct data manipulation.
     *
     * @return                         Materialized copy of this image in case it wasn't materialized, or image itself otherwise.
     */
    materialized_const_ref_type materialize() const {
      return const_cast<GenericImageBase*>(this)->materialize();
    }

    /** 
     * Works like materialize, but always allocates a new image.
     *
     * @return                         Newly allocated copy of this image.
     */
    materialized_type clone() const {
      materialized_type result;
      result.assign(this->ref());
      return result;
    }

    /** 
     * Converts this image to another color type. Use image_cast if you need to convert to another image type.
     *
     * @param OtherColor               Color type to convert to.
     */
    template<class OtherColor>
    typename image_cast_impl_base<Derived, OtherColor, color_type>::return_type convert() const {
      STATIC_ASSERT((!is_image<OtherColor>::value)); /* We better prohibit it. */
      return image_cast_impl_base<Derived, OtherColor, color_type>()(this->ref());
    }

    /**
     * Saves an image to a file.
     */
    void saveToFile(const std::string& fileName) const {
      image_cast<Image3b>(this->ref()).saveToFile(fileName);
    }
  };


// -------------------------------------------------------------------------- //
// Helpers
// -------------------------------------------------------------------------- //
  /** Helper macro used in classes derived from GenericImage. */
#define IMAGE_DERIVE(NAME)                                                      \
  private:                                                                      \
    explicit NAME(const base_type& base): base_type(base) {}                    \
    friend class base_type;                                                     \
  public:                                                                       \
    NAME() {}                                                                   \
    NAME(int width, int height): base_type(width, height) {}                    \
    NAME(int width, int height, int wStep, void* pixels, ImageDeallocator* deallocator): \
      base_type(width, height, wStep, pixels, deallocator) {}                   \
    NAME(const NAME& that): base_type(that) {}                                  \
                                                                                \
    template<class OtherDerived, bool otherMaterialized>                        \
    NAME(const GenericImageBase<color_type, OtherDerived, otherMaterialized>& that): \
      base_type(that) {}                                                        \
                                                                                \
    template<class OtherDerived, bool otherMaterialized>                        \
    NAME& operator=(const GenericImageBase<color_type, OtherDerived, otherMaterialized>& that) { \
      return base_type::operator=(that);                                        \
    }                                                                           \
                                                                                \
    NAME& operator= (const NAME& that) {                                        \
      return base_type::operator=(that);                                        \
    }

// -------------------------------------------------------------------------- //
// Image1b
// -------------------------------------------------------------------------- //
  /** Single-channel byte image. */
  class Image1b: public GenericImage<unsigned char, Image1b> {
    IMAGE_DERIVE(Image1b)

  public:
  };


// -------------------------------------------------------------------------- //
// Image3b
// -------------------------------------------------------------------------- //
  /** Three-channel byte image. */
  class Image3b: public GenericImage<Color3b, Image3b> {
    IMAGE_DERIVE(Image3b)

  public:
    static Image3b loadFromFile(const std::string& fileName);
    void saveToFile(const std::string& fileName) const;
  };


// -------------------------------------------------------------------------- //
// Image4b
// -------------------------------------------------------------------------- //
  /** Four-channel byte image. */
  class Image4b: public GenericImage<Color4b, Image4b> {
    IMAGE_DERIVE(Image4b)

  public:
  };


// -------------------------------------------------------------------------- //
// Image1f
// -------------------------------------------------------------------------- //
  /** Single-channel float image. */
  class Image1f: public GenericImage<float, Image1f> {
    IMAGE_DERIVE(Image1f)
  
  public:
#ifdef USE_IPPI
    Image1f gaussianBlur(float sigma) const;
    Image1f sub(const Image1f& that) const;
    Image1f resize(float widthRatio, float heightRatio) const;
    void fill(const color_type& value);
#endif // USE_IPPI

    void gradientMagAndDir(Image1f& magnitude, Image1f& direction) const;
  };


// -------------------------------------------------------------------------- //
// Image3f
// -------------------------------------------------------------------------- //
  /** Three-channel float image. */
  class Image3f: public GenericImage<Color3f, Image3f> {
    IMAGE_DERIVE(Image3f)
  
  public:
#ifdef USE_IPPI
    using GenericImage::draw;
    void draw(const Image3f& that, int x, int y);
    void draw(const Image3f& that, const arx::Matrix3f& m);
#endif // USE_IPPI
  };


// -------------------------------------------------------------------------- //
// Image4f
// -------------------------------------------------------------------------- //
  /** Four-channel float image. */
  class Image4f: public GenericImage<Color4f, Image4f> {
    IMAGE_DERIVE(Image4f)

  public:
  };


  /* Clean up. */
#undef IMAGE_DERIVE


// -------------------------------------------------------------------------- //
// Converter specializations.
// -------------------------------------------------------------------------- //
#ifdef USE_IPPI
#  define SPECIALIZE_IMAGE_CAST_BEGIN(TO_IMAGE, FROM_IMAGE)                     \
  template<>                                                                    \
  struct image_cast_impl<TO_IMAGE, FROM_IMAGE> {                                \
    typedef TO_IMAGE return_type;                                               \
    return_type operator()(const FROM_IMAGE& that) {                            \
      TO_IMAGE result(that.getWidth(), that.getHeight());
#  define SPECIALIZE_IMAGE_CAST_END()                                           \
      return result;                                                            \
    }                                                                           \
  };

#  define SPECIALIZE_IMAGE_CAST_C1_TO_C3(TO_IMAGE, FROM_IMAGE, IPP_DATATYPE)    \
  SPECIALIZE_IMAGE_CAST_BEGIN(TO_IMAGE, FROM_IMAGE)                             \
    const ARX_JOIN_2(Ipp, IPP_DATATYPE)* pSrc[3];                               \
    std::fill_n(pSrc, 3, that.getPixelData());                                  \
    ARX_JOIN_3(ippiCopy_, IPP_DATATYPE, _P3C3R)(pSrc, that.getWStep(), result.getPixelData(), result.getWStep(), that.getIppiSize()); \
  SPECIALIZE_IMAGE_CAST_END()

  SPECIALIZE_IMAGE_CAST_C1_TO_C3(Image3f, Image1f, 32f)
  SPECIALIZE_IMAGE_CAST_C1_TO_C3(Image3b, Image1b, 8u)
#  undef SPECIALIZE_IMAGE_CAST_C1_TO_C3

#  define SPECIALIZE_IMAGE_CAST_C1_TO_C4(TO_IMAGE, FROM_IMAGE, IPP_DATATYPE)    \
  SPECIALIZE_IMAGE_CAST_BEGIN(TO_IMAGE, FROM_IMAGE)                             \
    const ARX_JOIN_2(Ipp, IPP_DATATYPE)* pSrc[4];                               \
    std::fill_n(pSrc, 4, that.getPixelData());                                  \
    ARX_JOIN_3(ippiCopy_, IPP_DATATYPE, _P4C4R)(pSrc, that.getWStep(), result.getPixelData(), result.getWStep(), that.getIppiSize()); \
    ARX_JOIN_3(ippiSet_, IPP_DATATYPE, _C4CR)(color_limits<typename TO_IMAGE::color_data_type>::max(), result.getPixelData() + 3, result.getWStep(), result.getIppiSize()); \
  SPECIALIZE_IMAGE_CAST_END()

  SPECIALIZE_IMAGE_CAST_C1_TO_C4(Image4f, Image1f, 32f)
  SPECIALIZE_IMAGE_CAST_C1_TO_C4(Image4b, Image1b, 8u)
#  undef SPECIALIZE_IMAGE_CAST_C1_TO_C4

#  define SPECIALIZE_IMAGE_CAST_C34_TO_C1(TO_IMAGE, FROM_IMAGE, IPP_DATATYPE, IPP_CHANNELTYPE) \
  SPECIALIZE_IMAGE_CAST_BEGIN(TO_IMAGE, FROM_IMAGE)                             \
    ARX_JOIN_5(ippiRGBToGray_, IPP_DATATYPE, _, IPP_CHANNELTYPE, C1R)(that.getPixelData(), that.getWStep(), result.getPixelData(), result.getWStep(), that.getIppiSize()); \
  SPECIALIZE_IMAGE_CAST_END()

  SPECIALIZE_IMAGE_CAST_C34_TO_C1(Image1f, Image4f, 32f, AC4)
  SPECIALIZE_IMAGE_CAST_C34_TO_C1(Image1f, Image3f, 32f, C3)
  SPECIALIZE_IMAGE_CAST_C34_TO_C1(Image1b, Image4b, 8u, AC4)
  SPECIALIZE_IMAGE_CAST_C34_TO_C1(Image1b, Image3b, 8u, C3)
#  undef SPECIALIZE_IMAGE_CAST_C1_TO_C4
#endif // USE_IPPI

#ifdef USE_CIPPIMAGE
  /** Cast specialization for conversion of Image1f to CIppImage*. 
   * Please note that you must delete the newly created CIppImage instance manually. */
  template<>
  struct image_cast_impl<CIppImage*, Image1f> {
    typedef CIppImage* return_type;
    return_type operator()(const Image1f& that) {
      CIppImage* result = new CIppImage();
      result->Alloc(that.getIppiSize(), 1, 8);
      ippiScale_32f8u_C1R(that.getPixelData(), that.getWStep(), result->getData(), result->Step(), that.getIppiSize(), 0.0f, 1.0f);
      return result;
    }
  };

  /** Cast specialization for conversion of Image3f to CIppImage*. 
   * Please note that you must delete the newly created CIppImage instance manually. */
  template<>
  struct image_cast_impl<CIppImage*, Image3f> {
    typedef CIppImage* return_type;
    return_type operator()(const Image3f& that) {
      CIppImage* result = new CIppImage();
      result->Alloc(that.getIppiSize(), 3, 8);
      ippiScale_32f8u_C3R(that.getPixelData(), that.getWStep(), result->getData(), result->Step(), that.getIppiSize(), 0.0f, 1.0f);
      return result;
    }
  };
#endif // USE_CIPPIMAGE

} // namespace prec 

#endif // __IMAGE_H__
