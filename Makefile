COMPILERFLAGS = -Wall
CC = mpicc
CFLAGS = $(COMPILERFLAGS)
LIBS = -lm
OBJECTS = main.o complex.o util.o
TARGET = tema3

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

.PHONY: clean
clean:
	rm -f *.o $(TARGET) a.out *.pgm serial

.PHONY: archive
archive:
	zip 331CA_BarbulescuMihai_Tema3APD.zip *.c *.h *.sh Makefile README	test/*
