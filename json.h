#ifndef JSON_H
#define JSON_H

#include <map>
#include <vector>
#include <string.h>
#include <exception>

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
        public:
            Object() {
            }
            ~Object(){
                this->clear();
            }

            void import(const std::string key, const Value &value);
            bool remove(const std::string key) {
                return this->freeItem(key, true);
            }

            void set(std::string key, const int value);
            void set(std::string key, const double value);
            Value* get(std::string key);
            bool hasKey(std::string key);
            int count();
            void clear();
            std::string json();
            void json(std::stringstream &ss);

            int parse(const std::string json);
            int parse(size_t &pos, const std::string json);
    };

    class Array {
        private:
            std::vector<Value*> values;
        public:
            Array(){
            }
            ~Array(){
                this->clear();
            }
            void json(std::stringstream &ss);
            int parse(size_t &pos, const std::string json);
            int count() {
                return this->values.size();
            }
            Value* get(int pos) {
                return this->values.at(pos);
            }
            void clear();
    };

    class Value {
        public:
          enum Type {
            nullType,
            intType,
            stringType,
            boolType,
            arrayType,
            objectType,
            doubleType
        };
        private:
          Value::Type type;
          union {
            int valueInt;
            double valueDouble;
            std::string* valueString;
            bool valueBool;
	    Array* valueArray;
	    Object* valueObject;
        };
        public:
            void json(std::stringstream &ss);
            Value() {
                this->type = nullType;
            }
            Value(const int value) {
                this->type = intType;
                this->valueInt = value;
            }
            Value(const double value) {
                this->type = doubleType;
                this->valueDouble = value;
            }

            ~Value() {}
            void import( const Value &other ){
                if (this != &other){

                }
            }
            void reset(){
                //muna að hreinsa
                switch (this->type){
                    case stringType:
                        delete this->valueString;
                        break;
                    case arrayType:
                        delete this->valueArray;
                        break;
                    case objectType:
                        delete this->valueObject;
                        break;
                    case boolType:
                    case intType:
                    case doubleType:
                    case nullType:
		    	        break;
                }
                this->type = nullType;
            }

            void set(const int value) {
                this->reset();
                this->type = intType;
                this->valueInt = value;
            }

            void set(const double value) {
                this->reset();
                this->type = doubleType;
                this->valueDouble = value;
            }

            bool isNull(){
                return this->type == nullType;
            }
            bool isInt(){
                return this->type == intType;
            }
            int getInt(){
                if (this->type == intType){
                    return this->valueInt;
                }
                throw 1;
            }
            bool isDouble(){
                return this->type == intType || this->type == doubleType;
            }
            double getDouble(){
                if (this->type == intType){
                    return this->valueInt;
                } else if (this->type == doubleType) {
                    return this->valueDouble;
                }
                throw 1;
            }
            
            bool isBool(){
                return this->type == boolType;
            }
            int getBool(){
                if (this->type == boolType){
                    return this->valueBool;
                }
                throw 1;
            }
            bool isString(){
                return this->type == stringType;
            }
            std::string getString(){
                if (this->type == stringType){
                    return *this->valueString;
                }
                throw 1;
            }

            bool isArray(){
                return this->type == arrayType;
            }
            Array* getArray(){
                if (this->type == arrayType){
                    return this->valueArray;
                }
                throw 1;
            }
            bool isObject(){
                return this->type == objectType;
            }
            Object* getObject(){
                if (this->type == objectType){
                    return this->valueObject;
                }
                throw 1;
            }


            int parse(size_t &pos, const std::string json);
    };
}
#endif
