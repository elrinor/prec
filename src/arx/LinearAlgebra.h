#ifndef __ARX_LINEARALGEBRA_H__
#define __ARX_LINEARALGEBRA_H__

#include "config.h"
#include <cassert>
#include <iostream>
#include <algorithm>
#include <limits>
#include "smart_ptr.h"
#include "static_assert.h"
#include "Preprocessor.h"
#include "Mpl.h"
#include "Utility.h"

#ifdef ARX_INTELCC
#  pragma warning(disable: 69) // this seem to be the only way (
#endif

#ifdef min
#  undef min
#  define MIN_UNDEFFED
#endif
#ifdef max
#  undef max
#  define MAX_UNDEFFED
#endif

namespace arx {
  enum { DYNAMIC_SIZE = 0xFFFFFFFFU };

#define ARX_INHERIT_ASSIGNMENT_OPERATOR(FROMCLASS, OP)                          \
  template<class OtherT, class OtherDerived>                                    \
  this_type& operator OP (const MatrixBase<OtherT, OtherDerived>& that) {       \
    return static_cast<this_type&>(FROMCLASS::operator OP(that));               \
  }                                                                             \
  this_type& operator OP (const this_type& that) {                              \
    return static_cast<this_type&>(FROMCLASS::operator OP(that));               \
  }                                                                             \

#define ARX_INHERIT_SCALAR_ASSIGNMENT_OPERATOR(FROMCLASS, OP)                   \
  template<class OtherT>                                                        \
  this_type& operator OP(const OtherT& scalar) {                                \
    return static_cast<this_type&>(FROMCLASS::operator OP(scalar));             \
  }                                                                             \

#define ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM(FROMCLASS)                        \
  ARX_INHERIT_ASSIGNMENT_OPERATOR(FROMCLASS, +=)                                \
  ARX_INHERIT_ASSIGNMENT_OPERATOR(FROMCLASS, -=)                                \
  ARX_INHERIT_ASSIGNMENT_OPERATOR(FROMCLASS, =)                                 \
  ARX_INHERIT_SCALAR_ASSIGNMENT_OPERATOR(FROMCLASS, *=)                         \
  ARX_INHERIT_SCALAR_ASSIGNMENT_OPERATOR(FROMCLASS, /=)                         \

#define ARX_INHERIT_ASSIGNMENT_OPERATORS()                                      \
  ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM(MatrixBase)                             \
  
#define ARX_BEFRIEND_MATRIXBASE_WPREFIX(PREFIX)                                 \
  friend class PREFIX MatrixBase<value_type, this_type>;                        \

#define ARX_BEFRIEND_MATRIXBASE()                                               \
  ARX_BEFRIEND_MATRIXBASE_WPREFIX(ARX_EMPTY())                                  \

  /*
   * Derived:
   *   T& _elem(r, c);
   *   T _elem(r, c) const;
   *   std::size_t _rows() const;
   *   std::size_t _cols() const;
   */

  // nonassignable
  class nonassignable {
  private:
    nonassignable& operator= (nonassignable&);
  };

  // Forwards
  template<class T, class Derived> class MatrixBase;
  template<class T, std::size_t R, std::size_t C> class GenericMatrix;
  template<class M> class RowReference;
  template<class M> class ColReference;
  template<class L, class R, class O> class LinearOperation;
  template<class M, class O> class LinearUnaryOperation;
  template<class M, class O> class ScalarOperation;
  template<class L, class R> class MatrixMul;
  template<class M> class Cache;
  template<class M> class Transpose;
  template<class M> class Identity;
  template<class M> class Inverse;

  template<class T, std::size_t R, std::size_t C> class Matrix;
  template<class T> class DynamicMatrix;
  template<class T, std::size_t N> class Vector;
  template<class T> class DynamicVector;


  // Operations
  template<class T>
  struct Add {
    static T perform(const T& a, const T& b) {
      return a + b;
    }
  };

  template<class T>
  struct Sub {
    static T perform(const T& a, const T& b) {
      return a - b;
    }
  };

  template<class T>
  struct Mul {
    static T perform(const T& a, const T& b) {
      return a * b;
    }
  };

  template<class T>
  struct Div {
    static T perform(const T& a, const T& b) {
      return a / b;
    }
  };

  template<class T>
  struct Neg {
    static T perform(const T& a) {
      return -a;
    }
  };

  template<class T>
  struct Nop { // I know that's not a good name, but who cares anyway? )
    static T perform(const T& a) {
      return +a;
    }
  };


  // Traits
  template<class Linear> struct TraitsBase;
  template<class M> struct Traits;

  template<class T, class Derived> struct TraitsBase<MatrixBase<T, Derived> >: public TraitsBase<Derived> {};

  template<class T, std::size_t R, std::size_t C> struct TraitsBase<GenericMatrix<T, R, C> > {
    enum {static_rows = R, static_cols = C};
    typedef T value_type;
  };

  template<class M> struct TraitsBase<RowReference<M> > {
    enum {static_rows = 1, static_cols = TraitsBase<M>::static_cols};
    typedef typename TraitsBase<M>::value_type value_type;
  };

  template<class M> struct TraitsBase<ColReference<M> > {
    enum {static_rows = Traits<M>::static_rows, static_cols = 1};
    typedef typename TraitsBase<M>::value_type value_type;
  };

  template<class L, class R, class O> struct TraitsBase<LinearOperation<L, R, O> >: public TraitsBase<L> {
    // static + dynamic = static!
    enum {static_rows = Traits<L>::has_dynamic_rows ? Traits<R>::static_rows : Traits<L>::static_rows};
    enum {static_cols = Traits<L>::has_dynamic_cols ? Traits<R>::static_cols : Traits<L>::static_cols};
    typedef typename TraitsBase<L>::value_type value_type;
  }; // no asserts are needed here since everything is checked inside LinearOperation class

