#ifndef COLOR_PATTERN_H
#define COLOR_PATTERN_H

#include "color.h"
#include <stdint.h>
#include <string>
#include "../json.h"
#include <vector>

class ColorPattern : public Color {
    private:
      class ColorPatternItem {
      public:
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t brightness; 
        int size;
      public:
        ColorPatternItem(json::Object* o) {
           this->red = 0;
           this->green = 0;
           this->blue = 0;
           this->brightness = 0;
           this->size = 1;

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
      };

      std::vector<ColorPatternItem*> values;

    public:
       ColorPattern(json::Object* o) {
           json::Value* v = o->get(std::string("items"));
           json::Array* a = v->getArray();
           for (int i=0; i < a->count(); i++) {
               ColorPatternItem* item = new ColorPatternItem(a->get(i)->getObject());
               this->values.push_back(item);
           }
       }
      ~ColorPattern() {
        for (auto &item : this->values) {
            delete item;
        }

        this->values.clear();
      }

      void updateColor(int index, uint8_t &redValue, uint8_t &greenValue, uint8_t &blueValue, uint8_t &brightnessValue) {
          ColorPatternItem* item = this->values.at(index % this->values.size());

          redValue = item->red;
          greenValue = item->green;
          blueValue = item->blue;
          brightnessValue = item->brightness;          
      }

};

#endif
