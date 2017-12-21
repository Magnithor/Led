#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/ioctl.h>

#include <sys/select.h>

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <time.h>

// https://www.ibm.com/support/knowledgecenter/en/ssw_i5_54/rzab6/xnonblock.htm

#define bufferSize 80*1024
#define httpConnectionsLength 20

enum HttpMethod { None, GET, POST };
enum class HttpParse { GetMore, Error, Ok};


class HttpConnection {
    private:
       int state;
       int httpStatus;
       std::string responseData;
       std::string responseContentType;
    public:
        char* sendData;
        int   sendPos;
        int   sendLength;
    public:    
       char ip[60];
       int port;
       char data[bufferSize];  
       std::string path;         
       HttpMethod headerMethod;
       int dataLength; 
       time_t connectTime;
       time_t lastReciveTime;
    private:
       void ClearState() {
            this->state = -1;
            this->ip[0] = 0;
            this->data[0] = 0;
            this->dataLength = 0;
            this->headerMethod = None;
            this->httpStatus = 200;
            this->path = std::string("");
            this->responseData = std::string("");
            this->responseContentType = std::string("");
            this->sendData = nullptr;
            this->sendPos = 0;
            this->sendLength = 0;
       }
       void FixData(int pos) {     
           int c =  pos;      
           for (int i = 0; i < pos; i++){
               this->data[i] = this->data[c++];
           }

           this->dataLength -= pos; 
       }
       int FindSpace(int pos) {        
           while (pos < this->dataLength){
               if (this->data[pos] == ' '){
                   return pos;
               }
               if (this->data[pos] == '\n'){
                   return -2;
               }
               pos++;
           }
           return -1;
       }
       int FindNewLine(int pos){
           while (pos < this->dataLength){
               if (this->data[pos] == '\n'){
                   return pos;
               }
               
               pos++;
           }
           return -1;           
       }
       void Copy(int pos, int toPos, std::string& to){
            to.assign(this->data, pos, toPos-pos);
       }
    public:
        HttpConnection() {
            this->ClearState();
        }

        void Connect(sockaddr_in &client) { 
            gmtime(&this->connectTime);           
            this->ClearState();
            int err = getnameinfo((struct sockaddr*)&client, sizeof(client), this->ip, sizeof(this->ip), 0, 0, NI_NUMERICHOST);
            if (((struct sockaddr*)&client)->sa_family == AF_INET) {
                this->port = ntohs(((struct sockaddr_in*)&client)->sin_port);
            } else {
                this->port = ntohs(((struct sockaddr_in6*)&client)->sin6_port);
            }       
            this->state = 1;
            printf("Connect - %s:%u\n", this->ip, this->port);
        }

        HttpParse Received(char* data, int read) {
            gmtime(&this->lastReciveTime);    
            if (this->dataLength + read > bufferSize) {
                return HttpParse::Error; //buffer overflow
            }
            for (int i=0; i < read; i++){
                this->data[this->dataLength++] = data[i];
            }

            int pos = 0;
            int findSpace, findNewLine;
            bool run = true;
            while (run){
                if (this->dataLength == 0) {
                    return HttpParse::GetMore;
                }

                switch (this->state){
                    case -1: // Er  ekki buinn að ger connect;
                        return HttpParse::Error; 
                    case 1:  // http method
                        if (this->dataLength - pos == 0) {
                            return HttpParse::GetMore;
                        }  
                        switch (this->data[pos]){
                            case 'G':
                                if (this->dataLength - pos < 4){
                                    this->FixData(pos);
                                    return HttpParse::GetMore;
                                }
                                if (this->data[pos+1] != 'E' || 
                                    this->data[pos+2] != 'T' || 
                                    this->data[pos+3] != ' '){
                                    return HttpParse::Error; // ekki get....                                    
                                }
                                this->headerMethod = GET;
                                pos = 4;
                                this->state = 2;
                                break;
                            case 'P':
                                if (this->dataLength - pos < 5){
                                    this->FixData(pos);
                                    return HttpParse::GetMore;
                                }
                                if (this->data[pos+1] != 'O' || 
                                    this->data[pos+2] != 'S' || 
                                    this->data[pos+3] != 'T' || 
                                    this->data[pos+4] != ' '){
                                    return HttpParse::Error; // ekki post....                                    
                                }
                                this->headerMethod = POST;
                                pos = 5;
                                this->state = 2;
                                break;
                            default: //Er ekki get eða post hætta
                                return HttpParse::Error;
                        }                       
                        break;
                    case 2:  // http Path
                        findSpace = this->FindSpace(pos);
                        if (findSpace == -2){
                            return HttpParse::Error;
                        }
                        if (findSpace == -1){
                            this->FixData(pos);
                            return HttpParse::GetMore;   
                        }

                        this->Copy(pos, findSpace, this->path);
                        pos = findSpace + 1;
                        this->state = 3;
                        break;
                    case 3:  // http Protocol
                        findNewLine = this->FindNewLine(pos);
                        if (findNewLine == -1){
                            this->FixData(pos);
                            return HttpParse::GetMore;
                        }
                        pos = findNewLine + 1;
                        this->state = 4;                        
                        break;
                    case 4:  // Header Attr
                        findNewLine = this->FindNewLine(pos);
                        if (findNewLine == -1){
                            this->FixData(pos);
                            return HttpParse::GetMore;
                        }
                        //printf("pos = %d, findNewLine = %d, delta = %d\n", pos, findNewLine, findNewLine - pos);
                        if (findNewLine - pos == 1){
                            run = false;
                        }
                        pos = findNewLine + 1;
                        break;
                    default: // Á ekki að fara hingað
                        return HttpParse::Error;
                }
            }

           // printf("method = %d\n", this->headerMethod);
           // printf("path = %s\n", this->path.c_str());
            

            //printf("new data read %d -%s:%u\n", read, this->ip, this->port);
            data[read]=0;
            //printf("%s\n", data);
            return HttpParse::Ok;            
        }
        void Close() {
            if (this->state == -1){
                return;
            }

            printf("Close - %s:%u\n\n", this->ip, this->port);
            this->ClearState();
        }
    
        void SetResponseData(std::string value){
            this->responseData = value;
        }
        void SetResponseHttpStatus(int status){
            this->httpStatus = status;
        }
        void SetResponseContentType(std::string value){
            this->responseContentType = value;
        }
        std::string GetResponse(){
            std::string status;
            switch(this->httpStatus){
                case 200: status = std::string("200 OK"); break;
                case 404: status = std::string("404 Not Found"); break;
                default: status = std::to_string(this->httpStatus); break;
            }

            //Content-Type: text/html\n

            std::stringstream ss;
            ss << std::string("HTTP/1.1 ") << status;
            ss << std::string("\nConnection: close\nAccept-Ranges: bytes\nContent-Length: ");
            ss << std::to_string(this->responseData.length()) << std::string("\n");
            if (this->responseContentType.length() != 0){
                ss << std::string("Content-Type: ");
                ss << this->responseContentType;
                ss << std::string("\n");
            }

            ss << std::string("\n");
            ss << this->responseData;
            return ss.str();
        }
};

typedef void (*HttpResponseFunc)(HttpConnection*);

class HttpServer {
    private:
      HttpConnection httpConnections[httpConnectionsLength];
      uint16_t port;
      int listen_sd;
      int max_sd;
      char   buffer[bufferSize];
      fd_set        master_set, working_set;
      HttpResponseFunc httpResponseFunc;
    public:
        HttpServer(uint16_t port, HttpResponseFunc response) {
            this->httpResponseFunc = response;
            this->port = port;
            this->listen_sd = -1;  
        }

        bool Start(){
            struct sockaddr_in   addr;
            int on = 1;
            int rc;
            struct sockaddr_in serv_addr, cli_addr;
            /*************************************************************/
            /* Create an AF_INET stream socket to receive incoming       */
            /* connections on                                            */
            /*************************************************************/
            this->listen_sd = socket(AF_INET, SOCK_STREAM, 0);
            if (listen_sd < 0)
            {
                this->Error("socket() failed");
            }

            /*************************************************************/
            /* Allow socket descriptor to be reuseable                   */
            /*************************************************************/
            rc = setsockopt(this->listen_sd, SOL_SOCKET,  SO_REUSEADDR, (char *)&on, sizeof(on));
            if (rc < 0)
            {
                this->Error("setsockopt() failed");
            }

            /*************************************************************/
            /* Set socket to be nonblocking. All of the sockets for    */
            /* the incoming connections will also be nonblocking since  */
            /* they will inherit that state from the listening socket.   */
            /*************************************************************/
            rc = ioctl(this->listen_sd, FIONBIO, (char *)&on);
            if (rc < 0)
            {
                this->Error("ioctl() failed");
            }

            /*************************************************************/
            /* Bind the socket                                           */
            /*************************************************************/
            memset(&addr, 0, sizeof(addr));
            addr.sin_family      = AF_INET;
            addr.sin_addr.s_addr = htonl(INADDR_ANY);
            addr.sin_port        = htons(this->port);
            rc = bind(this->listen_sd, (struct sockaddr *)&addr, sizeof(addr));
            if (rc < 0)
            {
                this->Error("bind() failed");
            }

            /*************************************************************/
            /* Set the listen back log                                   */
            /*************************************************************/
            rc = listen(this->listen_sd, 32);
            if (rc < 0)
            {
                this->Error("listen() failed");
            }

            /*************************************************************/
            /* Initialize the master fd_set                              */
            /*************************************************************/
            FD_ZERO(&this->master_set);
            this->max_sd = listen_sd;
            FD_SET(this->listen_sd, &this->master_set);

            return true;
        }

