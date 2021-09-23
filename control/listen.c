#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<string.h>

int main()
{
int i;
void *memoria_compartida;
char buffer[100];
int shmid;
while(1){
	// el numero del driver, el espacio ocupado
	shmid=shmget((key_t)2345, 1024, 0666);
	if(shmid==-1){
		printf("0\n");
	}else{
		memoria_compartida=shmat(shmid,NULL,0);
		printf("Mensaje %s\n",(char *)memoria_compartida);
	}
  sleep(1);	
}
}
