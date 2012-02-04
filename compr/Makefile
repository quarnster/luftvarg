#
# KallistiGL test program
# (c)2001 Dan Potter
#   
.SUFFIXES = .s .o .c
TARGET = compr
LIBS = 
CFLAGS = -Wall -ml -m4-single-only -Os -fno-builtin -fomit-frame-pointer
OBJS = \
	startup.o \
	decomprs.o


all: $(TARGET).bin $(OBJS)

clean:
	rm -f $(TARGET).* $(OBJS) *.bin romdisk.*
	rm -f *~

rm-elf:
	-rm -f $(TARGET) romdisk.*

$(TARGET).bin: $(OBJS)
	sh-elf-gcc $(CFLAGS) -fno-strict-aliasing -nostartfiles -nostdlib -Wl -Ttext=0x8c010000 -o $(TARGET).elf \
		$(OBJS) $(LIBS)
	sh-elf-objcopy -O binary $(TARGET).elf $(TARGET).bin

.c.o:
	sh-elf-gcc $(CFLAGS) -c $< -o $@

.s.o:
	sh-elf-as -little $< -o $@

