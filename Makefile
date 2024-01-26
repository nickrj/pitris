ARMGNU ?= aarch64-linux-gnu

COPS = -Wall -O1 -std=c99 -nostdlib -nostartfiles -ffreestanding -Iinclude -mgeneral-regs-only -mcpu=cortex-a72+nosimd
ASMOPS = -Iinclude 

BUILD_DIR = build
SRC_DIR = src

all : tetris.img

clean :
	rm -rf $(BUILD_DIR) *.img 

$(BUILD_DIR)/%_c.o: $(SRC_DIR)/%.c
	mkdir -p $(@D)
	$(ARMGNU)-gcc -MMD $(COPS) -c $< -o $@

$(BUILD_DIR)/%_s.o: $(SRC_DIR)/%.S
	$(ARMGNU)-gcc -MMD $(ASMOPS) -c $< -o $@

C_FILES = $(wildcard $(SRC_DIR)/*.c)
ASM_FILES = $(wildcard $(SRC_DIR)/*.S)
OBJ_FILES = $(C_FILES:$(SRC_DIR)/%.c=$(BUILD_DIR)/%_c.o)
OBJ_FILES += $(ASM_FILES:$(SRC_DIR)/%.S=$(BUILD_DIR)/%_s.o)

DEP_FILES = $(OBJ_FILES:%.o=%.d)
-include $(DEP_FILES)

kernel8.img: $(SRC_DIR)/linker.ld $(OBJ_FILES)
	$(ARMGNU)-ld -m aarch64elf -nostdlib -T $(SRC_DIR)/linker.ld -o $(BUILD_DIR)/kernel8.elf  $(OBJ_FILES)
	$(ARMGNU)-objcopy $(BUILD_DIR)/kernel8.elf -O binary kernel8.img

tetris.img: kernel8.img
	dd bs=1M count=33 if=/dev/zero of=tetris.img
	mformat -F -i tetris.img ::
	mcopy -i tetris.img kernel8.img start4.elf bcm2711-rpi-4-b.dtb config.txt ::
