all: gcc -o haptic matrixlib.c matrixadvlib.c kinematic.c solveCoord.c solveFbi.c seriallib.c main.c

gcc -o haptic matrixlib.c matrixadvlib.c kinematic.c solveCoord.c solveFbi.c seriallib.c uart.c main.c

clean:
	rm *.bin *.o *.elf *.dis

	