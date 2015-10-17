CC=gcc
CFLAGS=-I.
DEPS = # header file 
OBJ = serverFork.o

serverFork: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
