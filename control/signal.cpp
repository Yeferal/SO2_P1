
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

#include <iostream>

#include <chrono>

#define REG_CURRENT_TASK _IOW('a','a',int32_t*)
 
#define SIGETX 44
 
static int done = 0;
int flag_check = 0;
 

 //para salir del handler
void ctrl_c_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGINT) {
        printf("\nPresiones ctrl+c\n");
        done = 1;
    }
}

void sig_event_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGETX) {
        flag_check = info->si_int;
        printf ("Valor recibido de la senial del kernel =  %u\n", flag_check);
    }
}
 
int main(){
	clock_t start, end;       //-- para el temporizador
    int fd;
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
 
 
    fd = open("/dev/etx_device", O_RDWR);
    if(fd < 0) {
            return 0;
    }
 
    /* register this task with kernel for signal */
    if (ioctl(fd, REG_CURRENT_TASK,(int32_t*) &numero)) {
        close(fd);
        exit(1);
    }
    printf("Escuchando (((\n");
   
    while(1) {
		
	if(flag_check){
		int value = 0;
		start = clock();
		end = clock();
		int n;
		int x = 1;

		int i;
		void *memoria_compartida_1;
		void *memoria_compartida_2;
		int presionado, sh_memo_id_1, sh_memo_id_2, flag_state=0;


		time_t tiempo_actual;
		struct tm *ts;
		char buffer[80];

		/* Get the current time */
		tiempo_actual = time(NULL);
		/* Format and print the time, "hh:mm:ss" */
		ts = localtime(&tiempo_actual);
		strftime(buffer, sizeof(buffer), "%H:%M:%S", ts);
		sh_memo_id_1=shmget((key_t)2345, 1024, 0666);
		sh_memo_id_2=shmget((key_t)2346, 1024, 0666);
		printf("Key of shared memory is %d\n",sh_memo_id_1);
		if(sh_memo_id_1==-1){
			sh_memo_id_1=shmget((key_t)2345, 1024, 0666|IPC_CREAT);
			flag_state=1;
		}
		if(sh_memo_id_2==-1){
			sh_memo_id_1=shmget((key_t)2346, 1024, 0666|IPC_CREAT);
		}

		if(flag_state){
			memoria_compartida_1=shmat(sh_memo_id_1,NULL,0); //process attached to shared memory segment
			presionado = 1;
			char a[2] ;
			*a= presionado+'0';
			strcpy(memoria_compartida_1,a);
			printf("You wrote : %s\n",(char *)memoria_compartida_1);
			memoria_compartida_2=shmat(sh_memo_id_2,NULL,0); //process attached to shared memory segment
			strcpy(memoria_compartida_2,buffer);
			printf("You wrote : %s\n",(char *)memoria_compartida_2);
		}else{
			memoria_compartida_1=shmat(sh_memo_id_1,NULL,0);
			memoria_compartida_2=shmat(sh_memo_id_2,NULL,0);
			printf("Found: %s\n",(char *)memoria_compartida_1);
			printf("Found: %s\n",(char *)memoria_compartida_2);
			struct tm tm;
			strptime(memoria_compartida_2, "%H:%M:%S", &tm);
			time_t t = mktime(&tm);
			
			struct tm tm2;
			strptime(buffer, "%H:%M:%S", &tm2);
			int sec = tm2.tm_sec - tm.tm_sec;
			printf("Execution time = %d\n", sec);
			
			value = 1;
    while(((float)(end - start))<=240.00000000){

			if(sec >= 3 || sec < 0){
				presionado = 1;
			} else if (sec >= 0.5){
				if(strcmp(memoria_compartida_1, "1") == 0){
					presionado = 2;
				} else if (strcmp(memoria_compartida_1, "2") == 0){
					presionado = 3;
				} else {
					presionado = 1;
				}	
			} else {
				goto end;
			}
			char a[2] ;
			*a= presionado+'0';
			strcpy(memoria_compartida_1,a);
			printf("You wrote : %s\n",(char *)memoria_compartida_1);
			memoria_compartida_2=shmat(sh_memo_id_2,NULL,0); //process attached to shared memory segment
			strcpy(memoria_compartida_2,buffer);
			printf("You wrote : %s\n",(char *)memoria_compartida_2);

		}
		end:
		printf("end");	
		flag_check=!flag_check;
	 }
    }
    printf("Closing Driver\n");
    close(fd);
}
