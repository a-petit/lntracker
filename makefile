CC = gcc
CFLAGS = -std=c11 -g -Wall -Wconversion -Werror -Wextra -Wpedantic -DM_SIZE=80
LDFLAGS =
objects = main.o lntracker.o vector.o hashtbl.o lnscan.o ftrack.o
executable = main

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

ftrack.o: ftrack.c ftrack.h vector.h
hashtbl.o: hashtbl.c hashtbl.h
lnscan.o: lnscan.c lnscan.h
lntracker.o: lntracker.c lntracker.h hashtbl.h ftrack.h vector.h lnscan.h
main.o: main.c lntracker.h
vector.o: vector.c vector.h
