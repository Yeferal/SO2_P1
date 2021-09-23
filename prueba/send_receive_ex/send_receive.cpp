/******************************************************************************/
/* send_receive.c   Dr. Juan Gonzalez Gomez. January 2009                     */
/*----------------------------------------------------------------------------*/
/* Example of Serial communications in Linux.                                 */
/* Sending and receiving data strings                                         */
/*----------------------------------------------------------------------------*/
/* GPL LICENSE                                                                */
/*----------------------------------------------------------------------------*/
/* This example sends a ASCII string to the serial port. It waits for the     */
/* same string to be echoed by another device (For example a microcontroller  */
/* running an echo-firmware or a wire joining the PC tx and rx pins           */
/* The received string is print on the screen. If no data is received         */
/* during the TIMEOUT time, a timeout message is printed                      */
/*                                                                            */
/* The serial port speed is configured to 9600 baud                           */
/*----------------------------------------------------------------------------*/
/* Example of use:                                                            */
/*                                                                            */
/*   ./send_receive /dev/ttyUSB0                                              */
/*                                                                            */
/*  The serial device name should be passed as a parameter                    */
/*  When executing this example, if the echoed data is received the           */
/*  output will be the following:                                             */
/*                                                                            */
/*    String sent------> ASCII Command test                                   */
/*    String received--> ASCII Command test (18 bytes)                        */
/*                                                                            */
/*  If no data is received, the output will be:                               */
/*    String sent------> ASCII Command test                                   */
/*    String received--> Timeout!                                             */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/*  In linux, the serial devices names are:                                   */
/*                                                                            */
/*    /dev/ttyS0  --> First native serial port                                */
/*    /dev/ttyS1  --> Second native serial port                               */
/*    ...                                                                     */
/*    /dev/ttyUSB0  --> First USB-RS232 converter                             */
/*    /dev/ttyUSB1  --> Second USB-RS232 converter                            */
/*    ...                                                                     */
/******************************************************************************/

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


/*--------------------------*/
/* FUNCTION PROTOTYPES      */
/*--------------------------*/

//int  serial_open(char *serial_name, speed_t baud);
//void serial_send(int serial_fd, char *data, int size);
//int  serial_read(int serial_fd, char *data, int size, int timeout_usec);
//void serial_close(int fd);


/******************************************************************************/
/* Open the serial port                                                       */
/*----------------------------------------------------------------------------*/
/* INPUT:                                                                     */
/*   -serial_name: Serial device name                                         */
/*   -baud: Serial speed. The constants Bxxxx are used, were xxxx  is the     */ 
/*          speed. They are defined in termios.h. For example: B9600, B19200..*/
/*          For more information type "man termios"                           */
/*                                                                            */
/* RETURNS:                                                                   */
/*   -The Serial device descriptor  (-1 if there is an error)                 */
/******************************************************************************/
int serial_open(char *serial_name, speed_t baud)
{
  struct termios newtermios;
  int fd;

  // Open the serial port
  fd = open(serial_name,O_RDWR | O_NOCTTY); 

  // Configure the serial port attributes: 
  //   -- No parity
  //   -- 8 data bits
  //   -- other things...
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

/*****************************************************/
/* Sending a string to the serial port.              */
/*                                                   */
/* INPUT:                                            */
/*   -serial_fd: Serial device descriptor            */
/*   -data:      data string to send                 */
/*   -size:      data string size                    */
/*****************************************************/
void serial_send(int serial_fd, char *data, int size)
{
  write(serial_fd, data, size);
}

/*************************************************************************/
/* Receiving a string from the serial port                               */
/*-----------------------------------------------------------------------*/
/* INPUT                                                                 */
/*   -serial_fd: Serial device descriptor                                */
/*   -size: Maximum data size to receive                                 */ 
/*   -timeout_usec: Timeout time (in micro-secs) for receiving the data  */
/*                                                                       */
/* OUTPUT:                                                               */
/*   -data: The serial data received within the timeout_usec time        */
/*                                                                       */
/* RETURNS:                                                              */
/*   -The number of bytes received.                                      */
/*   -0 if none received. It means a TIMEOUT!                            */
/*************************************************************************/
int serial_read(int serial_fd, char *data, int size, int timeout_usec)
{
  fd_set fds;
  struct timeval timeout;
  int count=0;
  int ret;
  int n;
  
  //-- Wait for the data. A block of size bytes is expected to arrive
  //-- within the timeout_usec time. This block can be received as 
  //-- smaller blocks.
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


/********************************************************************/
/* Close the serial port                                            */
/*------------------------------------------------------------------*/
/* INPUT: :                                                         */
/*   fd: Serial device descriptor                                   */
/********************************************************************/
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
