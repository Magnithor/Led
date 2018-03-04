#ifndef COLOR_PATTERN_H
#define COLOR_PATTERN_H

#include "color.h"
#include <stdint.h>
#include <string>
#include "../json.h"
#include <vector>

class ColorPattern : public Color {
    private:
      int size;
    public:
      class ColorPatternItem {
      private:
        uint8_t red;
        uint8_t green;
        uint8_t blue;
        uint8_t brightness; 
        int size;
      public:
        uint8_t Red() { return this->red; }
        uint8_t Green() { return this->green; }
        uint8_t Blue() { return this->blue; }
        uint8_t Brightness() { return this->brightness; }
        int Size() { return this->size; }
      public:
        ColorPatternItem(uint8_t r, uint8_t g, uint8_t b, uint8_t l, int s){
            this->red = r;
            this->green = g;
            this->blue = b;
            this->brightness = l;
            this->size = s;
        }
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

            key = std::string("size");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    this->size = c->getInt();
                }
            }
      }
      };

      std::vector<ColorPatternItem*> values;

    public:
       ColorPattern(json::Object* o) {     
           this->size = 0;      
           json::Value* v = o->get(std::string("items"));
           json::Array* a = v->getArray();
           for (int i=0; i < a->count(); i++) {
               ColorPatternItem* item = new ColorPatternItem(a->get(i)->getObject());
               this->values.push_back(item);
               this->size += item->Size();
           }
       }
       ColorPattern(){
           this->size = 0;
       }    
      ~ColorPattern() {
        for (auto &item : this->values) {
            delete item;
        }

        this->values.clear();
      }

      void AddItem(ColorPatternItem* item) {
          this->values.push_back(item);
          this->size += item->Size();
      }

      void updateColor(int index, uint8_t &redValue, uint8_t &greenValue, uint8_t &blueValue, uint8_t &brightnessValue) {
          index = index % this->size;
          ColorPatternItem* item = NULL;
          int sum = 0;
          for (unsigned int i =0; i < this->values.size(); i++) {
              item = this->values.at(i);
              sum += item->Size();
              if (index < sum){
                  break;
              }
          }

          redValue = item->Red();
          greenValue = item->Green();
          blueValue = item->Blue();
          brightnessValue = item->Brightness();          
      }

};

#endif
