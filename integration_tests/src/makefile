LDFLAGS += -lCommunicationModule

hex:
	avr-gcc -Os -DF_CPU=16000000 -mmcu=atmega328p -c main.c 
	avr-gcc -DF_CPU=16000000 -mmcu=atmega328p -o main.elf main.o 
	avr-objcopy -O ihex main.elf main.hex
	rm main.o
	rm main.elf

flash:
	avrdude -c arduino -p atmega328p -P /dev/tty.usbmodem1411 -U flash:w:main.hex
