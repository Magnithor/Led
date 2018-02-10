#ifndef COLOR_SOLID_H
#define COLOR_SOLID_H

#include "color.h"
#include <stdint.h>


class ColorSolid : public Color {
    public:
      uint8_t red;
      uint8_t green;
      uint8_t blue;
      uint8_t brightness;
    public:
      ColorSolid(uint8_t red, uint8_t green, uint8_t blue, uint8_t brightness) {
          this->red = red;
          this->green = green;
          this->blue = blue;
          this->brightness = brightness;
      }
      ~ColorSolid() {
          
      }

      void updateColor(int index, uint8_t &redValue, uint8_t &greenValue, uint8_t &blueValue, uint8_t &brightnessValue) {
          redValue = this->red;
          greenValue = this->green;
          blueValue = this->brightness;
          brightnessValue = this->brightness;
      }

};

#endif
