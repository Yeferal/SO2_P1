/***************************************************************************//**
*  \file       test_app.c
*
*  \details    Userspace application to test the Device driver
*
*  \author     EmbeTronicX
*
* *******************************************************************************/
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
 
static int done = 0;
int check = 0;
 
void ctrl_c_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGINT) {
        done = 1;
    }
}
 
void sig_event_handler(int n, siginfo_t *info, void *unused)
{
    if (n == SIGETX) {
        check = info->si_int;
    }
}
 
int main(){
    int fd;
    int32_t value, number;
    struct sigaction act;

	//se instala la cabecera de la senial
    sigemptyset(&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESTART);
    act.sa_sigaction = sig_event_handler;
    sigaction(SIGETX, &act, NULL);
 
    //se abre el driver instalado
    fd = open("/dev/etx_device", O_RDWR);
    if(fd < 0) {
            printf("Erro no se encuentra instalado el driver...\n");
            return 0;
    }
 
    //se inicia el registro de la senial del kernel
    /* register this task with kernel for signal */
    if (ioctl(fd, REG_CURRENT_TASK,(int32_t*) &number)) {
        printf("Error en el registro buscado\n");
        close(fd);
        exit(1);
    }
    printf("Escuchando...\n");
   
    while(1) {
	if(check){
		
		void *shared_memory;
		void *shared_memory2;
		int i, presionado, shmid, shmid2, bool1=0;
		time_t now;
		struct tm *ts;
		char buf[80];

		//se obtiene el tiempo actual, esta es una bandera, se le da el formato
		now = time(NULL);
		ts = localtime(&now);
		strftime(buf, sizeof(buf), "%H:%M:%S", ts);
		
		//se lee el 2345 que es la direccion de la memoria
		//1024 el numero de bites de espacio de memoria
		//aparta dos porque en uno se guarda la bandera del time 1
		//y el otro se guarda la bandera el time 2
		//en las shmid se guarda la clave del segmento de la memoria
		shmid=shmget((key_t)2345, 1024, 0666);
		shmid2=shmget((key_t)2346, 1024, 0666);
		if(shmid2==-1)
			shmid=shmget((key_t)2346, 1024, 0666|IPC_CREAT);
		
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
			shared_memory2=shmat(shmid2,NULL,0); //process attached to shared memory segment
			strcpy(shared_memory2,buf);
			printf("You wrote : %s\n",(char *)shared_memory2);
		}else{
			shared_memory=shmat(shmid,NULL,0);
			shared_memory2=shmat(shmid2,NULL,0);
			printf("Found: %s\n",(char *)shared_memory2);
			struct tm tm;
			strptime(shared_memory2, "%H:%M:%S", &tm);
			time_t t = mktime(&tm);
			
			struct tm tm2;
			strptime(buf, "%H:%M:%S", &tm2);
			int sec = tm2.tm_sec - tm.tm_sec;
			printf("Difernecia de Pulsacion = %d\n", sec);
			
			if(sec >= 4 || sec < 0){
				presionado = 1;
			} else if (sec >= 1){
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
			shared_memory2=shmat(shmid2,NULL,0); //process attached to shared memory segment
			strcpy(shared_memory2,buf);
			printf("Hora de la pulsacion : %s\n",(char *)shared_memory2);

		}
		end:
		printf("end");	
		check=!check;
	 }
    }
    printf("Closing Driver\n");
    close(fd);
}
