
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
 
static int realizado = 0;
int revision = 0;
 
void sig_event_handler(int n, siginfo_t *info, void *unused){
    if (n == SIGETX)
        revision = info->si_int;
}
 
int main(){
    int serial_fd;
    int32_t value, number;
    struct sigaction act;

	//se instala la cabecera de la senial
    sigemptyset(&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESTART);
    act.sa_sigaction = sig_event_handler;
    sigaction(SIGETX, &act, NULL);
 
    //se abre el driver instalado
    serial_fd = open("/dev/etx_device", O_RDWR);
    if(serial_fd < 0) {
        printf("Erro no se encuentra instalado el driver...\n");
        return 0;
    }
 
    //se inicia el registro de la senial del kernel
    /* register this task with kernel for signal */
    if (ioctl(serial_fd, REG_CURRENT_TASK,(int32_t*) &number)) {
        printf("Error en el registro buscado\n");
        close(serial_fd);
        exit(1);
    }
    printf("Escuchando...\n");
   
    while(1) {
	if(revision){
		int i, pulsacion, id_memoria, id_memoria2, flag_state=0;
		time_t now;
		struct tm *flag_now_time;
		char buffer[80];
		void *memoria_1;
		void *memoria_2;
		//se obtiene el tiempo actual, esta es una bandera, se le da el formato
		now = time(NULL);
		flag_now_time = localtime(&now);
		strftime(buffer, sizeof(buffer), "%H:%M:%S", flag_now_time);
		//se lee el 2345 que es la direccion de la memoria
		//1024 el numero de bites de espacio de memoria
		//aparta dos porque en uno se guarda la bandera del time 1
		//y el otro se guarda la bandera el time 2
		//en las id_memoria se guarda la clave del segmento de la memoria
		id_memoria=shmget((key_t)2345, 1024, 0666);
		id_memoria2=shmget((key_t)2346, 1024, 0666);
		if(id_memoria2==-1)
			id_memoria=shmget((key_t)2346, 1024, 0666|IPC_CREAT);
		
		if(id_memoria==-1){
			id_memoria=shmget((key_t)2345, 1024, 0666|IPC_CREAT);
			flag_state=1;
		}
		printf("====================================\n");
		if(!flag_state){
			struct tm end;
			struct tm start;
			memoria_1=shmat(id_memoria,NULL,0);
			memoria_2=shmat(id_memoria2,NULL,0);
			printf("Inicio: %s\n",(char *)memoria_2);
			strptime(memoria_2, "%H:%M:%S", &start);
			time_t t = mktime(&start);
			strptime(buffer, "%H:%M:%S", &end);
			int sec = end.tm_sec - start.tm_sec;
			printf("Difernecia de Pulsacion = %d\n", sec);
			if(sec >= 4 || sec < 0){
				pulsacion = 1;
			}else if (sec >= 1){
				if(strcmp(memoria_1, "1") == 0){
					pulsacion = 2;
				}else if (strcmp(memoria_1, "2") == 0){
					pulsacion = 3;
				}else {
					pulsacion = 1;
				}	
			} else {
				goto end;
			}
			char a[2] ;
			*a= pulsacion+'0';
			strcpy(memoria_1,a);
			memoria_2=shmat(id_memoria2,NULL,0);
			strcpy(memoria_2,buffer);
			printf("Fin: %s\n",(char *)memoria_2);
		}else{
			//se obtiene el proces para la memoria 1
			memoria_1=shmat(id_memoria,NULL,0);
			pulsacion = 1;
			char a[2] ;
			*a= pulsacion+'0';
			strcpy(memoria_1,a);
			//se obtiene el proces para la memoria 1
			memoria_2=shmat(id_memoria2,NULL,0);
			strcpy(memoria_2,buffer);
			printf("Hora de la pulsacion: %s\n",(char *)memoria_2);
			printf("====================================\n");
		}
		end:
		revision=!revision;
	 }
    }
    close(serial_fd);
}
