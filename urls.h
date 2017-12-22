#ifndef _URLS_H
#define _URLS_H

#include "httpServer.h"
#include <map>
#include <fstream>


class Urls{
    private:
         void Root(HttpConnection* httpConnection) {    
            httpConnection->SetResponseContentType(std::string("text/html"));
            std::string str = std::string("Root\n");
            httpConnection->SetResponseData(str);
        }



    private:
       // static const  
        std::map<std::string, void (Urls::*)(HttpConnection*)> urlMaps = {     {std::string("/"), &Urls::Root }        };

    public:
         void HttpResponse(HttpConnection* httpConnection)
         {    
            
            printf("--path = %s \n", httpConnection->path.c_str());
            
            auto find = this->urlMaps.find(httpConnection->path);
            if (find != this->urlMaps.end()){
                (*this.*find->second)(httpConnection);
                return;
            }
            

            if (httpConnection->path.compare("/3155.jpg") == 0) {
                httpConnection->SetResponseContentType(std::string("image/jpg"));
                std::ifstream is( "www/3155.jpg", std::ios::binary );
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

                    httpConnection->SetResponseData(std::string(buffer, length));

                    delete[] buffer;
                    return;
                }

                httpConnection->SetResponseHttpStatus(404);
                return;

            }

            httpConnection->SetResponseContentType(std::string("text/html"));
            std::string str = std::string("ok\n");
            httpConnection->SetResponseData(str);
        }
    
};

#endif