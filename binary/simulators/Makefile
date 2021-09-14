CCOPTS := -Wall -Wextra -DNEOPIXEL=0
CCLINK := -lpthread -lncurses
ifneq ($(DEBUG),)
CCOPTS += -DDEBUG=$(DEBUG)
endif
ifneq ($(TICK_UNIT),)
CCOPTS += -DTICK_UNIT=$(TICK_UNIT)
endif

.PHONY: all lintpy clean

all: binary

binary: binary.cpp arduino.cpp arduino.h clock.h config.h util.h
	g++ $(CCOPTS) -o $@ $< arduino.cpp $(CCLINK)

%.cpp: ../%.ino ino2c.py
	./ino2c.py $< $@

%.h: ../%.h
	ln -s $< $@

lintpy: $(wildcard *.py)
	python -m black .
	python -m isort --profile=black .
	python -m mypy --strict .
	python -m flake8 --ignore=E501,W503 .

clean:
	rm -f binary.cpp binary
