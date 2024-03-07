BUILD_DIR = ./build
ENTRY_POINT = 0xc0001500
HD60M_PATH = /home/lxc_990131/桌面/bochs/hd60M.img
AS = nasm
CC = gcc-4.4
LD = ld
LIB = -I lib/ -I lib/kernel/ -I lib/user/ -I kernel/ -I device/
ASFLAGS = -f elf -g
CFLAGS = -Wall $(LIB) -c -fno-builtin -W -Wstrict-prototypes -Wmissing-prototypes -m32 -g
LDFLAGS = -Ttext $(ENTRY_POINT) -e main -Map $(BUILD_DIR)/kernel.map -m elf_i386

OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/init.o \
	$(BUILD_DIR)/interrupt.o $(BUILD_DIR)/timer.o $(BUILD_DIR)/kernel.o \
	$(BUILD_DIR)/print.o $(BUILD_DIR)/debug.o $(BUILD_DIR)/string.o $(BUILD_DIR)/bitmap.o \
	$(BUILD_DIR)/memory.o

boot:$(BUILD_DIR)/mbr.o $(BUILD_DIR)/loader.o
$(BUILD_DIR)/mbr.o:boot/mbr.S
	$(AS) -I boot/include/ -o build/mbr.o boot/mbr.S
	
$(BUILD_DIR)/loader.o:boot/loader.S
	$(AS) -I boot/include/ -o build/loader.o boot/loader.S
	
$(BUILD_DIR)/main.o:kernel/main.c lib/kernel/print.h kernel/init.h kernel/debug.h lib/string.h
	$(CC) $(CFLAGS) -o $@ $<	

$(BUILD_DIR)/init.o:kernel/init.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/interrupt.o:kernel/interrupt.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/timer.o:device/timer.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(BUILD_DIR)/debug.o:kernel/debug.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(BUILD_DIR)/string.o:lib/string.c
	$(CC) $(CFLAGS) -o $@ $<
	
$(BUILD_DIR)/bitmap.o:lib/kernel/bitmap.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/memory.o:kernel/memory.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILD_DIR)/kernel.o:kernel/kernel.S 
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/print.o:lib/kernel/print.S
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD_DIR)/kernel.bin:$(OBJS)
	$(LD) $(LDFLAGS) -o $@ $^

.PHONY:mk_dir hd clean build all boot gdb_symbol

gdb_symbol:
	objcopy --only-keep-debug $(BUILD_DIR)/kernel.bin $(BUILD_DIR)/kernel.sym

mk_dir:
	if [ ! -d $(BUILD_DIR) ];then mkdir $(BUILD_DIR);fi 

hd:
	dd if=build/mbr.o of=$(HD60M_PATH) count=1 bs=512 conv=notrunc && \
	dd if=build/loader.o of=$(HD60M_PATH) count=4 bs=512 seek=2 conv=notrunc && \
	dd if=$(BUILD_DIR)/kernel.bin of=$(HD60M_PATH) bs=512 count=200 seek=9 conv=notrunc
	
clean:
	@cd $(BUILD_DIR) && rm -f ./* && echo "remove ./build all done"

build:$(BUILD_DIR)/kernel.bin

all:mk_dir boot build hd gdb_symbol
