#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/shm.h>
#include<string.h>

int main()
{
int i;
void *shared_memory;
char buff[100];
int shmid;
while(1){
	// el numero del driver, el espacio ocupado
	shmid=shmget((key_t)2345, 1024, 0666);
	if(shmid==-1){
		printf("0\n");
	}else{
		shared_memory=shmat(shmid,NULL,0);
		printf("Mensaje %s\n",(char *)shared_memory);
	}
  sleep(1);	
}
}
