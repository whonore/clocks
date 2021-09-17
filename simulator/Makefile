CCOPTS := -Wall -Wextra -Iarduino
CCLINK := -lpthread -lncurses
ifneq ($(DEBUG),)
CCOPTS += -DDEBUG=$(DEBUG)
endif

-include Makefile.inc

ARDUINO := $(wildcard arduino/*.[ch] arduino/*.cpp)
CLOCK := $(patsubst ../%.ino,%,$(wildcard ../*.ino))

.PHONY: all clean

all: $(CLOCK)

$(CLOCK): $(CLOCK).cpp $(ARDUINO)
	g++ $(CCOPTS) -o $@ $< arduino/arduino.cpp $(CCLINK)

%.cpp: ../%.ino ino2c.py
	./ino2c.py $< $@

clean:
	rm -f $(CLOCK) $(CLOCK).cpp
