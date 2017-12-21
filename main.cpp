#include <signal.h>
#include <errno.h>
#include <sched.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <stdint.h>
#include "fastgpioomega2.h"
#include "apa102.h"
#include "httpServer.h"
#include "urls.h"




volatile  int lastSignal = -1;
HttpServer httpServer(8080, &HttpResponse);
#define ledcount  144*3
rgb_bright_color color[ledcount];
APA102<(uint8_t)0, (uint8_t)1, (uint16_t) ledcount> apa102;


void sleep_milliseconds(uint32_t millis) {
  struct timespec sleep;
  sleep.tv_sec = millis / 1000;
  sleep.tv_nsec = (millis % 1000) * 1000000L;
  while (clock_nanosleep(CLOCK_MONOTONIC, 0, &sleep, &sleep) && errno == EINTR);
}

void sigHandler(int signo) {
  lastSignal = signo;
}



void Check() {
	if (lastSignal != -1){
		lastSignal = -1;
		printf( "\nShuting down...\n");
		printf( " * Close http\n");
		httpServer.Close();
		printf( " * turn off led\n");				
		apa102.TurnOff();
		printf( "exit\n");
		exit(0);
	}

	httpServer.Check();
}

void StartService(){
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
	//StartService();

	if (signal(SIGINT, sigHandler) != 0) {
		  printf("\ncan't catch SIGINT\n");
		  return -1;
	}
	
	httpServer.Start();

	int wait = 500;

	while (true){
		Check();
	
	
		for (int i=0; i < ledcount; i++) {
			color[i].blue = 20;
			color[i].red =0;
			color[i].green =40;
			color[i].brightness = 1;
		}
		
	//	apa102.write(color);
		Check();
		sleep_milliseconds(wait);

		for (int i=0; i < ledcount; i++) {
			color[i].blue = 0;
			color[i].red =40;
			color[i].green =0;
			color[i].brightness = 1;
		}
		
	//	apa102.write(color);
		Check();
		sleep_milliseconds(wait);
	}

	printf("Hello: Adalsteinn\n\n");
	return 0;
}