  template<class M, class O> struct TraitsBase<LinearUnaryOperation<M, O> >: public TraitsBase<M> {};

  template<class M, class O> struct TraitsBase<ScalarOperation<M, O> >: public TraitsBase<M> {};

  template<class L, class R> struct TraitsBase<MatrixMul<L, R> > {
    enum {static_rows = Traits<L>::static_rows};
    enum {static_cols = Traits<R>::static_cols};
    typedef typename TraitsBase<L>::value_type value_type;
  };

  template<class M> struct TraitsBase<Cache<M> >: public TraitsBase<M> {};

  template<class M> struct TraitsBase<Transpose<M> > {
    enum {static_rows = Traits<M>::static_cols};
    enum {static_cols = Traits<M>::static_rows};
    typedef typename TraitsBase<M>::value_type value_type;
  };

  template<class M> struct TraitsBase<Identity<M> >: public TraitsBase<M> {};

  template<class T, std::size_t R, std::size_t C> struct TraitsBase<Matrix<T, R, C> >: public TraitsBase<GenericMatrix<T, R, C> > {};

  template<class T> struct TraitsBase<DynamicMatrix<T> >: public TraitsBase<GenericMatrix<T, DYNAMIC_SIZE, DYNAMIC_SIZE> > {};

  template<class T, std::size_t N> struct TraitsBase<Vector<T, N> >: public TraitsBase<GenericMatrix<T, N, 1> > {};

  template<class T> struct TraitsBase<DynamicVector<T> >: public TraitsBase<GenericMatrix<T, DYNAMIC_SIZE, 1> > {};

  template<class M> struct Traits: public TraitsBase<M> {
    enum {has_dynamic_rows = (static_rows == DYNAMIC_SIZE), has_dynamic_cols = (static_cols == DYNAMIC_SIZE)};
    enum {has_static_rows = !has_dynamic_rows, has_static_cols = !has_dynamic_cols};
    enum {is_vector = (static_rows == 1 || static_cols == 1)};
    enum {is_matrix = !is_vector};
    enum {is_static = has_static_rows && has_static_cols};
    enum {is_dynamic = !is_static};
    enum {static_size = (has_static_rows && has_static_cols) ? (static_rows * static_cols) : DYNAMIC_SIZE};
  };


  // ConditionalCache
  // It is used when elements are accessed several times => it's better to precalculate everything
  template<class M> struct ConditionalCache {
    typedef Cache<M> simple_type;
    typedef simple_type type;
    enum {needs_caching = true};
  };

  template<class M> struct ConditionalCache<LinearUnaryOperation<M, Nop<typename Traits<M>::value_type> > > { // Huh, we'll get in trouble if unary operator+ for value_type really calculates something...
    typedef M simple_type;
    typedef const simple_type& type;
    enum {needs_caching = false};
  };

  template<class M> struct ConditionalCache<Cache<M> > {
    typedef Cache<M> simple_type;
    typedef const simple_type& type;
    enum {needs_caching = false};
  };

  template<class T, std::size_t R, std::size_t C> struct ConditionalCache<GenericMatrix<T, R, C> > {
    typedef GenericMatrix<T, R, C> simple_type;
    typedef const simple_type& type;
    enum {needs_caching = false};
  };

  template<class T, std::size_t R, std::size_t C> struct ConditionalCache<Matrix<T, R, C> > {
    typedef Matrix<T, R, C> simple_type;
    typedef const simple_type& type;
    enum {needs_caching = false};
  };

  template<class T> struct ConditionalCache<DynamicMatrix<T> > {
    typedef DynamicMatrix<T> simple_type;
    typedef const simple_type& type;
    enum {needs_caching = false};
  };

  template<class T, std::size_t N> struct ConditionalCache<Vector<T, N> > {
    typedef Vector<T, N> simple_type;
    typedef const simple_type& type;
    enum {needs_caching = false};
  };

  template<class T> struct ConditionalCache<DynamicVector<T> > {
    typedef DynamicVector<T> simple_type;
    typedef const simple_type& type;
    enum {needs_caching = false};
  };

  template<class M> struct ConditionalCache<Transpose<M> > {
    enum {needs_caching = ConditionalCache<M>::needs_caching};
    typedef typename if_c<needs_caching, Cache<Transpose<M> >, Transpose<M> >::type simple_type;
    typedef typename if_c<needs_caching, simple_type, const simple_type&>::type type;
  };

  template<class T, class Derived> struct ConditionalCache<MatrixBase<T, Derived> > {
    STATIC_ASSERT((!is_same<T, T>::value)); // no way!
  };


  // BrackAccessor
  template<class M, bool REQ1, bool CEQ1>
  class BrackAccessorBase {
  public:
    typedef RowReference<M> return_type;
    typedef const RowReference<M> const_return_type;
    
    static const_return_type access(const M& m, std::size_t index) {
      return const_return_type(m, index);
    }

    static return_type access(M& m, std::size_t index) {
      return return_type(m, index);
    }
  };

  template<class M>
  class BrackAccessorBase<M, true, false> {
  public:
    typedef typename Traits<M>::value_type& return_type;
    typedef typename Traits<M>::value_type const_return_type;

    static const_return_type access(const M& m, std::size_t index) {
      return m(0, index);
    }

    static return_type access(M& m, std::size_t index) {
      return m(0, index);
    }
  };

