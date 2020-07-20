all: binclock

binclock: binclock.c arduino.h
	gcc -Wall binclock.c -o binclock -lm

binclock.c: ../binclock.ino
	python ino2c.py ../binclock.ino binclock.c
