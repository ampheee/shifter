.PHONY: all	clean	djgpp	build	exe	runDos

all: djgpp

clean:
	rm -rf shifter.bin shifter.exe

djgpp:
	sudo apt-get update
	sudo apt-get install bison flex curl gcc g++ make texinfo zlib1g-dev g++ unzip
	DJGPP_PREFIX=/usr/local/my-djgpp
	git clone https://github.com/andrewwutw/build-djgpp.git
	cd build-djgpp
	sudo ./build-djgpp.sh 12.2.0	

build:
	gcc  -Wall -Wextra -Werror shifter.c -o shifter.bin

rebuild:


exe:
	~/djgpp/bin/i586-pc-msdosdjgpp-gcc-12.2.0 -march=i586 -m32 -o shifter.exe shifter.c

runDos:

