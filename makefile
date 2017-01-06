CC = gcc
CFLAGS = -std=c11 -g -Wall -Wconversion -Werror -Wextra -Wpedantic
LDFLAGS =
objects = main.o lntracker.o dyna.o hashtbl.o lnscanner.o ftrack.o quick_sort.o
executable = lntracker

all: $(executable)

clean:
	$(RM) $(objects) $(executable)

$(executable): $(objects)
	$(CC) $(objects) $(LDFLAGS) -o $(executable)

ftrack.o: ftrack.c ftrack.h dyna.h
hashtbl.o: hashtbl.c hashtbl.h
lnscanner.o: lnscanner.c lnscanner.h
lntracker.o: lntracker.c lntracker.h lnscanner.h hashtbl.h ftrack.h dyna.h
main.o: main.c lntracker.h lnscanner.h
quick_sort.o: quick_sort.c quick_sort.h
dyna.o: dyna.c quick_sort.h dyna.h
