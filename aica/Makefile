.SUFFIXES = .s .o .c
ARMCC = arm-elf-gcc
ARMAS = arm-elf-as -marm7
ARMLD = arm-elf-ld

ARMCFLAGS = -mcpu=arm7 -ffreestanding -fomit-frame-pointer -Os

INCS = -I/home/fredde/code/dreamhack/dreamcast/source
LIBS = -lgcc
TARGET = music.elf
OBJS = \
	base.o \
	main.o



all: music.h

clean:
	-rm -f $(TARGET) $(OBJS) *.bin
	rm -f *~
	rm -f music.h
	rm -f data.h

music.h: $(TARGET)
	bin2c music.elf.bin music.h.tmp
	echo 'unsigned char aica_data[] = {' > music.h
	cat music.h.tmp >> music.h
	echo '};' >> music.h
	-rm music.h.tmp

$(TARGET): data.h $(OBJS)
	$(ARMCC) $(ARMCFLAGS) -Wl,-Ttext=0 -nostartfiles -nostdlib -o $(TARGET) $(OBJS) $(LIBS)
	arm-elf-objcopy -O binary $(TARGET) $(TARGET).bin

data.h:
	../tools/xmconv song.xm data.tmp
	bin2c data.tmp data.h.tmp
	echo 'unsigned char data[] = {' > data.h
	cat data.h.tmp >> data.h
	echo '};' >> data.h
	-rm data.h.tmp
#	-rm data.tmp

.c.o:
	$(ARMCC) $(ARMCFLAGS) -c -o $@ $<

.s.o:
	$(ARMAS) -o $@ $<

