#ifndef __ARX_SMART_PTR__
#define __ARX_SMART_PTR__

#include "config.h"

#ifdef ARX_USE_BOOST
#  include <boost/shared_ptr.hpp>
#  include <boost/shared_array.hpp>
#  include <boost/scoped_ptr.hpp>
#  include <boost/scoped_array.hpp>
namespace arx {
  using boost::shared_ptr;
  using boost::shared_array;
  using boost::scoped_ptr;
  using boost::scoped_array;
}
#else

#ifdef ARX_LINUX
#  define ARX_INTERLOCKED_INCREMENT(x) (++(*x))
#  define ARX_INTERLOCKED_DECREMENT(x) (--(*x))
#  pragma warning "Linux non-boost implementation of shared_ptr template is currently not thread-safe. Please define ARX_USE_BOOST in config.h to use the implementation from boost."
#endif

#ifdef ARX_WIN32
#  define NOMINMAX
#  include <Windows.h>
#  define ARX_INTERLOCKED_INCREMENT(x) InterlockedIncrement(reinterpret_cast<long volatile*> (x))
#  define ARX_INTERLOCKED_DECREMENT(x) InterlockedDecrement(reinterpret_cast<long volatile*> (x))
#endif

#include <cassert>
#include <exception>
#include <algorithm>

namespace arx {

  namespace detail {

// -------------------------------------------------------------------------- //
// counted_base
// -------------------------------------------------------------------------- //
    class counted_base {
    private:
      counted_base(counted_base const&);
      counted_base& operator= (counted_base const&);

      long my_use_count;
    public:
      counted_base(): my_use_count(1) {}

      virtual ~counted_base() {}

      virtual void dispose() = 0;

      virtual void destroy() {
        delete this;
      }

      void add_ref_copy() {
        ARX_INTERLOCKED_INCREMENT(&my_use_count);
      }

      void release() {
        if(ARX_INTERLOCKED_DECREMENT(&my_use_count) == 0) {
          dispose();
          destroy();
        }
      }

      long use_count() const {
        return this->my_use_count;
      }
    };

// -------------------------------------------------------------------------- //
// counted_impl
// -------------------------------------------------------------------------- //
    template<class T> 
    class counted_impl: public counted_base {
    private:
      T* px;

      counted_impl(counted_impl const&);
      counted_impl& operator= (counted_impl const&);

    public:
      explicit counted_impl(T* px): px(px) {}

      virtual void dispose() {
        delete px;
      }
    };

    template<class P, class D> 
    class counted_impl_d: public counted_base {
    private:
      P px;
      D dx;

      counted_impl_d(counted_impl_d const&);
      counted_impl_d& operator= (counted_impl_d const&);

    public:
      counted_impl_d(P px, D dx): px(px), dx(dx) {}

      virtual void dispose() {
        dx(px);
      }
    };

// -------------------------------------------------------------------------- //
// shared_count
// -------------------------------------------------------------------------- //
    class shared_count {
    private:
      counted_base* pi;

    public:
      shared_count(): pi(0) {}

      template<class Y> 
      explicit shared_count(Y* p): pi(0) {
        pi = new counted_impl<Y>(p);
        if(pi == 0) {
          delete p;
          throw std::bad_alloc();
        }
      }

      shared_count(shared_count const& r): pi(r.pi) {
        if(pi != 0) 
          pi->add_ref_copy();
      }

      template<class P, class D> 
      shared_count(P p, D d): pi(0) {
        pi = new counted_impl_d<P, D>(p, d);
        if(pi == 0) {
          d(p);
          throw std::bad_alloc();
        }
      }


      shared_count& operator= (shared_count const& r) {
        counted_base* tmp = r.pi;
        if(tmp != pi) {
          if(tmp != 0)
            tmp->add_ref_copy();
          if(pi != 0)
            pi->release();
          pi = tmp;
        }
        return *this;
      }

      ~shared_count() {
        if(pi != 0)
          pi->release();
      }

      void swap(shared_count& r) {
        counted_base* tmp = r.pi;
        r.pi = pi;
        pi = tmp;
      }

      long use_count() const {
        return pi != 0 ? pi->use_count() : 0;
      }

      bool unique() const {
        return use_count() == 1;
      }
    };

  } // namespace detail


// -------------------------------------------------------------------------- //
// shared_ptr
// -------------------------------------------------------------------------- //
  template<class T> 
  class shared_ptr {
  private:
    template<class Y> friend class shared_ptr;

    T* px;
    detail::shared_count pn;

  public:
    typedef shared_ptr<T> this_type;
    typedef T element_type;
    typedef T value_type;

    shared_ptr(): px(0), pn() {}
    
    template<class Y> 
    explicit shared_ptr(Y* p): px(p), pn(p) {}

    template<class Y, class D>
    explicit shared_ptr(Y* p, D* d): px(p), pn(p, d) {}

    void swap(shared_ptr<T> & other) {
      std::swap(px, other.px);
      pn.swap(other.pn);
    }

    bool operator! () const {
      return px == 0;
    }

    typedef T* this_type::* unspecified_bool_type;

    operator unspecified_bool_type() const {
      return (px == 0)?(0):(&this_type::px);
    }

    T& operator* () const {
      assert(px != 0);
      return *px;
    }

    T* operator-> () const {
      assert(px != 0);
      return px;
    }

    shared_ptr& operator=(shared_ptr const& r) {
      px = r.px;
      pn = r.pn;
      return *this;
    }

    template<class Y>
    shared_ptr& operator=(shared_ptr<Y> const& r)  {
      px = r.px;
      pn = r.pn;
      return *this;
    }

