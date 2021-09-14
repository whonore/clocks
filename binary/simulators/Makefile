CCOPTS := -Wall -Wextra -DNEOPIXEL=0
CCLINK := -lpthread -lncurses
ifneq ($(DEBUG),)
CCOPTS += -DDEBUG=$(DEBUG)
endif
ifneq ($(TICK_UNIT),)
CCOPTS += -DTICK_UNIT=$(TICK_UNIT)
endif

.PHONY: all lintpy clean

all: clock

clock: clock.cpp arduino.cpp arduino.h clock.h config.h util.h
	g++ $(CCOPTS) -o $@ $< arduino.cpp $(CCLINK)

clock.cpp: ../clock.ino ino2c.py
	./ino2c.py ../clock.ino $@

%.h: ../%.h
	ln -s $< $@

lintpy: $(wildcard *.py)
	python -m black .
	python -m isort --profile=black .
	python -m mypy --strict .
	python -m flake8 --ignore=E501,W503 .

clean:
	rm -f clock.cpp clock
