#include "json.h"
#include <sstream>

namespace json{
    bool isWhiteChar(const char ch){
        return (ch == 9 || ch == 10 || ch == 13 || ch == 32);
    }
    void ignoreWhiteSpace(size_t &pos, const std::string json){
        while (pos < json.length() && isWhiteChar(json[pos])) {
            pos++;
        }
    }
    bool parseString(size_t &pos, const std::string json, std::string &value){
        std::stringstream ss;
        if (pos == json.length()){
            return false;
        }
        if (json[pos] != '"'){ return false; }
        pos++;
        bool lastCharWasBackSlash = false;
        while (pos < json.length()) {
            switch (json[pos]){
                case '\\':
                    if (lastCharWasBackSlash){
                        ss << '\\';
                        lastCharWasBackSlash = false;
                    } else {
                        lastCharWasBackSlash = true;
                    }
                break;
                case '"':
                    if (lastCharWasBackSlash){
                        ss << '"';
                        lastCharWasBackSlash = false;
                    } else {
                        value.assign(ss.str());
                        pos++;
                        return true;
                    }
                break;
                case 'b':
                    if (lastCharWasBackSlash) {
                        ss << '\b';
                        lastCharWasBackSlash = false;
                    } else {
                        ss << 'b';
                    }
                break;
                case 'f':
                    if (lastCharWasBackSlash) {
                        ss << '\f';
                        lastCharWasBackSlash = false;
                    } else {
                        ss << 'f';
                    }
                break;
                case 'n':
                    if (lastCharWasBackSlash) {
                        ss << '\n';
                        lastCharWasBackSlash = false;
                    } else {
                        ss << 'n';
                    }
                break;
                case 'r':
                    if (lastCharWasBackSlash) {
                        ss << '\r';
                        lastCharWasBackSlash = false;
                    } else {
                        ss << 'r';
                    }
                break;
                case 't':
                    if (lastCharWasBackSlash) {
                        ss << '\t';
                        lastCharWasBackSlash = false;
                    } else {
                        ss << 't';
                    }
                break;

                case '/':
                    lastCharWasBackSlash = false;
                    ss << '/';
                    break;
                default:
                    if (lastCharWasBackSlash){
                        return false;
                    }
                    ss << json[pos];
                    break;
                break;
            }
            
            pos++;
        }

        return false;
    }
    /*
std::string escape_string( const std::string &input, const bool quote = false ) {
    static std::string map[256], *once = 0;
    if( !once ) {
        // base
        for( int i = 0; i < 256; ++i ) {
            map[ i ] = std::string() + char(i);
        }
        // non-printable
        for( int i = 0; i < 32; ++i ) {
            std::stringstream str;
            str << "\\u" << std::hex << std::setw(4) << std::setfill('0') << i;
            map[ i ] = str.str();
        }
        // exceptions
        map[ byte('"') ] = "\\\"";
        map[ byte('\\') ] = "\\\\";
        map[ byte('/') ] = "\\/";
        map[ byte('\b') ] = "\\b";
        map[ byte('\f') ] = "\\f";
        map[ byte('\n') ] = "\\n";
        map[ byte('\r') ] = "\\r";
        map[ byte('\t') ] = "\\t";

        once = map;
    }
    std::string output;
    output.reserve( input.size() * 2 + 2 ); // worst scenario
    if( quote ) output += '"';
    for( std::string::const_iterator it = input.begin(), end = input.end(); it != end; ++it )
        output += map[ byte(*it) ];
    if( quote ) output += '"';
    return output;
}
*/

    void Object::import(const std::string key, const Value &value) {
                std::map<std::string, Value*>::iterator found = this->valueMap.find(key);
                if (found != this->valueMap.end()){
                    delete found->second;
                }

                this->valueMap[key] = new Value( value );
            }

    bool Object::freeItem(const std::string key, bool removeItFromMap) {
                std::map<std::string, Value*>::iterator found = this->valueMap.find(key);
                if (found != this->valueMap.end()){
                    delete found->second;
                    if (removeItFromMap) {
                        this->valueMap.erase(found); 
                    }
                    return true;
                }

                return false;
            }

    void Object::set( std::string key,  int value) {
        this->freeItem(key, false);
        this->valueMap[key] = new Value( value );
    }

    int Object::getCount() {
        return this->valueMap.size();
    }

    void Object::clear() {        
        for (auto const& item : this->valueMap) {
            delete item.second;
        }

        this->valueMap.clear();
    }

    const Value* Object::get( std::string key) {
        return this->valueMap[key];
    }
    bool Object::hasKey(std::string key){
        return this->valueMap.find( key ) != this->valueMap.end();
    }


    std::string Object::json(){
        std::stringstream ss;        
        return this->json(ss);
    }
    std::string Object::json(std::stringstream &ss){
        ss << "{";
        bool first = true;
        for (auto const& x : this->valueMap)
        {
            if (first){
                first = false;
            } else {
                ss << ",";
            }
            ss << "\"" << x.first << "\""   // string (key)
                    << ':';
            
            x.second->json(ss);
        }
        ss << "}";
        return ss.str();        
    }
        
    int Object::parse(const std::string json){
        size_t pos = 0;
        return this->parse(pos, json);
    }

    int Object::parse(size_t &pos, const std::string json) {
        this->clear();
        ignoreWhiteSpace(pos, json);
        if (pos == json.length() || json[pos] != '{'){
            return 1;
        }

        pos++;
        std::string key;

        while (pos < json.length()){
            ignoreWhiteSpace(pos, json);
            char ch = json[pos];            
            if (ch == '"'){
                if (!parseString(pos, json, key)){
                    return 2;
                }

                ignoreWhiteSpace(pos, json);
                if (pos == json.length()){
                    return 3;
                }
                ch = json[pos];
                if (ch != ':'){
                    return 4;
                }

                pos++;
                ignoreWhiteSpace(pos, json);
                if (pos == json.length()){
                    return 5;
                }
                Value * value = new Value();
                int parseValue = value->parse(pos, json);
                if (parseValue != 0){
                    return parseValue;
                }
                ignoreWhiteSpace(pos, json);
                if (pos == json.length()){
                    return 6;
                }
                ch = json[pos];
                if (ch == ','){
                    this->freeItem(key, false);
                    this->valueMap[key] = value;
                    pos++;
                    continue;
                }

                if (ch == '}'){
                    this->freeItem(key, false);
                    this->valueMap[key] = value;
                    pos++;
                    return 0;
                }
                
                delete value;
                return 7;
            } else {
                if (ch == '}'){
                    return 0;
                } else {
                    return 8;
                }
            } 
        }

        return false;
    }

// ---------------------- Value ---------------------
    void Value::json(std::stringstream &ss){
        switch(this->type){
            case intType:
                ss << this->valueInt;
                break;
            case nullType:
                ss << "null";
                break;
            case stringType:
                ss << "uups";
                break;
        }
    }

    int Value::parse(size_t &pos, const std::string json){
        if (json.length() == pos){
            return 100;
        }
        switch (json[pos]){
            case 'n': //null
              return 102;
            break;
            case '"': //string
              return 103;
            break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9': //number
                return 104;
            break;
            case '{': //object
                return 105;
            break;
            case '[':  //array          
                return 105;
            break;
            case 't':  //true
                return 106;
            break;
            case 'f': //false
                return 107;
            break;
            default:
              return 101;
        }

        return 0;
    }

}

