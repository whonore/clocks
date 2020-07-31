CCOPTS := -Wall
CCLINK := -lm -lpthread -lncurses
ifneq ($(DEBUG),)
CCOPTS += -DDEBUG=$(DEBUG)
endif

.PHONY: all lintpy clean

all: binclock

binclock: binclock.c arduino.c arduino.h
	gcc $(CCOPTS) -o $@ $< arduino.c $(CCLINK)

binclock.c: ../binclock.ino ino2c.py
	./ino2c.py ../binclock.ino $@

lintpy: $(wildcard *.py)
	python -m black .
	python -m isort --profile=black .
	python -m mypy --strict .
	python -m flake8 --ignore=E501,W503 .

clean:
	rm -f binclock.c binclock
