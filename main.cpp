#include <signal.h>
#include <sstream>
#include "fastgpioomega2.h"
#include "apa102.h"
#include "httpServer.h"
#include "urls.h"
#include "playBack.h"
#include "playBackItemSlide.h"
#include "json.h"
#include "mqtt.h"

#define ledcount  144*3
APA102 apa102((uint8_t)0, (uint8_t)1, (uint16_t) ledcount);
PlayBack playBack(&apa102);

Urls urls(&apa102, &playBack);


HttpServer* httpServer;
//Mqtt* mqtt;

void httpResponse(HttpConnection* httpConnection);

volatile  int lastSignal = -1;

void httpResponse(HttpConnection* httpConnection) {
    urls.httpResponse(httpConnection);
}

void sleep_milliseconds(uint32_t millis) {
  struct timespec sleep;
  sleep.tv_sec = millis / 1000;
  sleep.tv_nsec = (millis % 1000) * 1000000L;
  while (clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, &sleep) && errno == EINTR);
}

void sigHandler(int signo) {
  lastSignal = signo;
}

int check() {
	if (lastSignal != -1){
		lastSignal = -1;
		printf( "\nShuting down...\n");
		printf( " * Close http\n");
		httpServer->closeAll();               
		printf( " * turn off led\n");				
		playBack.turnOff();
		printf( "exit\n");
                delete httpServer;
               // delete mqtt;
		exit(0);
	}

	httpServer->check();
   //     printf(":\n");
     //   mqtt->check();
     //   printf(".\n");
	return playBack.update();
}

void startService(){
	/* Our process ID and Session ID */
        pid_t pid, sid;

        /* Fork off the parent process */
        pid = fork();
        if (pid < 0) {
                exit(EXIT_FAILURE);
        }
        /* If we got a good PID, then
           we can exit the parent process. */
        if (pid > 0) {
                exit(EXIT_SUCCESS);
        }

        /* Change the file mode mask */
        umask(0);

        /* Open any logs here */        

        /* Create a new SID for the child process */
        sid = setsid();
        if (sid < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }



        /* Change the current working directory */
        if ((chdir("/")) < 0) {
                /* Log the failure */
                exit(EXIT_FAILURE);
        }

		/* Close out the standard file descriptors */
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

}

void loadConfig(json::Object &config){
        std::ifstream inFile;
        inFile.open("led.json");//open the input file

        std::stringstream strStream;
        strStream << inFile.rdbuf();//read the file
        config.parse(strStream.str());
}


int main(int argc, char* argv[])
{	
        json::Object config;
        loadConfig(config);
        {
                std::string httpConfigKey = std::string("httpServer");
                std::string httpConfigPort = std::string("port");
                if (!config.hasKey(httpConfigKey)){
                        printf("config missing %s\n", httpConfigKey.c_str());
                        return -1;
                }

                if (!config.get(httpConfigKey)->isObject()){
                        printf("config %s is not object\n",httpConfigKey.c_str());
                        return -1;
                }

                json::Object *httpConfig = config.get(httpConfigKey)->getObject();
                if (!httpConfig->hasKey(httpConfigPort)){
                        printf("config missing %s in %s\n", httpConfigPort.c_str(), httpConfigKey.c_str());
                        return -1;
                }
                if (!httpConfig->get(httpConfigPort)->isInt()){
                        printf("config %s in %s are not int\n", httpConfigPort.c_str(), httpConfigKey.c_str());
                        return -1;                
                }
                int httpPort = httpConfig->get(httpConfigPort)->getInt();

                httpServer = new HttpServer(httpPort, &httpResponse);
        }
/*
        {
                std::string configKey = std::string("mqtt");
                std::string configPort = std::string("port");
                std::string configIp = std::string("ip");
                std::string configClientId = std::string("clientId");
                std::string configKeepAlive = std::string("keepAlive");
                if (!config.hasKey(configKey)){
                        printf("config missing %s\n", configKey.c_str());
                        return -1;
                }

                if (!config.get(configKey)->isObject()){
                        printf("config %s is not object\n",configKey.c_str());
                        return -1;
                }

                json::Object *configObject = config.get(configKey)->getObject();
                if (!configObject->hasKey(configPort)){
                        printf("config missing %s in %s\n", configPort.c_str(), configKey.c_str());
                        return -1;
                }
                if (!configObject->get(configPort)->isInt()){
                        printf("config %s in %s are not int\n", configPort.c_str(), configKey.c_str());
                        return -1;                
                }
                int port = configObject->get(configPort)->getInt();

                if (!configObject->hasKey(configIp)){
                        printf("config missing %s in %s\n", configIp.c_str(), configKey.c_str());
                        return -1;
                }
                if (!configObject->get(configIp)->isString()){
                        printf("config %s in %s are not string\n", configIp.c_str(), configKey.c_str());
                        return -1;                
                }
                std::string ip = configObject->get(configIp)->getString();

                if (!configObject->hasKey(configClientId)){
                        printf("config missing %s in %s\n", configClientId.c_str(), configKey.c_str());
                        return -1;
                }
                if (!configObject->get(configClientId)->isString()){
                        printf("config %s in %s are not string\n", configClientId.c_str(), configKey.c_str());
                        return -1;                
                }
                std::string clientId = configObject->get(configClientId)->getString();

                if (!configObject->hasKey(configKeepAlive)){
                        printf("config missing %s in %s\n", configKeepAlive.c_str(), configKey.c_str());
                        return -1;
                }
                if (!configObject->get(configKeepAlive)->isInt()){
                        printf("config %s in %s are not int\n", configKeepAlive.c_str(), configKey.c_str());
                        return -1;                
                }
                int keepAlive = configObject->get(configKeepAlive)->getInt();
                mqtt = new Mqtt(port, ip, clientId, keepAlive);
        }

*/
	PlayBackItem *item = new PlayBackItemFade(&apa102, 
                (uint8_t)0,(uint8_t)255,(uint8_t)0,(uint8_t)1,
                (uint8_t)0,(uint8_t)0,(uint8_t)255,(uint8_t)1,
                30
        );
	playBack.push(item);

	//StartService();

	if (signal(SIGINT, sigHandler) != 0) {
		  printf("\ncan't catch SIGINT\n");
		  return -1;
	}
	
	httpServer->start();

	int wait = 500;

	while (true){
		int waitRequest = check();
                if (waitRequest < 0 || waitRequest > wait) {
		        sleep_milliseconds(wait);
                } else {
                        sleep_milliseconds(waitRequest);
                }
	}

	printf("Hello: Adalsteinn\n\n");
	return 0;
}
