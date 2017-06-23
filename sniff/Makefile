CC=gcc
CFLAGS=-I.
DEPS = something.h
LDIR = -L.
LIBS = -l:librtlsdr.a -l:libconvenience_static.a -l:libusb-1.0.a -lpthread -lrt

all: sniff

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sniff: sniff.o
	$(CC) -o $@ $^ $(LDIR) $(LIBS)

clean:
	rm -f *.o sniff
