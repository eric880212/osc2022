all: kernel8.img

a.o: a.S #From Source Code to Object Files¶
	aarch64-linux-gnu-gcc -g -c a.S

#An ELF file can be loaded and executed by program loaders. Program loaders are usually provided by the operating system in a regular development environment. In bare-metal programming, ELF can be loaded by some bootloaders.
kernel8.elf: a.o #From Object Files to ELF
	aarch64-linux-gnu-ld -T linker.ld -o kernel8.elf a.o

kernel8.img: kernel8.elf #From ELF to Kernel Image (Rpi3’s bootloader can’t load ELF files. Hence, you need to convert the ELF file to a raw binary image.)
	aarch64-linux-gnu-objcopy -O binary kernel8.elf kernel8.img

run:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -d in_asm
debug:
	qemu-system-aarch64 -M raspi3 -kernel kernel8.img -display none -S -s
clean:
	rm *.o kernel8.*
