CC=gcc
DEBUG= -g
CFLAGS= -c -Wall $(DEBUG)
LFLAGS= -Wall $(DEBUG)
SOURCE= mirrorServer.c mirrorInitiator.c contentServer.c
HEADER= mirrorServer.h
OBJ= mirrorServer.o mirrorInitiator.o contentServer.o
OUT= mirrorServer mirrorInitiator contentServer

all= mirrorServer mirrorInitiator contentServer

mirrorServer: mirrorServer.o
	$(CC) $(LFLAGS) mirrorServer.o -o mirrorServer -pthread

mirrorServer.o: mirrorServer.c mirrorServer.h
	$(CC) $(CFLAGS) mirrorServer.c 

mirrorInitiator: mirrorInitiator.o
	$(CC) $(LFLAGS) mirrorInitiator.o -o mirrorInitiator -pthread

mirrorInitiator.o: mirrorInitiator.c mirrorServer.h
	$(CC) $(CFLAGS) mirrorInitiator.c -pthread

contentServer: contentServer.o 
	$(CC) $(LFLAGS) contentServer.o -o contentServer -pthread

contentServer.o: contentServer.c mirrorServer.h
	$(CC) $(CFLAGS) contentServer.c 

clean:
	rm -f $(OBJ) $(OUT)

count:
	wc $(SOURCE) $(HEADER)