  template<class M>
  class BrackAccessorBase<M, false, true> {
  public:
    typedef typename Traits<M>::value_type& return_type;
    typedef typename Traits<M>::value_type const_return_type;

    static const_return_type access(const M& m, std::size_t index) {
      return m(index, 0);
    }

    static return_type access(M& m, std::size_t index) {
      return m(index, 0);
    }
  };

  template<class M>
  class BrackAccessorBase<M, true, true>: public BrackAccessorBase<M, false, true> {};

  template<class M>
  class BrackAccessor: public BrackAccessorBase<M, Traits<M>::static_rows == 1, Traits<M>::static_cols == 1> {};

  // Assigner
  template<class L, class R, bool IsVectors> 
  class AssignerBase {
  public:
    static L& assign(L& m0, const R& m1) {
      assert(m0.size() == m1.size());
      for(std::size_t i = 0; i < m0.size(); i++)
        m0[i] = m1[i];
      return m0;
    }
  };

  template<class L, class R> 
  class AssignerBase<L, R, false> {
  public:
    static L& assign(L& m0, const R& m1) {
      assert(m0.rows() == m1.rows() && m0.cols() == m1.cols());
      for(std::size_t r = 0; r < m0.cols(); r++)
        for(std::size_t c = 0; c < m0.cols(); c++)
          m0(r, c) = m1(r, c);
      return m0;
    }
  };

  template<class L, class R>
  class Assigner: public AssignerBase<L, R, Traits<L>::is_vector && Traits<R>::is_vector> {};

  // MatrixBase
  template<class T, class Derived>
  class MatrixBase: public Traits<Derived> {
  public:
    typedef T value_type;

    Derived& ref() {
      return *static_cast<Derived*>(this);
    }

    const Derived& ref() const {
      return *static_cast<const Derived*>(this);
    }

    value_type& operator()(std::size_t r, std::size_t c) {
      return this->ref()._elem(r, c);
    }

    value_type operator()(std::size_t r, std::size_t c) const {
      return this->ref()._elem(r, c);
    }

    typename BrackAccessor<Derived>::return_type operator[] (std::size_t index) {
      return BrackAccessor<Derived>::access(this->ref(), index);
    }

    typename BrackAccessor<Derived>::const_return_type operator[] (std::size_t index) const {
      return BrackAccessor<Derived>::access(this->ref(), index);
    }

    std::size_t rows() const {
      return this->ref()._rows();
    }

    std::size_t cols() const {
      return this->ref()._cols();
    }

    std::size_t size() const {
      return rows() * cols();
    }

    template<typename OtherDerived>
    value_type dot(const OtherDerived& that) const {
      STATIC_ASSERT((Traits<Derived>::is_vector && Traits<OtherDerived>::is_vector));
      assert(size() == that.size());
      value_type s = value_type();
      for(std::size_t i = 0; i < size(); i++)
        s += (*this)[i] * that[i];
      return s;
    }

    value_type normSqr() const {
      return this->dot(*this);
    }

    value_type norm() const {
      return sqrt(normSqr());
    }

    void normalize() {
      *this /= norm();
    }

    void fill(const value_type& value) {
      for(std::size_t r = 0; r < rows(); r++)
        for(std::size_t c = 0; c < cols(); c++)
          (*this)(r, c) = value;
    }

    ColReference<Derived> col(int c) {
      return ColReference<Derived>(this->ref(), c);
    }

    const ColReference<Derived> col(int c) const {
      return ConstColReference<Derived>(this->ref(), c);
    }

    RowReference<Derived> row(int r) {
      return RowReference<Derived>(this->ref(), r);
    }

    const RowReference<Derived> row(int c) const {
      return ConstRowReference<Derived>(this->ref(), r);
    }

    Transpose<Derived> transpose() {
      return Transpose<Derived>(this->ref());
    }

    const Transpose<Derived> transpose() const {
      return Transpose<Derived>(this->ref());
    }

    static const Identity<Derived> identity(std::size_t size) {
      return Identity<Derived>(size);
    }

    static const Identity<Derived> identity() {
      STATIC_ASSERT((Traits<Derived>::is_static));
      return Identity<Derived>(0); // size is ignored in this case
    }

    template<typename OtherDerived>
    Derived& operator= (const MatrixBase<value_type, OtherDerived>& that) {
      return Assigner<Derived, OtherDerived>::assign(this->ref(), that.ref());
    }

    Derived& operator= (const MatrixBase& that) {
      return this->operator=<Derived>(that);
    }

    template<typename OtherDerived>
    Derived& operator+= (const MatrixBase<value_type, OtherDerived>& that) {
      return *this = *this + that;
    }

    template<typename OtherDerived>
    Derived& operator-= (const MatrixBase<value_type, OtherDerived>& that) {
      return *this = *this - that;
    }

    template<typename OtherDerived>
    Derived& operator*= (const MatrixBase<value_type, OtherDerived>& that) {
      return *this = *this * that;
    }

    Derived& operator*= (const value_type& that) {
      return *this = *this * that;
    }
    
    Derived& operator/= (const value_type& that) {
      return *this = *this / that;
    }
  };

  template<class T, class Derived, class CharT, class CharTraits>
  std::basic_ostream<CharT, CharTraits>& operator<< (std::basic_ostream<CharT, CharTraits>& s, const MatrixBase<T, Derived>& m) {
    // this is a rare and costy operation so we don't use static ifs here
    if(Traits<Derived>::is_vector) {
      s << m[0];
      for(std::size_t i = 1; i < m.size(); i++)
        s << " " << m[i];
    } else {
      for(std::size_t r = 0; r < m.rows(); r++) {
        s << m(r, 0);
        for(std::size_t c = 1; c < m.cols(); c++)
          s << " " << m(r, c);
        if(r < m.rows() - 1)
          s << std::endl;
      }
    }
    return s;
  }

