#include "json.h"
#include <sstream>

namespace json{
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
                        this->valueMap.erase (found); 
                    }
                    return true;
                }

                return false;
            }

    void Object::set( std::string key,  int value) {
        this->freeItem(key, false);
        this->valueMap[key] = new Value( value );
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

}

