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
 
#define SIGETX 44 //segmentento de la senial del serial TX
 
static int done = 0;
int check = 0;
 
void ctrl_c_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGINT) {
        printf("\nrecieved ctrl-c\n");
        done = 1;
    }
}
 
void sig_event_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGETX) {
        check = info->si_int;
        printf ("Valor recibido de la senial del Kernel =  %u\n", check);
    }
}
 
int main(){
    int fd;
    int32_t valor, numero;
    struct sigaction act;
 
    //Si se recibe una senial de control+c Para iterrumpir la senial
    sigemptyset (&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESETHAND);
    act.sa_sigaction = ctrl_c_handler;
    sigaction (SIGINT, &act, NULL);
 
    //se verifica si esta instalado el handler en el kernel
    sigemptyset(&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESTART);
    act.sa_sigaction = sig_event_handler;
    sigaction(SIGETX, &act, NULL);
 
 	//el /dev/etx_device es donde se va a esuchar
    fd = open("/dev/etx_device", O_RDWR);
    if(fd < 0) {
            return 0;
    }
 
    /* register this task with kernel for signal */
    if (ioctl(fd, REG_CURRENT_TASK,(int32_t*) &numero)) {
        close(fd);
        exit(1);
    }
    printf("Escuchando (((...\n");
   
    while(1) {
	if(check){
	 

	 	printf("*************************************************\n");
	 	printf("Esperando a que se envia una senial\n");
		int i;
		void *memoria_compartida_1, *memoria_compartida_2;
		int presionado, shmid_1,shmid_2;

		int bool1=0;


		time_t tiempo_actual;
		struct tm *ts;
		char buffer[80];

		//obtenemos el tiempo
		tiempo_actual = time(NULL);
		
		ts = localtime(&tiempo_actual);
		strftime(buffer, sizeof(buffer), "%H:%M:%S", ts);
		shmid=shmget((key_t)2345, 1024, 0666);
		shmid2=shmget((key_t)2346, 1024, 0666);
		printf("Key of shared memory is %d\n",shmid);
		if(shmid==-1){
			shmid=shmget((key_t)2345, 1024, 0666|IPC_CREAT);
			bool1=1;
		}
		if(shmid2==-1){
			shmid=shmget((key_t)2346, 1024, 0666|IPC_CREAT);
		}

		if(bool1){
			memoria_compartida_1=shmat(shmid,NULL,0); //procesos adjuntos de la memoria compartida
			presionado = 1;
			char a[2] ;
			*a= presionado+'0';
			strcpy(memoria_compartida_1,a);
			printf("You wrote : %shm_1\n",(char *)memoria_compartida_1);
			memoria_compartida_2=shmat(sbuffer_1,NULL,0); //process attached to shared memory segment
			strcpy(memoria_compartida_2,buffer);mem_1			printf("You wrote : %s\n",(char *)memoria_compartida_2);
		}else{
			memoria_compartida_1=shmat(sshm_1,NULL,0);
			memoria_compartida_2=shmat(shmid2,NULL,0);
			printf("Found: %s\n",(char *)memoria_compartida__1);
			printf("Found: %s\n",(char *)memoria_compartida_2);
			struct tmmemoria__1;
			strptime(memoria_compartida_2, "%H:%M:%S", &tm);
			time_t t = mktime(&tm);
			
			struct tm tm2;
			strptime(buffer, "%H:%M:%S", &tm2);
			int sec = tm2.tm_sec - tm.tm_sec;
			printf("Execution time = %d\n", sec);
			
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
			printf("You wrote : %shm_1\n",(char *)memoria_compartida_1);
			memoria_compartida_2=shmat(sbuffer_1,NULL,0); //process attached to shared memory segment
			strcpy(memoria_compartida_2,buffer);mem_1			printf("You wrote : %s\n",(char *)memoria_compartida_2);

		}
		printf("*************************************************\n");
		end:
		printf("end");	
		check=!check;
	 }
    }
    printf("Closing Driver\n");
    close(fd);
}
