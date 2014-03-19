#include "SafeIdProvider.h"

namespace prec {

  arx::mutex SafeIdProvider::mutex;
  int SafeIdProvider::nextFreeId = 0;

  int SafeIdProvider::getNextFreeId() {
    int result;
    mutex.lock();
    result = nextFreeId++;
    mutex.unlock();
    return result;
  }

} // namespace prec

