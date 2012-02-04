
.SUFFIXES = .s .o .c
TARGET = intro
CFLAGS = -Wall -ml -m4-single-only -Os -fno-builtin -fomit-frame-pointer
OBJS = \
	startup.o \
	samplegen.o \
	sincos.o \
	main.o



all: data.h

clean:
	-rm -f $(TARGET).* $(OBJS) *.bin romdisk.*
	rm -f *~
	rm -f data.h


data.h: $(TARGET).bin $(OBJS)
	../tools/slz p intro.bin intro.slz
	bin2c intro.slz data.h.tmp
	echo 'unsigned char slz_data[] = {' > data.h
	cat data.h.tmp >> data.h
	echo '};' >> data.h
	-rm data.h.tmp


$(TARGET).bin: $(OBJS)
	sh-elf-gcc $(CFLAGS) -fno-strict-aliasing -nostartfiles -nostdlib -Wl -Ttext=0x0ef00000 -o $(TARGET).elf \
		$(OBJS)
	sh-elf-objcopy -O binary $(TARGET).elf $(TARGET).bin

run: $(TARGET).bin
	dc-tool -t 192.168.0.66 -a 0x0ef00000 -x $(TARGET).bin

.c.o:
	sh-elf-gcc $(CFLAGS) -c $< -o $@

.s.o:
	sh-elf-as -little $< -o $@

