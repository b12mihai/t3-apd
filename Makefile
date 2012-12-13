COMPILERFLAGS = -w -g
CC = mpicc
CFLAGS = $(COMPILERFLAGS)
LIBS = -lm
OBJECTS = main.o complex.o util.o
SERIAL = serial.o complex.o util.o
TARGET = tema3

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $(OBJECTS) $(LIBS)

serial: $(SERIAL)
	$(CC) $(CFLAGS) -o $@ $(SERIAL) $(LIBS)

clean:
	rm -f *.o $(TARGET) a.out *.pgm serial

archive:
	zip 331CA_BarbulescuMihai_Tema3APD.zip *.c *.h *.sh Makefile README	test/*
