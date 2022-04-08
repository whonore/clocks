CC := g++
CCOPTS := -Wall -Wextra -Werror -Os
CCINC := -Iarduino
CCLINK := -lpthread -lncurses
ifneq ($(DEBUG),)
CCDEF := -DDEBUG=$(DEBUG)
endif

CPPCHECK := cppcheck
CPPCHECKOPTS := \
	--std=c++14 \
	--language=c++ \
	--enable=warning,style,performance,portability \
	--error-exitcode=1 \
	--inline-suppr
CPPCHECKINC := --include=arduino/arduino.h $(CCINC)

FORMAT := clang-format
FORMATOPTS := --dry-run -Werror

-include Makefile.inc

ARDUINO := $(wildcard arduino/*.[ch] arduino/*.cpp)
CLOCK := $(patsubst ../%.ino,%,$(wildcard ../*.ino))
HFILES := $(wildcard ../*.h)

.PHONY: all check clean

all: $(CLOCK)

$(CLOCK): $(CLOCK).cpp $(ARDUINO) $(HFILES)
	$(CC) $(CCOPTS) $(CCINC) $(CCDEF) -o $@ $< arduino/arduino.cpp $(CCLINK)

%.cpp: ../%.ino scripts/ino2c.py scripts/expand-if.sh scripts/remove-unused-decl.py
	scripts/ino2c.py $< $@
	env CPP_INC="$(CCINC)" CPP_OPT_EXTRA="$(CCDEF)" scripts/expand-if.sh $@ $@
	scripts/remove-unused-decl.py $@

check: $(CLOCK).cpp $(HFILES)
	$(CPPCHECK) $(CPPCHECKOPTS) $(CPPCHECKINC) $(CCDEF) $<
	$(FORMAT) $(FORMATOPTS) ../$(CLOCK).ino $(HFILES)

clean:
	rm -f $(CLOCK) $(CLOCK).cpp
