CROSS-COMPILER = arm-none-eabi-
QEMU = ./gnu-mcu-eclipse-qemu/bin/qemu-system-gnuarmeclipse
all: button.bin

button.bin: button.c isr_vectors.s
	$(CROSS-COMPILER)gcc -Wall -mcpu=cortex-m4 -mthumb -nostartfiles -T stm32f4.ld button.c isr_vectors.s -o button.elf
	$(CROSS-COMPILER)objcopy -O binary button.elf button.bin
	$(CROSS-COMPILER)gcc -Wall -mcpu=cortex-m4 -mthumb -nostartfiles -T stm32f4.ld blink.c isr_vectors.s -o blink.elf
	$(CROSS-COMPILER)objcopy -O binary blink.elf blink.bin

qemu:
	@echo
	@echo "Press Ctrl+A and then press X to exit QEMU"
	@echo
	$(QEMU) -M STM32F4-Discovery -nographic -gdb tcp::1234 -S -kernel button.bin

flash:
	sudo st-flash --reset write button.bin 0x8000000

clean:
	rm -f *.o *.elf *.bin


