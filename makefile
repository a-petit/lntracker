CC = gcc
CFLAGS = -std=c11 -g -Wall -Wconversion -Werror -Wextra -Wpedantic -DM_SIZE=80
LDFLAGS =
objects = main.o vector.o hashtbl.o lnscan.o ftrack.o
executable = main

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

hashtbl.o: hashtbl.c hashtbl.h
ftrack.o: ftrack.c ftrack.h vector.h
lnscan.o: lnscan.c lnscan.h
main.o: main.c vector.h hashtbl.h lnscan.h ftrack.h
vector.o: vector.c vector.h