  // MatrixStorageBase
  template<class T, std::size_t R, std::size_t C> 
  class MatrixStorageBase {
  public:
    T data[R * C];

    MatrixStorageBase(std::size_t rows, std::size_t cols) {}

    std::size_t _rows() const { 
      return R; 
    }

    std::size_t _cols() const { 
      return C; 
    }
  };

  template<class T, std::size_t R>
  class MatrixStorageBase<T, R, DYNAMIC_SIZE> {
  protected:
    std::size_t __cols;
    T* data;

    MatrixStorageBase(std::size_t rows, std::size_t cols): __cols(cols), data(new T[cols * R]) {}

    ~MatrixStorageBase() { 
      delete[] this->data; 
    }

    std::size_t _rows() const { 
      return R; 
    }

    std::size_t _cols() const { 
      return this->__cols; 
    }
  };

  template<class T, std::size_t C>
  class MatrixStorageBase<T, DYNAMIC_SIZE, C> {
  protected:
    std::size_t __rows;
    T* data;

    MatrixStorageBase(std::size_t rows, std::size_t cols): __rows(rows), data(new T[C * rows]) {}

    ~MatrixStorageBase() {
      delete[] this->data;
    }

    std::size_t _rows() const { 
      return this->__rows; 
    }

    std::size_t _cols() const { 
      return C; 
    }
  };

  template<class T>
  class MatrixStorageBase<T, DYNAMIC_SIZE, DYNAMIC_SIZE> {
  protected:
    std::size_t __rows, __cols;
    T* data;

    MatrixStorageBase(std::size_t rows, std::size_t cols): __rows(rows), __cols(cols), data(new T[rows * cols]) {}

    ~MatrixStorageBase() { 
      delete[] this->data;
    }

    std::size_t _rows() const { 
      return this->__rows; 
    }

    std::size_t _cols() const { 
      return this->__cols; 
    }
  };

  // MatrixStorage
  template<class T, std::size_t R, std::size_t C>
  class MatrixStorage: public MatrixStorageBase<T, R, C> {
  protected:
    MatrixStorage(std::size_t rows, std::size_t cols): MatrixStorageBase(rows, cols) {}

    T& _elem(std::size_t r, std::size_t c) { 
      assert(r < this->_rows() && c < this->_cols());
      return this->data[r * this->_cols() + c]; 
    }

    const T& _elem(std::size_t r, std::size_t c) const {
      assert(r < this->_rows() && c < this->_cols());
      return this->data[r * this->_cols() + c]; 
    }

    T* _data() {
      return static_cast<T*>(this->data);
    }
    
    const T* _data() const {
      return static_cast<T*>(this->data);
    }
  };

  // GenericMatrix
  template<class T, std::size_t R, std::size_t C>
  class GenericMatrix: public MatrixBase<T, GenericMatrix<T, R, C> >, private MatrixStorage<T, R, C> {
  private:
    typedef GenericMatrix this_type;

    ARX_BEFRIEND_MATRIXBASE();

  public:
    GenericMatrix(std::size_t r, std::size_t c): MatrixStorage(r, c) {}

    GenericMatrix(std::size_t r, std::size_t c, value_type value): MatrixStorage(r, c) {
      this->fill(value);
    }

    ARX_INHERIT_ASSIGNMENT_OPERATORS();

    template<class OtherT, class OtherDerived>
    GenericMatrix(const MatrixBase<OtherT, OtherDerived>& that): MatrixStorage(that.rows(), that.cols()) {
      *this = that;
    }

    GenericMatrix(const GenericMatrix& that): MatrixStorage(that.rows(), that.cols()) {
      *this = that;
    }

    T* data() {
      return this->_data();
    }

    const T* data() const {
      return this->_data();
    }
  };


  // RowReference
  template<class M>
  class RowReference: public MatrixBase<typename Traits<M>::value_type, RowReference<M> > {
  private:
    const M& m;
    std::size_t row;

    typedef RowReference this_type;

  public:
    RowReference(const M& m, std::size_t row): m(m), row(row) {
      assert(row < m.rows());
    }

    // default copy constructor is OK

    std::size_t _rows() const {
      return 1; 
    }

    std::size_t _cols() const {
      return this->m.cols(); 
    }

    value_type& _elem(std::size_t r, std::size_t c) {
      assert(r == 0);
      return const_cast<M&>(this->m)(this->row, c);
    }

    value_type _elem(std::size_t r, std::size_t c) const { 
      assert(r == 0);
      return this->m(this->row, c);
    }

    ARX_INHERIT_ASSIGNMENT_OPERATORS();
  };


  // ColReference
  template<class M>
  class ColReference: public MatrixBase<typename Traits<M>::value_type, ColReference<M> > {
  private:
    const M& m;
    std::size_t col;

    typedef ColReference this_type;

  public:
    ColReference(const M& m, std::size_t col): m(m), col(col) {
      assert(col < m.cols());
    }

    // default copy constructor is OK

    std::size_t _rows() const {
      return this->m.rows(); 
    }

    std::size_t _cols() const {
      return 1;
    }

    value_type& _elem(std::size_t r, std::size_t c) {
      assert(c == 0);
      return const_cast<M&>(this->m)(r, this->col);
    }

    value_type _elem(std::size_t r, std::size_t c) const { 
      assert(c == 0);
      return this->m(r, this->col);
    }

    ARX_INHERIT_ASSIGNMENT_OPERATORS();
  };


