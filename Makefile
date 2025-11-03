CFLAGS = -Wall -g
BINDIR=$(HOME)/bin
OBJ=biglcommon.o biglclient.o  biglserver.o

all     : $(OBJ)

biglman : biglcommon.o biglserver.o biglman.c
	gcc $(CFLAGS) biglcommon.o biglserver.o biglman.c -o biglman
	cp biglman      $(BINDIR)

script :
	cp scripts/bigl*.sh  $(BINDIR)/.

biglcommon.o : biglcommon.c 
	gcc $(CFLAGS) -c biglcommon.c
biglserver.o : biglserver.c 
	gcc $(CFLAGS) -c biglserver.c
biglclient.o : biglclient.c 
	gcc $(CFLAGS) -c biglclient.c


update : $(OBJ) hosts.grp
	date > update
	scripts/installobj.sh hosts.grp
	date >> update

clean      :
	rm -f *~
	rm -f *.o
	rm biglman