    T* get() const {
      return px;
    }

    bool unique() const {
      return pn.unique();
    }

    long use_count() const {
      return pn.use_count();
    }

    template<class Y> 
    void reset(Y* p) {
      assert(p == 0 || p != px); // catch self-reset errors
      this_type(p).swap(*this);
    }

    template<class Y, class D> 
    void reset(Y* p, D* d) {
      this_type(p, d).swap(*this);
    }

    template<class U> 
    bool operator<(const shared_ptr<U>& that) const {
      return pn < that.pn;
    }
  };

  template<class T, class U> 
  inline bool operator==(shared_ptr<T> const & a, shared_ptr<U> const & b) {
    return a.get() == b.get();
  }

  template<class T, class U> 
  inline bool operator!=(shared_ptr<T> const & a, shared_ptr<U> const & b) {
    return a.get() != b.get();
  }

  template<class T> inline void swap(shared_ptr<T> & a, shared_ptr<T> & b) {
    a.swap(b);
  }

  namespace detail {
    template<class T> class array_deleter {
    public:
      typedef void result_type;
      typedef T * argument_type;
      void operator()(T * x) const {
        delete [] x;
      }
    };
  } // namespace detail


  // -------------------------------------------------------------------------- //
  // shared_array
  // -------------------------------------------------------------------------- //
  template<class T>
  class shared_array {
  private:
    T* px;
    detail::shared_count pn;

  public:
    typedef shared_array<T> this_type;
    typedef detail::array_deleter<T> deleter;
    typedef T element_type;
    typedef T value_type;

    explicit shared_array(T* p = 0): px(p), pn(p, deleter()) {}

    template<class D> 
    shared_array(T* p, D d): px(p), pn(p, d) {}

    void swap(shared_array<T>& other) {
      std::swap(px, other.px);
      pn.swap(other.pn);
    }

    void reset(T* p = 0) {
      assert(p == 0 || p != px);
      this_type(p).swap(*this);
    }

    template<class D>
    void reset(T* p, D d){
      this_type(p, d).swap(*this);
    }

    T& operator[] (std::ptrdiff_t i) const {
      assert(px != 0);
      assert(i >= 0);
      return px[i];
    }

    T* get() const {
      return px;
    }

    typedef T* this_type::* unspecified_bool_type;

    operator unspecified_bool_type() const {
      return (px == 0)?(0):(&this_type::px);
    }

    bool operator! () const {
      return px == 0;
    }
  };

  template<class T>
  inline bool operator==(shared_array<T> const & a, shared_array<T> const & b) {
    return a.get() == b.get();
  }

  template<class T> 
  inline bool operator!=(shared_array<T> const & a, shared_array<T> const & b) {
    return a.get() != b.get();
  }

  template<class T>
  inline bool operator<(shared_array<T> const & a, shared_array<T> const & b) {
    return std::less<T*>()(a.get(), b.get());
  }

  template<class T> 
  void swap(shared_array<T> & a, shared_array<T> & b) {
    a.swap(b);
  }

  // -------------------------------------------------------------------------- //
  // scoped_ptr
  // -------------------------------------------------------------------------- //
  template<class T> class scoped_ptr {
  private:
    T* ptr;

    scoped_ptr(const scoped_ptr&);
    scoped_ptr& operator=(const scoped_ptr&);

    typedef scoped_ptr<T> this_type;

  public:
    typedef T element_type;

    explicit scoped_ptr(T* p = 0): ptr(p) {}

    explicit scoped_ptr(std::auto_ptr<T> p): ptr(p.release()) {}

    ~scoped_ptr() {
      delete ptr;
    }

    void reset(T* p = 0) {
      assert(p == 0 || p != ptr);
      this_type(p).swap(*this);
    }

    T& operator*() const {
      assert(ptr != 0);
      return *ptr;
    }

    T* operator->() const {
      assert(ptr != 0);
      return ptr;
    }

    T* get() const {
      return ptr;
    }

    typedef T* this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const {
      return ptr == 0 ? 0 : &this_type::ptr;
    }

    bool operator!() const {
      return ptr == 0;
    }

    void swap(scoped_ptr& b) {
      T* tmp = b.ptr;
      b.ptr = ptr;
      ptr = tmp;
    }
  };

  template<class T> inline void swap(scoped_ptr<T>& a, scoped_ptr<T>& b) {
    a.swap(b);
  }


  // -------------------------------------------------------------------------- //
  // scoped_array
  // -------------------------------------------------------------------------- //
  template<class T> class scoped_array {
  private:
    T * ptr;

    scoped_array(scoped_array const &);
    scoped_array & operator=(scoped_array const &);

    typedef scoped_array<T> this_type;

  public:
    typedef T element_type;

    explicit scoped_array(T* p = 0): ptr(p) {}

    ~scoped_array() {
      delete[] ptr;
    }

    void reset(T* p = 0) {
      assert(p == 0 || p != ptr);
      this_type(p).swap(*this);
    }

    T& operator[](std::ptrdiff_t i) const {
      assert(ptr != 0);
      assert(i >= 0);
      return ptr[i];
    }

    T* get() const {
      return ptr;
    }

    typedef T * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const {
      return ptr == 0 ? 0 : &this_type::ptr;
    }

    bool operator!() const {
      return ptr == 0;
    }

    void swap(scoped_array& b) {
      T* tmp = b.ptr;
      b.ptr = ptr;
      ptr = tmp;
    }
  };

  template<class T> inline void swap(scoped_array<T>& a, scoped_array<T>& b) {
    a.swap(b);
  }

} // namespace arx

#endif // ARX_USE_BOOST

#endif // __ARX_SMART_PTR__

