
CC = g++
CFLAGS =  -Iserial

#-- Serial library
SERIAL = serial.o

#-- Example: send_receive
NAME1 = send_receive
DEP1 = send_receive_ex/$(NAME1).o 

all: $(NAME1)

#---------------------------
#-- Example: send_receive
#---------------------------
$(NAME1): $(DEP1) 
	$(CC) -o $(NAME1) $(DEP1) 
  
clean::
	  rm -f $(NAME1) $(DEP1) *.bak *~
 
