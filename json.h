#ifndef JSON_H
#define JSON_H

#include <map>
#include <vector>
#include <string.h>

namespace json
{
    bool parseString(size_t &pos, const std::string json, std::string &value);

    class Value;
    class Object;
    class Array;

    class Object {
        private:
            std::map<std::string, Value*> valueMap;
            bool freeItem(const std::string key, bool removeItFromMap);
            std::string json(std::stringstream &ss);
        public:
            Object() {
            }

            void import(const std::string key, const Value &value);
            bool remove(const std::string key) {                
                return this->freeItem(key, true);
            }
        
            void set( std::string key,  int value);
            const Value* get( std::string key);
            bool hasKey(std::string key);
            int getCount();
            void clear();
            std::string json();

            int parse(const std::string json);
            int parse(size_t &pos, const std::string json);
    };

    class Array {
        private:
            std::vector<Value*> values;
        public:
            Array(){                
            }
    };

    class Value {
        public:
          enum Type {
            nullType,
            intType,
            stringType/*,
            BOOL,
            NULL,
            ARRAY_,
            OBJECT_,
            INVALID_*/
        };
        private:
          Value::Type type;
          union {
            int valueInt;
            std::string* valueString;
        };
        public:
            void json(std::stringstream &ss);
            Value(){
                this->type = nullType;
            }
            Value(const int value){
                this->type = intType;
                this->valueInt = value;
            }

            ~Value(){}
            void import( const Value &other ){
                if (this != &other){

                }
            }
            void reset(){
                //muna að hreinsa
                this->type = nullType; 
            }

            void set(const int value){
                this->reset();
                this->type = intType;
                this->valueInt = value;
            }

            int parse(size_t &pos, const std::string json);
    };
}
#endif
