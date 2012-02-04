.SUFFIXES = .s .o .c

CC = gcc
CFLAGS =

INCS =
LIBS = -lgcc
TARGET = xmconv
OBJS = \
	main.o \
	xm.o



all: $(TARGET) parse4kxm slz

slz: slz.o
	$(CC) slz.o -o slz

parse4kxm: parse4kxm.o
	$(CC) parse4kxm.o -o parse4kxm

clean:
	-rm -f xmconv parse4kxm slz *.o
	rm -f *~

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET)

.c.o:
	$(CC) $(CFLAGS) -c -o $@ $<
