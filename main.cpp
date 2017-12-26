#include <signal.h>
#include "fastgpioomega2.h"
#include "apa102.h"
#include "httpServer.h"
#include "urls.h"
#include "playBack.h"
#include "playBackItemSolid.h"


void httpResponse(HttpConnection* httpConnection);

volatile  int lastSignal = -1;
HttpServer httpServer(8080, &httpResponse);
#define ledcount  144*3
APA102 apa102((uint8_t)0, (uint8_t)1, (uint16_t) ledcount);
PlayBack playBack(&apa102);

Urls urls(&apa102, &playBack);

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
		httpServer.closeAll();
		printf( " * turn off led\n");				
		playBack.turnOff();
		printf( "exit\n");
		exit(0);
	}

	httpServer.check();
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

int main(int argc, char* argv[])
{	
	PlayBackItemSolid *solid = new PlayBackItemSolid(&apa102, (uint8_t)0,(uint8_t)1,(uint8_t)0,(uint8_t)1);
	playBack.push(solid);

	//StartService();

	if (signal(SIGINT, sigHandler) != 0) {
		  printf("\ncan't catch SIGINT\n");
		  return -1;
	}
	
	httpServer.start();

	int wait = 500;

	while (true){
		check();
		sleep_milliseconds(wait);
	}

	printf("Hello: Adalsteinn\n\n");
	return 0;
}
