CCOPTS := -Wall -Wextra -Os
CCINC := -Iarduino
CCLINK := -lpthread -lncurses
ifneq ($(DEBUG),)
CCDEF := -DDEBUG=$(DEBUG)
endif

-include Makefile.inc

ARDUINO := $(wildcard arduino/*.[ch] arduino/*.cpp)
CLOCK := $(patsubst ../%.ino,%,$(wildcard ../*.ino))
HFILES := $(wildcard ../*.h)

.PHONY: all clean

all: $(CLOCK)

$(CLOCK): $(CLOCK).cpp $(ARDUINO) $(HFILES)
	g++ $(CCOPTS) $(CCINC) $(CCDEF) -o $@ $< arduino/arduino.cpp $(CCLINK)

%.cpp: ../%.ino scripts/ino2c.py scripts/expand-if.sh scripts/remove-unused-decl.py
	scripts/ino2c.py $< $@
	env CPP_INC="$(CCINC)" CPP_OPT_EXTRA="$(CCDEF)" scripts/expand-if.sh $@ $@
	scripts/remove-unused-decl.py $@

clean:
	rm -f $(CLOCK) $(CLOCK).cpp
