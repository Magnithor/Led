#ifndef COLOR_SOLID_H
#define COLOR_SOLID_H

#include "color.h"
#include <stdint.h>
#include <string>
#include "../json.h"

class ColorSolid : public Color {
    public:
      uint8_t red;
      uint8_t green;
      uint8_t blue;
      uint8_t brightness;
    public:
      ColorSolid(json::Object* o) {
           this->red = 0;
           this->green = 0;
           this->blue = 0;
           this->brightness = 0;

            std::string key = std::string("red");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    this->red = c->getInt();
                }
            }

            key = std::string("green");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    this->green = c->getInt();
                }
            }

            key = std::string("blue");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    this->blue = c->getInt();
                }
            }

            key = std::string("brightness");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    this->brightness = c->getInt();
                }
            }
      }
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
          blueValue = this->blue;
          brightnessValue = this->brightness;
      }

};

#endif
