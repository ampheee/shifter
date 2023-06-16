PHONY:

build:
	rm -rf temp.file test.any
	echo -n "Test!" > test.any
	gcc  -Wall -Wextra -Werror shifter.c
rebuild:

