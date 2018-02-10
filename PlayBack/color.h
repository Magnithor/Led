#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

class Color {
    public:
      virtual void updateColor(int index, uint8_t &red, uint8_t &green, uint8_t &blue, uint8_t &brightness) = 0;
      virtual ~Color() { }
};
#endif
