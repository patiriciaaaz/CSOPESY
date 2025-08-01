#include <algorithm>
#include "ProcessManager.h"

bool isPowerOfTwo(int x) {
    return (x & (x - 1)) == 0;
}

bool isValidMemorySize(int size) {
    return (size >= 64 && size <= 65536) && isPowerOfTwo(size);
}
