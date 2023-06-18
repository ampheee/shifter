PHONY:

build:
	rm -rf temp.file test.any
	# echo -n "AnotherPartOfText.Trying this" > test.any
	gcc  -Wall -Wextra -Werror shifter.c -o shifter.bin
rebuild:

exe:
	~/djgpp/bin/i586-pc-msdosdjgpp-gcc-12.2.0 -march=i586 -m32 -o shifter.exe shifter.c