        bool Check()
        {
            int on = 1;
            
            int rc, i, new_sd;
            int end_server = false;
            int close_conn = false; //?
            struct timeval       timeout;
            timeout.tv_sec  = 0 * 60;
            timeout.tv_usec = 10;

            fd_set writeFds; 
            FD_ZERO (&writeFds);
            /* initialize bit mask */
            FD_SET (this->max_sd, &writeFds);

            /**********************************************************/
            /* Copy the master fd_set over to the working fd_set.     */
            /**********************************************************/
            memcpy(&this->working_set, &this->master_set, sizeof(this->master_set));

            /**********************************************************/
            /* Call select() and wait 5 minutes for it to complete.   */
            /**********************************************************/
            rc = select(this->max_sd + 1, &this->working_set,  &writeFds, NULL, &timeout);

            /**********************************************************/
            /* Check to see if the select call failed.                */
            /**********************************************************/
            if (rc < 0)
            {
                printf("  select() failed");
                return false;
            }

            /**********************************************************/
            /* Check to see if the 5 minute time out expired.         */
            /**********************************************************/
            if (rc == 0)
            {
              //  printf("rc == 0\n");
                return false;
            }

            /**********************************************************/
            /* One or more descriptors are readable.  Need to         */
            /* determine which ones they are.                         */
            /**********************************************************/
            int desc_ready = rc;
            for (i=0; i <= this->max_sd  &&  desc_ready > 0; ++i)
            {
                /*******************************************************/
                /* Check to see if this descriptor is ready            */
                /*******************************************************/
                if (this->httpConnections[i].sendData != nullptr && FD_ISSET(i, &writeFds)){
                    close_conn = false;

                    while (this->httpConnections[i].sendPos < this->httpConnections[i].sendLength) {
                        //printf(".\n");
                        int rc = send(i, this->httpConnections[i].sendData+this->httpConnections[i].sendPos,  
                            this->httpConnections[i].sendLength-this->httpConnections[i].sendPos, 0);                                 
                        if (rc < 0)
                        {
                          //  printf("  send() failed 2 %d - %d\n", rc, errno);
                            break;
                        }

                        this->httpConnections[i].sendPos += rc;
                        //printf("Sent:: %d : %d af %d\n", rc, this->httpConnections[i].sendPos, this->httpConnections[i].sendLength);
                    }

                    //printf("Send:: done\n");
                    if (this->httpConnections[i].sendPos >= this->httpConnections[i].sendLength){
                         printf("Send::close.. done\n");
                        close_conn = true;
                         if (close_conn)
                        {
                            this->httpConnections[i].Close();
                            close(i);
                            FD_CLR(i, &this->master_set);
                            if (i == this->max_sd)
                            {
                                while (FD_ISSET(max_sd, &this->master_set) == false)
                                max_sd -= 1;
                            }
                        }
                    }
                    break;
                        
                }
                
                if (FD_ISSET(i, &this->working_set))
                {
                /****************************************************/
                /* A descriptor was found that was readable - one   */
                /* less has to be looked for.  This is being done   */
                /* so that we can stop looking at the working set   */
                /* once we have found all of the descriptors that   */
                /* were ready.                                      */
                /****************************************************/
                desc_ready -= 1;

                /****************************************************/
                /* Check to see if this is the listening socket     */
                /****************************************************/
                if (i == this->listen_sd)
                {
                    /*************************************************/
                    /* Accept all incoming connections that are      */
                    /* queued up on the listening socket before we   */
                    /* loop back and call select again.              */
                    /*************************************************/
                    do
                    {
                        /**********************************************/
                        /* Accept each incoming connection.  If       */
                        /* accept fails with EWOULDBLOCK, then we     */
                        /* have accepted all of them.  Any other      */
                        /* failure on accept will cause us to end the */
                        /* server.                                    */
                        /**********************************************/
                        sockaddr_in client;
                        client.sin_family = AF_INET;
                        socklen_t c_len = sizeof(client);
                        new_sd = accept(this->listen_sd, (sockaddr*)&client, &c_len);
                        if (new_sd < 0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                            perror("  accept() failed");
                            end_server = true;
                            }
                            break;
                        }

                        rc = ioctl(new_sd, FIONBIO, (char *)&on);
                        if (rc < 0)
                        {
                            perror("ioctl() failed");
                        }

                        /**********************************************/
                        /* Add the new incoming connection to the     */
                        /* master read set                            */
                        /**********************************************/                        
                        this->httpConnections[new_sd].Connect(client);

                        FD_SET(new_sd, &this->master_set);
                        if (new_sd > this->max_sd)
                            this->max_sd = new_sd;

                        /**********************************************/
                        /* Loop back up and accept another incoming   */
                        /* connection                                 */
                        /**********************************************/
                    } while (new_sd != -1);
                }

                /****************************************************/
                /* This is not the listening socket, therefore an   */
                /* existing connection must be readable             */
                /****************************************************/
                else
                {
                    close_conn = false;
                    /*************************************************/
                    /* Receive all incoming data on this socket      */
                    /* before we loop back and call select again.    */
                    /*************************************************/
                    do
                    {                       
                        /**********************************************/
                        /* Receive data on this connection until the  */
                        /* recv fails with EWOULDBLOCK.  If any other */
                        /* failure occurs, we will close the          */
                        /* connection.                                */
                        /**********************************************/
                        rc = recv(i, this->buffer, bufferSize, 0);
                        if (rc < 0)
                        {
                            if (errno != EWOULDBLOCK)
                            {
                                perror("  recv() failed");
                                close_conn = true;
                            }

                            break;
                        }

                        /**********************************************/
                        /* Check to see if the connection has been    */
                        /* closed by the client                       */
                        /**********************************************/
                        if (rc == 0)
                        {
                            close_conn = true;
                            break;
                        }

                        /**********************************************/
                        /* Data was received                          */
                        /**********************************************/
                        int len = rc;
                        switch (this->httpConnections[i].Received(this->buffer, rc)) {
                            case HttpParse::Error:
                                close_conn = true;
                                printf("Httparse::Error\n");
                                break;
                            case HttpParse::GetMore:
                                break;
                            case HttpParse::Ok:
                                printf("Httparse::Ok\n");
                                this->httpResponseFunc(&this->httpConnections[i]);
                                std::string sendData = this->httpConnections[i].GetResponse();
                                //printf("%s\n", sendData.c_str());
                                printf("Sent %d\n", sendData.length());
                                this->httpConnections[i].sendData = (char*) sendData.c_str();
                                this->httpConnections[i].sendLength = sendData.length();
                                this->httpConnections[i].sendPos = 0;
                                while (this->httpConnections[i].sendPos < this->httpConnections[i].sendLength) {
                                   // printf(".\n");
                                    int rc = send(i, this->httpConnections[i].sendData+this->httpConnections[i].sendPos,  this->httpConnections[i].sendLength-this->httpConnections[i].sendPos, 0);
                                    if (rc < 0)
                                    {
                                     //   printf("  send() failed  %d - %d\n", rc, errno);
                                        break;
                                    }

                                    this->httpConnections[i].sendPos += rc;
                                   // printf("Sent %d : %d af %d\n", rc, this->httpConnections[i].sendPos, this->httpConnections[i].sendLength);
                                }

                                
                                if (this->httpConnections[i].sendPos >= this->httpConnections[i].sendLength){
                                    close_conn = true;
                                    printf("Send done\n");
                                }
                                break;
                        }

                    } while (true);

                    /*************************************************/
                    /* If the close_conn flag was turned on, we need */
                    /* to clean up this active connection.  This     */
                    /* clean up process includes removing the        */
                    /* descriptor from the master set and            */
                    /* determining the new maximum descriptor value  */
                    /* based on the bits that are still turned on in */
                    /* the master set.                               */
                    /*************************************************/
                    if (close_conn)
                    {
                        this->httpConnections[i].Close();
                        close(i);
                        FD_CLR(i, &this->master_set);
                        if (i == this->max_sd)
                        {
                            while (FD_ISSET(max_sd, &this->master_set) == false)
                            max_sd -= 1;
                        }
                    }
                } /* End of existing connection is readable */
                } /* End of if (FD_ISSET(i, &working_set)) */
            } /* End of loop through selectable descriptors */

        }

        bool Close()
        {
            for (int i=0; i <= this->max_sd; ++i)
            {
                if (FD_ISSET(i, &this->master_set)){
                    this->httpConnections[i].Close();
                    close(i);
                }
            }

            close(this->listen_sd);
            return true;
        }
    private:
        void Error(const char *msg)
        {
            perror(msg);
            if (this->listen_sd != -1) {
                this->Close();
            }
            exit(-1);
        }
};

#endif