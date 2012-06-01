INO=/usr/local/bin/ino
MODEL=atmega328
#MODEL=mega
SERIAL_PORT=/dev/ttyUSB0
SERIAL_RATE=9600


default: build

build: .build/firmware.elf

.build/firmware.elf: src/*.ino
	$(INO) build -m $(MODEL)

upload: build
	$(INO) upload -m $(MODEL) -p $(SERIAL_PORT)

serial:
	@echo -e "\nYOU CAN EXIT WITH ctrl+a ctrl-x\n"
	$(INO) serial -p $(SERIAL_PORT) -b $(SERIAL_RATE)

clean:
	rm -rf .build