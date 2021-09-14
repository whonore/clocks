CCOPTS := -Wall -Wextra -DNEOPIXEL=0
CCLINK := -lpthread -lncurses
ifneq ($(DEBUG),)
CCOPTS += -DDEBUG=$(DEBUG)
endif
ifneq ($(TICK_UNIT),)
CCOPTS += -DTICK_UNIT=$(TICK_UNIT)
endif

.PHONY: all lintpy clean

all: binclock

binclock: binclock.cpp arduino.cpp arduino.h
	g++ $(CCOPTS) -o $@ $< arduino.cpp $(CCLINK)

binclock.cpp: ../binclock.ino ino2c.py
	./ino2c.py ../binclock.ino $@

lintpy: $(wildcard *.py)
	python -m black .
	python -m isort --profile=black .
	python -m mypy --strict .
	python -m flake8 --ignore=E501,W503 .

clean:
	rm -f binclock.cpp binclock
