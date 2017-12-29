#include <signal.h>
#include <sstream>
#include "fastgpioomega2.h"
#include "apa102.h"
#include "httpServer.h"
#include "urls.h"
#include "playBack.h"
#include "playBackItemSolid.h"
#include "json.h"

#define ledcount  144*3
APA102 apa102((uint8_t)0, (uint8_t)1, (uint16_t) ledcount);
PlayBack playBack(&apa102);

Urls urls(&apa102, &playBack);


HttpServer* httpServer;
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

void check() {
	if (lastSignal != -1){
		lastSignal = -1;
		printf( "\nShuting down...\n");
		printf( " * Close http\n");
		httpServer->closeAll();               
		printf( " * turn off led\n");				
		playBack.turnOff();
		printf( "exit\n");
                delete httpServer;
		exit(0);
	}

	httpServer->check();
	playBack.update();
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

std::string httpConfigKey = std::string("httpServer");
std::string httpConfigPort = std::string("port");

int main(int argc, char* argv[])
{	
        json::Object config;
        loadConfig(config);
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


	PlayBackItemSolid *solid = new PlayBackItemSolid(&apa102, (uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)1);
	playBack.push(solid);

	//StartService();

	if (signal(SIGINT, sigHandler) != 0) {
		  printf("\ncan't catch SIGINT\n");
		  return -1;
	}
	
	httpServer->start();

	int wait = 500;

	while (true){
		check();
		sleep_milliseconds(wait);
	}

	printf("Hello: Adalsteinn\n\n");
	return 0;
}
