#include "random.h"
#include "macro.h"

unsigned int _randomValue = 0x0001;

unsigned int nextRandom() {
    _randomValue ^= _randomValue << 7;
    _randomValue ^= _randomValue >> 9;
    _randomValue ^= _randomValue << 8;
    return _randomValue;
}

unsigned char nextRandom8() {
    nextRandom();
    return LSB(_randomValue);
}
