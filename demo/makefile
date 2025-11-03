CFLAGS = -Wall -g
CFLAGS = -O2
OBJ =   $(HOME)/$(BIGLROOT)/biglcommon.o $(HOME)/$(BIGLROOT)/biglclient.o
INC = -I$(HOME)/$(BIGLROOT)  
CC = gcc

all     : bigl.exe

bigl.exe : src/client.c
	$(CC)  $(CFLAGS) $(OBJ) $(INC) src/client.c -o bigl.exe


clean      :
	rm -f *~
	rm *.o *.exe
bigclean   :
	rm -f bigloop-*
	rm -f nohup.out
