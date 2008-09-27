PREFIX		 = /mnt/hd/e8-gcc/bin/arm-linux-gnueabi
CFLAGS		:= -fno-stack-protector -fno-builtin -nostdinc -fPIC
LDFLAGS		:= -static -nostdlib -fPIC
ASFLAGS		:= -meabi=4
LIBS		:= -lgcc

ELF_TARGET	 = boot.elf
RAW_TARGET	 = boot

CC		 = $(PREFIX)-gcc
AS		 = $(PREFIX)-as
LD 		 = $(PREFIX)-gcc
OBJCOPY		 = $(PREFIX)-objcopy

LD_SCRIPT	:= ld-script
ASM_OBJS	:= init.o
C_OBJS		:= main.o console.o vsprintf.o printf.o font_8x8.o memstr.o font.o gpt.o ipu.o atag.o crc32.o hw_misc.o mu.o dsp.o
OBJS		:= $(ASM_OBJS) $(C_OBJS)

VERSION		 = $(shell hg -q id 2>/dev/null || echo unknown)
CFLAGS		+= $(CFLAGS) -DLDR_VERSION=\"$(VERSION)\"

.PHONY		= all clean
$(ELF_TARGET):	$(OBJS) $(LD_SCRIPT)
	$(LD) -T$(LD_SCRIPT) $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
$(RAW_TARGET):	$(ELF_TARGET)
	$(OBJCOPY) -O binary -R .note -R .comment -S $< $@

clean:
	rm -f $(OBJS) $(ELF_TARGET) $(RAW_TARGET)
all:		$(RAW_TARGET)

.SECONDEXPANSION:
$(ASM_OBJS):	$$(patsubst %.o,%.S,$$@)
	$(AS) $(ASFLAGS) -o $@ $^
$(C_OBJS):	$$(patsubst %.o,%.c,$$@)
	$(CC) $(CFLAGS) -c $^