  // LinearOperation
  template<class L, class R, class O>
  class LinearOperation: public MatrixBase<typename Traits<L>::value_type, LinearOperation<L, R, O> >, private nonassignable {
  private:
    const L& l;
    const R& r;

    typedef LinearOperation this_type;

    STATIC_ASSERT((is_same<typename Traits<L>::value_type, typename Traits<R>::value_type>::value));
    STATIC_ASSERT((Traits<L>::static_cols == Traits<R>::static_cols || Traits<L>::has_dynamic_cols || Traits<R>::has_dynamic_cols));
    STATIC_ASSERT((Traits<L>::static_rows == Traits<R>::static_rows || Traits<L>::has_dynamic_rows || Traits<R>::has_dynamic_rows));

  public:
    LinearOperation(const L& l, const R& r): l(l), r(r) {
      assert(l.cols() == r.cols() && l.rows() == r.rows());
    }

    // default copy constructor is OK
    
    std::size_t _rows() const {
      return this->l.rows();
    }

    std::size_t _cols() const {
      return this->l.cols();
    }

    value_type _elem(std::size_t r, std::size_t c) const { 
      return O::perform(this->l(r, c), this->r(r, c));
    }
  };

  template<class T, class L, class R>
  const LinearOperation<L, R, Add<T> > operator+ (const MatrixBase<T, L>& l, const MatrixBase<T, R>& r) {
    return LinearOperation<L, R, Add<T> >(l.ref(), r.ref());
  }

  template<class T, class L, class R>
  const LinearOperation<L, R, Sub<T> > operator- (const MatrixBase<T, L>& l, const MatrixBase<T, R>& r) {
    return LinearOperation<L, R, Sub<T> >(l.ref(), r.ref());
  }


  // LinearUnaryOperation
  template<class M, class O>
  class LinearUnaryOperation: public MatrixBase<typename Traits<M>::value_type, LinearUnaryOperation<M, O> >, private nonassignable {
  private:
    const M& m;

    typedef LinearUnaryOperation this_type;

  public:
    LinearUnaryOperation(const M& m): m(m) {}

    // default copy constructor is OK

    std::size_t _rows() const {
      return this->m.rows();
    }

    std::size_t _cols() const {
      return this->m.cols();
    }

    value_type _elem(std::size_t r, std::size_t c) const { 
      return O::perform(this->m(r, c));
    }
  };

  template<class T, class M>
  const LinearUnaryOperation<M, Neg<T> > operator- (const MatrixBase<T, M>& m) {
    return LinearUnaryOperation<M, Neg<T> >(m.ref());
  }

  template<class T, class M>
  const LinearUnaryOperation<M, Nop<T> > operator+ (const MatrixBase<T, M>& m) {
    return LinearUnaryOperation<M, Nop<T> >(m.ref());
  }


  // MatrixMul
  template<class L, class R>
  class MatrixMul: public MatrixBase<typename Traits<L>::value_type, MatrixMul<L, R> >, private nonassignable {
  private:
    typename ConditionalCache<L>::type l;
    typename ConditionalCache<R>::type r;

    typedef MatrixMul this_type;

    STATIC_ASSERT((is_same<typename Traits<L>::value_type, typename Traits<R>::value_type>::value));
    STATIC_ASSERT((Traits<L>::static_cols == Traits<R>::static_rows || Traits<L>::has_dynamic_cols || Traits<R>::has_dynamic_rows));

  public:
    MatrixMul(const L& l, const R& r): l(l), r(r) {
      assert(l.cols() == r.rows());
    }

    // default copy constructor is OK

    std::size_t _rows() const {
      return this->l.rows();
    }

    std::size_t _cols() const {
      return this->r.cols();
    }

    value_type _elem(std::size_t r, std::size_t c) const { 
      value_type result = value_type();
      for(std::size_t i = 0; i < l.cols(); i++)
        result += this->l(r, i) * this->r(i, c);
      return result;
    }
  };

  template<class T, class L, class R>
  const MatrixMul<L, R> operator* (const MatrixBase<T, L>& l, const MatrixBase<T, R>& r) {
    return MatrixMul<L, R>(l.ref(), r.ref());
  }


  // ScalarOperation
  template<class M, class O>
  class ScalarOperation: public MatrixBase<typename Traits<M>::value_type, ScalarOperation<M, O> >, private nonassignable {
  private:
    const M& m;
    value_type s;

    typedef ScalarOperation this_type;

  public:
    ScalarOperation(const M& m, const value_type& s): m(m), s(s) {}

    // default copy constructor is OK

    std::size_t _rows() const {
      return this->m.rows();
    }

    std::size_t _cols() const {
      return this->m.cols();
    }

    value_type _elem(std::size_t r, std::size_t c) const { 
      return O::perform(this->m(r, c), this->s);
    }
  };

  template<class T, class M>
  const ScalarOperation<M, Mul<T> > operator* (const MatrixBase<T, M>& l, const T& r) {
    return ScalarOperation<M, Mul<T> >(l.ref(), r);
  }

  template<class T, class M>
  const ScalarOperation<M, Mul<T> > operator* (const T& l, const MatrixBase<T, M>& r) {
    return ScalarOperation<M, Mul<T> >(r.ref(), l); // this enforces the operator* of type T to be commutative
  }

  template<class T, class M>
  const ScalarOperation<M, Div<T> > operator/ (const MatrixBase<T, M>& l, const T& r) {
    return ScalarOperation<M, Div<T> >(l.ref(), r);
  }

  // Cache
  template<class M>
  class Cache: public GenericMatrix<typename Traits<M>::value_type, Traits<M>::static_rows, Traits<M>::static_cols>, private nonassignable {
  private:
    typedef Cache this_type;

