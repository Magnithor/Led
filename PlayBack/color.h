#ifndef COLOR_H
#define COLOR_H

#include <stdint.h>

class Color {
    public:
      virtual void updateColor(int index, uint8_t &red, uint8_t &green, uint8_t &blue, uint8_t &brightness) = 0;
      void updateColor(int index, double &red, double &green, double &blue, double &brightness) {
        uint8_t r, g, b, br;
        this->updateColor(index, r, g, b, br);
        red = r;
        green = g;
        blue = b;
        brightness = br;
      }
      virtual ~Color() { }
};
#endif
