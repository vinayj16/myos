# Compiler and tools
CC = gcc
LD = ld
AS = nasm

# Core compiler flags
CFLAGS = -ffreestanding -O2 -Wall -Wextra -m64 -nostdlib -fno-pic \
         -mno-sse -mno-sse2 -mno-mmx -mno-80387 -mno-red-zone \
         -Iinclude -Ikernel/mm -Ikernel/libc -std=gnu11 -fno-stack-protector \
         -fno-omit-frame-pointer -mcmodel=kernel

# Linker flags
LDFLAGS = -nostdlib -z max-page-size=0x1000 -static -Bsymbolic --no-undefined --entry=_start

# Source files
KERNEL_SRCS = $(filter-out kernel/ps2.c, $(wildcard kernel/*.c))
DRIVER_SRCS = $(wildcard kernel/drivers/*.c)
AI_SRCS = $(wildcard kernel/ai/**/*.c)
LIBC_SRCS = $(wildcard kernel/libc/*.c)
MM_SRCS = $(wildcard kernel/mm/*.c)

# Object files
KERNEL_OBJS = $(KERNEL_SRCS:.c=.o)
DRIVER_OBJS = $(DRIVER_SRCS:.c=.o)
AI_OBJS = $(AI_SRCS:.c=.o)
LIBC_OBJS = $(LIBC_SRCS:.c=.o)
MM_OBJS = $(MM_SRCS:.c=.o)
ASM_OBJS = kernel/arch/x86_64/boot_working.o

# Final object list
OBJ = $(ASM_OBJS) $(KERNEL_OBJS) $(DRIVER_OBJS) $(AI_OBJS) $(LIBC_OBJS) $(MM_OBJS)

# Build targets
.PHONY: all clean run debug

all: myos.iso

debug: CFLAGS += -DDEBUG -g

debug: myos.iso

# Pattern rules for compiling
%.o: %.c
	@echo "  CC      $@"
	@$(CC) $(CFLAGS) -c $< -o $@

%.o: %.s
	@echo "  AS      $@"
	@$(AS) -f elf64 $< -o $@

# Final kernel binary
kernel.bin: $(OBJ) linker_simple.ld
	@echo "  LD      $@"
	@$(LD) -T linker_simple.ld -o $@ $(OBJ) $(LDFLAGS)
	@echo "  OBJCOPY $@"
	@objcopy -O binary kernel.bin kernel.bin.tmp
	@mv kernel.bin.tmp kernel.bin

# ISO generation
myos.iso: kernel.bin boot/grub.cfg
	@echo "  GENISO  $@"
	@mkdir -p iso/boot/grub
	@cp kernel.bin iso/boot/
	@cp boot/grub.cfg iso/boot/grub/
	@grub-mkrescue -o myos.iso iso 2> /dev/null
	@rm -rf iso

# ISO target alias
iso: myos.iso

# Run in QEMU
run: myos.iso
	qemu-system-x86_64 -cdrom myos.iso -m 2G -smp 2 -cpu qemu64

# Debug with QEMU and GDB
debug-run: myos.iso
	qemu-system-x86_64 -cdrom myos.iso -m 2G -s -S &\
	sleep 1 && gdb -ex "target remote localhost:1234" -ex "symbol-file kernel.bin"

# Cleanup
clean:
	@echo "  CLEAN"
	@rm -rf iso *.o *.bin $(OBJ) kernel.bin myos.iso
	@find . -name '*.o' -exec rm -f {} \;

# Include dependency files
-include $(OBJ:.o=.d)

# Generate dependency files
%.d: %.c
	@$(CC) $(CFLAGS) -MM -MT "$@" -MT "$*.o" -o $@ $<

kernel/font.o: kernel/font.c kernel/font.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/text.o: kernel/text.c kernel/font.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/mouse.o: kernel/mouse.c kernel/mouse.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/keyboard.o: kernel/keyboard.c kernel/keyboard.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/framebuffer.o: kernel/framebuffer.c kernel/framebuffer.h kernel/graphics.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/window.o: kernel/window.c kernel/window.h

# Memory management objects
kernel/mm/mm.o: kernel/mm/mm.c kernel/mm/mm.h
	$(CC) $(CFLAGS) -c $< -o $@

# Process management objects
kernel/process.o: kernel/process.c kernel/process.h
	$(CC) $(CFLAGS) -c $< -o $@

# Filesystem objects
kernel/fs.o: kernel/fs.c kernel/fs.h
	$(CC) $(CFLAGS) -c $< -o $@

kernel/desktop.o: kernel/desktop.c kernel/desktop.h
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: run clean all