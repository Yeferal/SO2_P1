
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

#include <chrono>

#include <fcntl.h>
#include <sys/select.h>

// #define SERIAL_H
#include <termios.h>

int serial_open(char *serial_name, speed_t baud)
{
  struct termios newtermios;
  int fd;

  fd = open(serial_name,O_RDWR | O_NOCTTY); 

  newtermios.c_cflag= CBAUD | CS8 | CLOCAL | CREAD;
  newtermios.c_iflag=IGNPAR;
  newtermios.c_oflag=0;
  newtermios.c_lflag=0;
  newtermios.c_cc[VMIN]=1;
  newtermios.c_cc[VTIME]=0;

  // Set the speed
  cfsetospeed(&newtermios,baud);
  cfsetispeed(&newtermios,baud);
  
  // flush the input buffer
  if (tcflush(fd,TCIFLUSH)==-1) {
    return -1;
  }

  // flush the output buffer
  if (tcflush(fd,TCOFLUSH)==-1) {
    return -1;
  }

  //-- Configure the serial port now!!
  if (tcsetattr(fd,TCSANOW,&newtermios)==-1) {
    return -1;
  }  

  //-- Return the file descriptor
  return fd;
}

void serial_send(int serial_fd, char *data, int size)
{
  write(serial_fd, data, size);
}

int serial_read(int serial_fd, char *data, int size, int timeout_usec)
{
  fd_set fds;
  struct timeval timeout;
  int count=0;
  int ret;
  int n;

  do {
      //-- Set the fds variable to wait for the serial descriptor
      FD_ZERO(&fds);
      FD_SET (serial_fd, &fds);
    
      //-- Set the timeout in usec.
      timeout.tv_sec = 0;  
      timeout.tv_usec = timeout_usec;

      //-- Wait for the data
      ret=select (FD_SETSIZE,&fds, NULL, NULL,&timeout);
    
    //-- If there are data waiting: read it
      if (ret==1) {
        
        //-- Read the data (n bytes)
        n=read (serial_fd, &data[count], size-count); 
        
        //-- The number of bytes receives is increased in n
        count+=n;
        
        //-- The last byte is always a 0 (for printing the string data)
        data[count]=0;
      }

    //-- Repeat the loop until a data block of size bytes is received or
    //-- a timeout occurs
  } while (count<size && ret==1);

  //-- Return the number of bytes reads. 0 If a timeout has occurred.
  return count;
}

void serial_close(int fd)
{
  close(fd);
}


//------------------
//-- CONSTANTS
//------------------

//-- ASCII string to send through the serial port
#define CMD       "a"

//-- ASCII String length
#define CMD_LEN   1

//--TIMEOUT in micro-sec (It is set to 1 sec in this example)
#define TIMEOUT 100000




/**********************/
/*  MAIN PROGRAM      */
/**********************/

int main (int argc, char* argv[])
{
  int serial_fd;           //-- Serial port descriptor
  char data[CMD_LEN+1];    //-- The received command
  clock_t start, end;       //-- para el temporizador
  
  //-- Check if the serial device name is given
  if (argc<2) {
    printf ("No serial device name is given\n");
    exit(0);
  }

  //-- Open the serial port
  //-- The speed is configure at 9600 baud
  serial_fd=serial_open(argv[1],B9600);
  
  //-- Error checking
  if (serial_fd==-1) {
    printf ("Error opening the serial device: %s\n",argv[1]);
    perror("OPEN");
    exit(0);
  }

 while(1==1){
  int value = 0;
  start = clock();
  end = clock();
  int n;
  n=serial_read(serial_fd,data,CMD_LEN,TIMEOUT);

  int x = 1;
  //printf ("Diferencia: final=%ld, inicion=%ld %f\n",end,start,((float)(end - start)));
  if (n>0) {   
    
    
    value = 1;
    while(((float)(end - start))<=240.00000000){
      //printf ("Diferencia: %f\n",((float)((float)end - (float)start)));
      n=serial_read(serial_fd,data,CMD_LEN,TIMEOUT);
      if (n>0) {
        switch(value){
          case 0:
            value = 1;
            x = 1;
            break;
          case 1:
            value = 2;
            x = 2;
            break;
          case 2:
            value = 3;
            x = 3;
            break;
        }
      }
      end = clock();
    }
    
    printf ("Mensaje %d\n",x);

  }else {
    //printf ("Timeout!\n");
  }
 }
  
  //-- Close the serial port
  serial_close(serial_fd);

  return 0;
}
