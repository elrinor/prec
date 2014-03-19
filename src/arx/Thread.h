#ifndef __ARX_THREAD_H__
#define __ARX_THREAD_H__

#include "config.h"
#include "Utility.h"

#ifdef ARX_USE_BOOST
#  include <boost/thread.hpp>
namespace arx {
  using boost::mutex;
}

#else // ARX_USE_BOOST

#ifdef ARX_WIN32

#  define NOMINMAX
#  include <Windows.h>
namespace arx {
  class mutex: noncopyable {
  private:
    CRITICAL_SECTION m;

  public:
    mutex() { InitializeCriticalSection(&m); }
    ~mutex() { DeleteCriticalSection(&m); }

    void lock() { EnterCriticalSection(&m); }
    void unlock() { LeaveCriticalSection(&m); }
  };
}

#elif defined(_POSIX_THREADS)

#  include <pthread.h>
namespace arx {
  class mutex: noncopyable {
  private:
    pthread_mutex_t m;

  public:
    pthread_mutex() { pthread_mutex_init(&m, 0); }
    ~pthread_mutex() { pthread_mutex_destroy(&m); }

    void lock() { pthread_mutex_lock(&m); }
    void unlock() { pthread_mutex_unlock(&m); }
  };
}

#elif defined(ARX_DISABLE_THREADS)

namespace arx {
  class null_mutex;
  typedef null_mutex mutex;
}

#else
#  error "Threads are not supported on your system, #define ARX_DISABLE_THREADS to turn off multithreading support in ArX Library."
#endif

#endif // ARX_USE_BOOST

namespace arx {
  
  /** Mutex stub - for use as template argument in case thread safety isn't needed. */
  class null_mutex: noncopyable {
  private:
    null_mutex(const null_mutex &);
    void operator=(const null_mutex &);

  public:
    null_mutex() {}

    static void lock() {}
    static void unlock() {}
  };

}

#endif // __ARX_THREAD_H__
