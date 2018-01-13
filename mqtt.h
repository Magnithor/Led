#ifndef MQTT_H
#define MQTT_H

#include <string>
#include <stdint.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <fcntl.h>
#include <unistd.h>

#define bufferSize 80*1024
/*
# Message types
CONNECT = 0x10
CONNACK = 0x20
PUBLISH = 0x30
PUBACK = 0x40
PUBREC = 0x50
PUBREL = 0x60
PUBCOMP = 0x70
SUBSCRIBE = 0x80
SUBACK = 0x90
UNSUBSCRIBE = 0xA0
UNSUBACK = 0xB0
PINGREQ = 0xC0
PINGRESP = 0xD0
DISCONNECT = 0xE0
*/



class Mqtt{
  private:
    uint16_t port;
    std::string ip;
    bool connected, mqttConnected;
    int sockfd;
    struct sockaddr_in remoteaddr;
    char buffer[bufferSize];
    char bufferOut[bufferSize];
    int bufferPos, parsePos;
    size_t bufferOutSize;
    std::string clientId;
    uint16_t keepAlive;
    int parseState;
  private:
    void fixData() {
        printf("fixData %d %d\n", this->parsePos, this->bufferPos);
        if (this->parsePos == this->bufferPos) {
            printf("Same\n");
            this->parsePos = 0;
            this->bufferPos = 0;
            return;
        }

        int c =  this->parsePos;      
        for (int i = 0; i < this->parsePos; i++) {
            this->buffer[i] = this->buffer[c++];
        }

        this->bufferPos -= this->parsePos;
        this->parsePos = 0;
    }
    int parse(int size) {
        while (true) {
            if (this->bufferPos == this->parsePos) {
                return 1;
            }
            switch(this->parseState) {
                case 0:
                    switch (this->buffer[this->parsePos]) {
                        case 0x20:
                            this->parseState = 1;
                            this->parsePos++;
                        break;
                        default:
                        return -1;
                    }
                break;
                case 1:
                    if ((this->bufferPos - this->parsePos) < 3) {
                        printf("mqtt not big \n");
                        return 1;
                    }
                    if (this->buffer[this->parsePos] != 2){
                        return -1;
                    }

                    this->parsePos++;
                    if (this->buffer[this->parsePos] > 2) {
                        return -1;
                    }
                    bool acceptFlag;
                    acceptFlag = this->buffer[this->parsePos] == 1;
                    if (acceptFlag) {
                        printf("acceptFlag True\n");
                    } else {
                        printf("acceptFlag False\n");
                    }
                    this->parsePos++;
                    if (this->buffer[this->parseState] == 0) {
                        this->mqttConnected  = true;
                        this->parsePos++;
                        this->fixData();
                        return 0;
                    }
                    return -1;
                break;
                default:
                  return -1;
            }
        }
    }
  public:
    Mqtt(uint16_t port, std::string ip, std::string clientId, uint16_t keepAlive){
        this->port = port;
        this->ip = ip;
        this->connected = false;
        this->mqttConnected = false;
        this->clientId = clientId;
        this->keepAlive = keepAlive;


    }  
    void check(){
        printf("-\n");
        if (!connected){
            this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
            int flags = fcntl(this->sockfd, F_GETFL);
            if (fcntl(this->sockfd, F_SETFL, flags | O_NONBLOCK) == -1){
                printf("uups mqtt\n");
                exit(0);
            }
            this->mqttConnected = false;
            this->bufferPos = 0;  
            this->parsePos = 0;
            this->parseState = 0;     
            this->bufferOutSize = 0;  
            this->remoteaddr.sin_family = AF_INET;
            this->remoteaddr.sin_addr.s_addr = inet_addr(this->ip.c_str());
            this->remoteaddr.sin_port = htons(this->port);
            if (connect(this->sockfd, (struct sockaddr *)&this->remoteaddr, sizeof(this->remoteaddr)) == -1){
                if (errno == EINPROGRESS) {
					printf("    connect EINPROGRESS OK (expected)\n");
                    this->connected = true;
				} else {
                    printf("fail login..");
                }
            } else {
                this->connected = true;
            }

            if (this->connected){
                printf("Conneced\n");
                this->bufferOutSize = createConnectMqttMessage(this->clientId, this->keepAlive, this->bufferOut, bufferSize);
            }
        } else {            
            struct timeval  timeout;
            timeout.tv_sec  = 0 * 60;
            timeout.tv_usec = 10;
            fd_set rds, wds;
            printf("-1\n");
            FD_ZERO(&rds);
            printf("-2\n");

            FD_SET(this->sockfd, &rds);   
            printf("-3\n");
            wds = rds;             
            int rc = select(this->sockfd+1, &rds, &wds, NULL, &timeout);
            printf("-4\n");
            if (rc == -1) {
                printf("select fail\n");
                return;
            }

            if (FD_ISSET(this->sockfd, &wds) && this->bufferOutSize > 0){
                printf("Send..\n");
                size_t rc = send(this->sockfd, this->bufferOut, this->bufferOutSize, 0);
                if (this->bufferOutSize != rc){
                    printf("demm\n"); //todo: laga bufferout
                } else {
                    this->bufferOutSize = 0;
                }
                printf("<<<\n");
            } else {
                printf("!<<<\n");
            }

            if (FD_ISSET(this->sockfd, &rds)){
                printf("bufferPos = %d\n", this->bufferPos);
                rc = recv(this->sockfd, &this->buffer[this->bufferPos], bufferSize - this->bufferPos, 0);
                if (rc == 0){
                    printf("rc == 0\n");
                    this->connected = false;
                    close(this->sockfd);
                    return;
                }
                if (rc == -1){
                    printf("rc == -1\n");
                    this->connected = false;
                    close(this->sockfd);
                    return;
                }
                printf(">>>%d\n", rc);
                for (int i =0; i < rc; i++){
                    printf("%d ", this->buffer[i]);
                }
                printf("\n");

                switch(this->parse(rc)){
                    case -1:
                        this->connected = false;
                        printf("parse(rc) == -1\n");
                        close(this->sockfd);
                    break;
                }
                
            } else {
                printf("!>>>\n");
            }
        }
    }

   
    static int createConnectMqttMessage(std::string clientId, uint16_t keepalive, char* buffer, int size){
        int length = 12 + 2 + clientId.length();
        if (length > size) {
            return -1;
        }
        buffer[0] = 0x10; //Connect
        buffer[1] = 0x1a; //msg length
        buffer[2] = 0x00; //msg protocol name length
        buffer[3] = 0x04; //msg protocol name length
        buffer[4] = 0x4d; //M
        buffer[5] = 0x51; //Q 
        buffer[6] = 0x54; //T
        buffer[7] = 0x54; //T
        buffer[8] = 0x04; //v3.1.1
        buffer[9] = 0x02; //connect flags
        buffer[10] = keepalive >> 8;
        buffer[11] = keepalive & 0xff;
        buffer[12] = clientId.length() >> 8;
        buffer[13] = clientId.length() & 0xff;
        int pos = 14;
        for (size_t i =0; i < clientId.length(); i++){
            buffer[pos+i] = clientId[i];
        }

        return length;        
    }

    
};

#endif