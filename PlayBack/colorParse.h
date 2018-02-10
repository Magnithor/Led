#ifndef COLOR_PARSE_H
#define COLOR_PARSE_H

#include <string>
#include "colorSolid.h"
#include "color.h"
#include "../json.h"

class ColorParse {
    public:
    static Color* Parse(const json::Object* value) 
    {
        std::string key = std::string("solidColor");
        if (value->hasKey(key)) {
            json::Value* v = value->get(key);
            if (!v->isObject()) {
                return NULL;
            }

            uint8_t red = 0;
            uint8_t green = 0;
            uint8_t blue = 0;
            uint8_t brightness = 0;

            json::Object* o = v->getObject();
            key = std::string("red");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    red = c->getInt();
                }
            }

            key = std::string("green");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    green = c->getInt();
                }
            }

            key = std::string("blue");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    blue = c->getInt();
                }
            }

            key = std::string("brightness");
            if (o->hasKey(key)){
                json::Value* c = o->get(key);
                if (c->isInt()){
                    brightness = c->getInt();
                }
            }

            return new ColorSolid(red, green, blue, brightness);
        }

        return NULL;
    }
};

#endif

