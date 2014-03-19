#ifndef __SAFEIDPROVIDER_H__
#define __SAFEIDPROVIDER_H__

#include <arx/Thread.h>

namespace prec {
  /**
   * SafeIdProvider offers an interface to get unique integer identifiers in a thread-safe manner.
   */
  class SafeIdProvider {
  private:
    static arx::mutex mutex;
    static int nextFreeId;

  public:
    static int getNextFreeId();
  };

} // namespace prec

#endif // __SAFEIDPROVIDER_H__
