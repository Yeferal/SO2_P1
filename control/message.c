
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include<sys/shm.h>
#include <time.h>

#define REG_CURRENT_TASK _IOW('a','a',int32_t*)
 
#define SIGETX 44
 
static int flag_realizado = 0;
int flag_check = 0;
 
void ctrl_c_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGINT) {
        printf("\nrecieved ctrl-c\n");
        flag_realizado = 1;
    }
}
 
void sig_event_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGETX) {
        flag_check = info->si_int;
        printf ("Valor de la Senial recibida del kernel =  %u\n", flag_check);
    }
}
 
int main(){
    int serial_fd;
    int32_t valor, numero;
    struct sigaction act;
 
    /* install ctrl-c interrupt handler to cleanup at exit */
    sigemptyset (&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESETHAND);
    act.sa_sigaction = ctrl_c_handler;
    sigaction (SIGINT, &act, NULL);
 
    /* install custom signal handler */
    sigemptyset(&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESTART);
    act.sa_sigaction = sig_event_handler;
    sigaction(SIGETX, &act, NULL);
 
 
    serial_fd = open("/dev/etx_device", O_RDWR);
    if(serial_fd < 0) {
            return 0;
    }
 
    /* register this task with kernel for signal */
    if (ioctl(serial_fd, REG_CURRENT_TASK,(int32_t*) &numero)) {
        close(serial_fd);
        exit(1);
    }
    printf("Escuchando (((...\n");
   
    while(1) {
	if(flag_check){
		int i, presionado, shmid, bool1=0;
		void *shared_memory;
		time_t now;
		struct tm *ts;
		char buf[80];

		/* Get the current time */
		now = time(NULL);
		/* Format and print the time, "hh:mm:ss" */
		ts = localtime(&now);
		strftime(buf, sizeof(buf), "%H:%M:%S", ts);
		shmid=shmget((key_t)2345, 1024, 0666);
		printf("Key of shared memory is %d\n",shmid);
		if(shmid==-1){
			shmid=shmget((key_t)2345, 1024, 0666|IPC_CREAT);
			bool1=1;
		}
		if(bool1){
			shared_memory=shmat(shmid,NULL,0); //process attached to shared memory segment
			presionado = 1;
			char a[2] ;
			*a= presionado+'0';
			strcpy(shared_memory,a);
			printf("You wrote : %s\n",(char *)shared_memory);
		}else{
			shared_memory=shmat(shmid,NULL,0);
			printf("Found: %s\n",(char *)shared_memory);
			struct tm tm;
			strptime(shared_memory, "%H:%M:%S", &tm);
			time_t t = mktime(&tm);
			struct tm tm2;
			strptime(buf, "%H:%M:%S", &tm2);
			int sec = tm2.tm_sec - tm.tm_sec;
			printf("Execution time = %d\n", sec);
			
			if(sec >= 3 || sec < 0){
				presionado = 1;
			} else if (sec >= 0.5){
				if(strcmp(shared_memory, "1") == 0){
					presionado = 2;
				} else if (strcmp(shared_memory, "2") == 0){
					presionado = 3;
				} else {
					presionado = 1;
				}	
			} else {
				goto end;
			}
			char a[2] ;
			*a= presionado+'0';
			strcpy(shared_memory,a);
			printf("You wrote : %s\n",(char *)shared_memory);
		}
		end:
		printf("end");	
		flag_check=!flag_check;
	 }
    }
    printf("Closing Driver\n");
    close(serial_fd);
}
