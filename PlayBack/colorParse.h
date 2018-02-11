#ifndef COLOR_PARSE_H
#define COLOR_PARSE_H

#include <string>
#include "colorSolid.h"
#include "colorPattern.h"
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

            json::Object* o = v->getObject();           
            return new ColorSolid(o);
        }

        key = std::string("patternColor");
        if (value->hasKey(key)) {
            json::Value* v = value->get(key);
            if (!v->isObject()) {
                return NULL;
            }

            json::Object* o = v->getObject();           
            return new ColorPattern(o);
        }

        return NULL;
    }
};

#endif