  public:
    Cache(const M& m): GenericMatrix(m) {}

    // default copy constructor is OK
  };

  // Transpose
  template<class M>
  class Transpose: public MatrixBase<typename Traits<M>::value_type, Transpose<M> > {
  private:
    const M& m;

    typedef Transpose this_type;

  public:
    Transpose(const M& m): m(m) {}

    // default copy constructor is OK

    std::size_t _rows() const {
      return this->m.cols();
    }

    std::size_t _cols() const {
      return this->m.rows();
    }

    value_type _elem(std::size_t r, std::size_t c) const { 
      return this->m(c, r);
    }

    value_type& _elem(std::size_t r, std::size_t c) {
      return const_cast<M&>(this->m)(c, r);
    }

    ARX_INHERIT_ASSIGNMENT_OPERATORS();
  };

  // Identity
  template<class M> 
  class Identity: public MatrixBase<typename Traits<M>::value_type, Identity<M> >, private nonassignable {
  private:
    template<std::size_t StaticSize>
    struct size_storage {
      size_storage(std::size_t) {};

      std::size_t operator()() const {
        return StaticSize;
      }
    };

    template<>
    struct size_storage<DYNAMIC_SIZE> {
    private:
      std::size_t size;

    public:
      size_storage(std::size_t size): size(size) {};

      std::size_t operator()() const {
        return this->size;
      }
    };

    STATIC_ASSERT((Traits<M>::static_cols == Traits<M>::static_rows));
    size_storage<Traits<M>::static_cols> __size;

  public:
    Identity(std::size_t size): __size(size) {}

    std::size_t _rows() const {
      return this->__size();
    }

    std::size_t _cols() const {
      return this->__size();
    }

    value_type _elem(std::size_t r, std::size_t c) const {
      assert(r < this->_rows() && c < this->_cols());
      return (r == c) ? static_cast<value_type>(1) : static_cast<value_type>(0);
    }
  };


  // Static-sized Matrix
  template<class T, std::size_t R, std::size_t C>
  class Matrix: public GenericMatrix<T, R, C> {
  private:
    STATIC_ASSERT((R != DYNAMIC_SIZE && C != DYNAMIC_SIZE));

    typedef Matrix this_type;

  public:
    Matrix(): GenericMatrix(0, 0) {}

    explicit Matrix(value_type value): GenericMatrix(0, 0, value) {}

    template<class OtherT, class OtherDerived>
    Matrix(const MatrixBase<OtherT, OtherDerived>& that): GenericMatrix(that) {}

    ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM(GenericMatrix);
  };


#define ARX_DEFINE_MATRIX_SPECIALIZATION(SIZE_R, SIZE_C) \
  template<class T>                                                             \
  class Matrix<T, SIZE_R, SIZE_C>: public GenericMatrix<T, SIZE_R, SIZE_C> {    \
  private:                                                                      \
    typedef Matrix this_type;                                                   \
                                                                                \
  public:                                                                       \
    Matrix(): GenericMatrix(0, 0) {}                                            \
                                                                                \
    explicit Matrix(value_type value): GenericMatrix(0, 0, value) {}            \
                                                                                \
    template<class OtherT, class OtherDerived>                                  \
    Matrix(const MatrixBase<OtherT, OtherDerived>& that): GenericMatrix(that) {} \
                                                                                \
    ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM(GenericMatrix);
#define ARX_END_MATRIX_SPECIALIZATION()                                         \
  };

  ARX_DEFINE_MATRIX_SPECIALIZATION(3, 3)
    Matrix inverse() const {
      Matrix inv;
      const Matrix& m(*this);

      const T det_minor00 = m(1, 1) * m(2, 2) - m(1, 2) * m(2, 1);
      const T det_minor10 = m(0, 1) * m(2, 2) - m(0, 2) * m(2, 1);
      const T det_minor20 = m(0, 1) * m(1, 2) - m(0, 2) * m(1, 1);
      const T det_minor01 = m(1, 0) * m(2, 2) - m(1, 2) * m(2, 0);
      const T det_minor11 = m(0, 0) * m(2, 2) - m(0, 2) * m(2, 0);
      const T det_minor21 = m(0, 0) * m(1, 2) - m(0, 2) * m(1, 0);
      const T det_minor02 = m(1, 0) * m(2, 1) - m(1, 1) * m(2, 0);
      const T det_minor12 = m(0, 0) * m(2, 1) - m(0, 1) * m(2, 0);
      const T det_minor22 = m(0, 0) * m(1, 1) - m(0, 1) * m(1, 0);

      const T det = det_minor00 * m(0, 0) - det_minor10 * m(1, 0) + det_minor20 * m(2, 0);
      const T invdet = static_cast<T>(1) / det;
      inv(0, 0) = det_minor00 * invdet;
      inv(0, 1) = -det_minor10 * invdet;
      inv(0, 2) = det_minor20 * invdet;
      inv(1, 0) = -det_minor01 * invdet;
      inv(1, 1) = det_minor11 * invdet;
      inv(1, 2) = -det_minor21 * invdet;
      inv(2, 0) = det_minor02 * invdet;
      inv(2, 1) = -det_minor12 * invdet;
      inv(2, 2) = det_minor22 * invdet;

      return inv;
    }
    
    static Matrix translation(const T x, const T y) {
      Matrix result = Matrix::identity();
      result[0][2] = x;
      result[1][2] = y;
      return result;
    }

    static Matrix scale(float scaleX, float scaleY) {
      Matrix result = Matrix::identity();
      result[0][0] = scaleX;
      result[1][1] = scaleY;
      return result;
    }

