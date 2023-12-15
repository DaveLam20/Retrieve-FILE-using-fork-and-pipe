CC=gcc
CFLAGS=-Wall -g
OBJ=main.o retrieve_usp.o
EXEC=run

$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC)

main.o: main.c retrieve_usp.h
	$(CC) $(CFLAGS) main.c -c

retrieve_usp.o: retrieve_usp.c retrieve_usp.h
	$(CC) $(CFLAGS) retrieve_usp.c -c

clean:
	rm -f $(OBJ) $(EXEC)
