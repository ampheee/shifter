# SHIFTER

Shifter is a command-line program that cyclically shifts bits in a file. It allows you to shift bits in a file by a given number of bits between -8 and 8.

The program was ported to MS-DOS and can be run in a DOSBox. It can also be run on Linux.

## Usage

To use the program in MS-DOS:

```
SHIFTER.EXE FILE.ANY N
```
To use the program in Linux:

```
./shifter.bin FILE.ANY N
```


Where `FILE.ANY` is the name of the file you want to shift and `N` is the number of bits by which you want to shift the file. The `N` must be in the range of -8 to 8.


## Build

To build a program on Linux Ubuntu, you can use the included Makefile for the purpose of building it.

To build a .exe file that can be run on MS-DOS, use the included Makefile to exe.