    static Matrix scale(float scaleFactor) {
      return scale(scaleFactor, scaleFactor);
    }
  ARX_END_MATRIX_SPECIALIZATION()


  // Dynamically-sized Matrix
  template<class T>
  class DynamicMatrix: public GenericMatrix<T, DYNAMIC_SIZE, DYNAMIC_SIZE> {
  private:
    typedef DynamicMatrix this_type;

  public:
    DynamicMatrix(std::size_t r, std::size_t c): GenericMatrix(r, c) {}

    DynamicMatrix(std::size_t r, std::size_t c, value_type value): GenericMatrix(r, c, value) {}

    template<class OtherT, class OtherDerived>
    DynamicMatrix(const MatrixBase<OtherT, OtherDerived>& that): GenericMatrix(that) {}

    ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM(GenericMatrix);
  };


  // Static-sized Vector
  template<class T, std::size_t N>
  class Vector: public GenericMatrix<T, N, 1> {
  private:
    STATIC_ASSERT((N != DYNAMIC_SIZE));

    typedef Vector this_type;

  public:
    Vector(): GenericMatrix(0, 0) {}

    explicit Vector(value_type value): GenericMatrix(0, 0, value) {}

    template<class OtherT, class OtherDerived>
    Vector(const MatrixBase<OtherT, OtherDerived>& that): GenericMatrix(that) {}

    ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM(GenericMatrix);
  };

#define ARX_DEFINE_VECTOR_ACCESSOR(NAME, INDEX)                                 \
  value_type NAME() const {                                                     \
    return (*this)[INDEX];                                                      \
  }                                                                             \
  value_type& NAME() {                                                          \
    return (*this)[INDEX];                                                      \
  }                                                                             \

#define ARX_DEFINE_VECTOR_2_ACCESSORS()                                         \
  ARX_DEFINE_VECTOR_ACCESSOR(x, 0)                                              \
  ARX_DEFINE_VECTOR_ACCESSOR(y, 1)                                              \

#define ARX_DEFINE_VECTOR_3_ACCESSORS()                                         \
  ARX_DEFINE_VECTOR_2_ACCESSORS()                                               \
  ARX_DEFINE_VECTOR_ACCESSOR(z, 2)                                              \

#define ARX_DEFINE_VECTOR_4_ACCESSORS()                                         \
  ARX_DEFINE_VECTOR_3_ACCESSORS()                                               \
  ARX_DEFINE_VECTOR_ACCESSOR(w, 3)                                              \

#define ARX_DEFINE_VECTOR_SPECIALIZATION(SIZE, DCONSTRUCTORPARAMS, DCONSTRUCTOR) \
  template<class T>                                                             \
  class Vector<T, SIZE>: public GenericMatrix<T, SIZE, 1> {                     \
  private:                                                                      \
    typedef Vector this_type;                                                   \
                                                                                \
  public:                                                                       \
    Vector(): GenericMatrix(0, 0) {}                                            \
                                                                                \
    explicit Vector(value_type value): GenericMatrix(0, 0, value) {}            \
                                                                                \
    template<class OtherT, class OtherDerived>                                  \
    Vector(const MatrixBase<OtherT, OtherDerived>& that): GenericMatrix(that) {} \
                                                                                \
    Vector(value_type x, value_type y DCONSTRUCTORPARAMS): GenericMatrix(0, 0) { \
      (*this)[0] = x;                                                           \
      (*this)[1] = y;                                                           \
      DCONSTRUCTOR;                                                             \
    }                                                                           \
                                                                                \
    ARX_DEFINE_VECTOR_ ## SIZE ## _ACCESSORS();                                 \
    ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM(GenericMatrix);
#define ARX_END_VECTOR_SPECIALIZATION()                                         \
  };


  ARX_DEFINE_VECTOR_SPECIALIZATION(2, ARX_EMPTY(), ARX_EMPTY())
    float angle() const {
      return atan2(y(), x());
    }
  ARX_END_VECTOR_SPECIALIZATION()

  ARX_DEFINE_VECTOR_SPECIALIZATION(3, ARX_COMMA() value_type z, (*this)[2] = z)
    explicit Vector(Vector<value_type, 2> that): GenericMatrix(0, 0) {
      (*this)[0] = that[0];
      (*this)[1] = that[1];
      (*this)[2] = static_cast<value_type>(1);
    }
  ARX_END_VECTOR_SPECIALIZATION()

  ARX_DEFINE_VECTOR_SPECIALIZATION(4, ARX_COMMA() value_type z ARX_COMMA() value_type w, (*this)[2] = z; (*this)[3] = w)
    explicit Vector(Vector<value_type, 3> that): GenericMatrix(0, 0) {
      (*this)[0] = that[0];
      (*this)[1] = that[1];
      (*this)[2] = that[3];
      (*this)[3] = static_cast<value_type>(1);
    }
  ARX_END_VECTOR_SPECIALIZATION()

  // clean up
#undef ARX_DEFINE_VECTOR_SPECIALIZATION
#undef ARX_END_VECTOR_SPECIALIZATION
#undef ARX_DEFINE_VECTOR_4_ACCESSORS
#undef ARX_DEFINE_VECTOR_3_ACCESSORS
#undef ARX_DEFINE_VECTOR_2_ACCESSORS
#undef ARX_DEFINE_VECTOR_ACCESSOR
#undef ARX_DEFINE_MATRIX_SPECIALIZATION
#undef ARX_END_MATRIX_SPECIALIZATION


  // Dynamically-sized Vector
  template<class T>
  class DynamicVector: public GenericMatrix<T, DYNAMIC_SIZE, 1> {
  private:
    typedef DynamicVector this_type;

  public:
    explicit DynamicVector(std::size_t n): GenericMatrix(n, 0) {}

    DynamicVector(std::size_t n, value_type value): GenericMatrix(n, 0, value) {}

    template<class OtherT, class OtherDerived>
    DynamicVector(const MatrixBase<OtherT, OtherDerived>& that): GenericMatrix(that) {}

    ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM(GenericMatrix);
  };


  // LinearSolver
  template<class T, class A, class B, bool UseDirectPermutations>
  struct LinearSystemSolver {
    static void solve(MatrixBase<T, A>& a, MatrixBase<T, B>& b) {
      /* This solver will be called only for statically-sized matrices, 
       * so it is a good idea to check everything at compile time. */
      STATIC_ASSERT((Traits<A>::is_matrix && Traits<B>::is_vector));
      STATIC_ASSERT((Traits<A>::static_rows == Traits<B>::static_size));

      // Triangulize the matrix.
      for(std::size_t col = 0; col < b.size() - 1; col++) {
        // Move row with largest coefficient to top.
        T maxc = std::numeric_limits<T>::min();
        std::size_t pivot = 0;
        for(std::size_t row = col; row < b.size(); row++) {
          T coef = abs(a[row][col]);
          if(coef > maxc) {
            maxc = coef;
            pivot = row;
          }
        }

        // Exchange "pivot" with "col" row 
        if(pivot != col) {
          for(std::size_t i = 0; i < a.cols(); i++)
            std::swap(a(pivot, i), a(col, i));
          std::swap(b[pivot], b[col]);
        }

        // Do reduction for this column.
        for(std::size_t row = col + 1; row < b.size(); row++) {
          T mult = a[row][col] / a[col][col];
          for(std::size_t c = col; c < b.size(); c++)  // Could start with c=col+1?
            a[row][c] -= mult * a[col][c];
          b[row] -= mult * b[col];
        }
      }

      // Do back substitution.  Pivoting does not affect solution order.
      for(int row = (int) b.size() - 1; row >= 0; row--) {
        T val = b[row];
        for(int col = (int) b.size() - 1; col > row; col--)
          val -= b[col] * a[row][col];
        b[row] = val / a[row][row];
      }
    }
  };

  template<class T, class A, class B>
  struct LinearSystemSolver<T, A, B, false> {
    static void solve(MatrixBase<T, A>& a, MatrixBase<T, B>& b) {
      STATIC_ASSERT((Traits<A>::is_matrix && Traits<B>::is_vector));
      assert(a.rows() == b.size());

      // construct permutation vector
      GenericMatrix<std::size_t, Traits<A>::static_rows, 1> p(a.rows(), 1);
      for(std::size_t i = 0; i < a.rows(); i++)
        p[i] = i;

      // Triangulize the matrix.
      for(std::size_t col = 0; col < b.size() - 1; col++) {
        // Move row with largest coefficient to top.
        T maxc = std::numeric_limits<T>::min();
        std::size_t pivot = 0;
        for(std::size_t row = col; row < b.size(); row++) {
          T coef = abs(a[p[row]][col]);
          if(coef > maxc) {
            maxc = coef;
            pivot = row;
          }
        }

        // Exchange "pivot" with "col" row 
        if(pivot != col) {
          std::swap(p[pivot], p[col]);
          std::swap(b[pivot], b[col]);
        }

        // Do reduction for this column.
        for(std::size_t row = col + 1; row < b.size(); row++) {
          T mult = a[p[row]][col] / a[p[col]][col];
          for(std::size_t c = col; c < b.size(); c++)  // Could start with c=col+1?
            a[p[row]][c] -= mult * a[p[col]][c];
          b[row] -= mult * b[col];
        }
      }

      // Do back substitution
      for(int row = (int) b.size() - 1; row >= 0; row--) {
        T val = b[row];
        for(int col = (int) b.size() - 1; col > row; col--)
          val -= b[col] * a[p[row]][col];
        b[row] = val / a[p[row]][row];
      }
    }
  };

  template<class T, class A, class B> 
  void solveLinearSystem(MatrixBase<T, A>& a, MatrixBase<T, B>& b) {
    LinearSystemSolver<T, A, B, Traits<A>::is_static && 
      Traits<B>::is_static && Traits<A>::static_cols < ARX_LINEAR_SOLVER_PERMUTATION_VERTOR_USAGE_THRESH>::solve(a, b);
  }

  // Useful Typedefs
  typedef Vector<float, 4>     Vector4f;
  typedef Vector<float, 3>     Vector3f;
  typedef Vector<float, 2>     Vector2f;

  typedef Matrix<float, 4, 4>  Matrix4f;
  typedef Matrix<float, 3, 3>  Matrix3f;
  typedef Matrix<float, 2, 2>  Matrix2f;

  typedef DynamicVector<float> VectorXf;
  typedef DynamicMatrix<float> MatrixXf;

} // namespace arx

// clean up
#ifdef MIN_UNDEFFED
#  define min(x, y) ((x > y)?(y):(x))
#  undef MIN_UNDEFFED
#endif
#ifdef MAX_UNDEFFED
#  define max(x, y) ((x > y)?(x):(y))
#  undef MAX_UNDEFFED
#endif

#undef ARX_INHERIT_ASSIGNMENT_OPERATOR
#undef ARX_INHERIT_SCALAR_ASSIGNMENT_OPERATOR
#undef ARX_INHERIT_ASSIGNMENT_OPERATORS_FROM
#undef ARX_INHERIT_ASSIGNMENT_OPERATORS
#undef ARX_BEFRIEND_MATRIXBASE_WPREFIX
#undef ARX_BEFRIEND_MATRIXBASE

#endif
