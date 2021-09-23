
CC = g++
CFLAGS =  -Iserial

#-- Serial library
SERIAL = serial.o

#-- Example: send_receive
NAME1 = send_receive
DEP1 = send_receive_ex/$(NAME1).o 


all: $(NAME1)

#------------------------------
#-- serial.o
#------------------------------
$(SERIAL): serial/serial.c serial/serial.h
	    $(CC) -o $(SERIAL) -c serial/serial.c

clean::
	  rm -f $(SERIAL) *.bak *~

#---------------------------
#-- Example: send_receive
#---------------------------
$(NAME1): $(DEP1) 
	$(CC) -o $(NAME1) $(DEP1) 
  
clean::
	  rm -f $(NAME1) $(DEP1) *.bak *~
 
