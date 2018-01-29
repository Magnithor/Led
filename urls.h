#ifndef _URLS_H
#define _URLS_H

#include "httpServer.h"
#include <map>
#include <fstream>
#include "json.h"
#include <string>
#include "apa102.h"
#include "playBack.h"
#include "playBackItemSolid.h"

class Urls{
    private:
         void root(HttpConnection* httpConnection) {    
            httpConnection->setResponseContentType(std::string("text/html"));
            json::Object o;
            o.set(std::string("count"), this->apa102->getCount());
            o.set(std::string("ledStatus"), this->apa102->getLedStatus());
            httpConnection->setResponseData(o.json());
        }

        void clear(HttpConnection* httpConnection) {    
            this->playBack->clear();
            httpConnection->setResponseContentType(std::string("text/html"));
            json::Object o;
            o.set(std::string("count"), this->apa102->getCount());
            o.set(std::string("ledStatus"), this->apa102->getLedStatus());
            httpConnection->setResponseData(o.json());
        }

        void solidColor(HttpConnection* httpConnection) {    
            try {
            printf("Post data = %s\n", httpConnection->postValue.c_str());
            json::Object input;
            input.parse(httpConnection->postValue);
            
            this->playBack->push(new PlayBackItemSolid(this->apa102, 
                input.get(std::string("red"))->getInt(), 
                input.get(std::string("green"))->getInt(), 
                input.get(std::string("blue"))->getInt(), 
                input.get(std::string("brightness"))->getInt()));
            httpConnection->setResponseContentType(std::string("text/html"));
            json::Object o;
            o.set(std::string("count"), this->apa102->getCount());
            o.set(std::string("ledStatus"), this->apa102->getLedStatus());
            o.set(std::string("inputCount"), (int)input.count());
            httpConnection->setResponseData(o.json());
            } catch(...){
                httpConnection->setResponseHttpStatus(500);
            }
        }
        void solidColorAndClear(HttpConnection* httpConnection) {    
            try {
            printf("Post data = %s\n", httpConnection->postValue.c_str());
            json::Object input;
            input.parse(httpConnection->postValue);
            
            this->playBack->clear();

            this->playBack->push(new PlayBackItemSolid(this->apa102, 
                input.get(std::string("red"))->getInt(), 
                input.get(std::string("green"))->getInt(), 
                input.get(std::string("blue"))->getInt(), 
                input.get(std::string("brightness"))->getInt()));
            httpConnection->setResponseContentType(std::string("text/html"));
            json::Object o;
            o.set(std::string("count"), this->apa102->getCount());
            o.set(std::string("ledStatus"), this->apa102->getLedStatus());
            o.set(std::string("inputCount"), (int)input.count());
            httpConnection->setResponseData(o.json());
            } catch(...){
                httpConnection->setResponseHttpStatus(500);
            }
        }

    private:
       // static const  
        std::map<std::string, void (Urls::*)(HttpConnection*)> urlMaps = {    
            {std::string("/"), &Urls::root }
            ,{std::string("/clear"), &Urls::clear}
            ,{std::string("/solidColor"), &Urls::solidColor}
            ,{std::string("/solidColorAndClear"), &Urls::solidColorAndClear}
        };
        APA102 *apa102;
        PlayBack *playBack;
    public:
         Urls(APA102* apa102, PlayBack* playBack){
             this->apa102 = apa102;
             this->playBack = playBack;            
         }

         void httpResponse(HttpConnection* httpConnection)
         {            
            printf("--path = %s \n", httpConnection->path.c_str());
            
            auto find = this->urlMaps.find(httpConnection->path);
            if (find != this->urlMaps.end()){
                (*this.*find->second)(httpConnection);
                return;
            }            

            if (httpConnection->path.compare("/favicon.ico") == 0) {
                httpConnection->setResponseContentType(std::string("image/ico"));
                std::ifstream is( "www/favicon.ico", std::ios::binary );
                if (is) {
                    std::string data;
                    // get length of file:
                    is.seekg (0, is.end);
                    int length = is.tellg();
                    is.seekg (0, is.beg);

                    char * buffer = new char [length];

//                    std::cout << "Reading " << length << " characters... ";
                    // read data as a block:
                    is.read (buffer, length);

  /*                  if (is)
                        std::cout << "all characters read successfully.";
                    else
                        std::cout << "error: only " << is.gcount() << " could be read";*/
                    is.close();

                    // ...buffer contains the entire file...

                    httpConnection->setResponseData(std::string(buffer, length));

                    delete[] buffer;
                    return;
                }

                httpConnection->setResponseHttpStatus(404);
                return;

            }

            httpConnection->setResponseContentType(std::string("text/html"));
            std::string str = std::string("ok\n");
            httpConnection->setResponseData(str);
        }
    
};

#endif