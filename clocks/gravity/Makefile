FONTDIR := fonts
FONTS := $(shell find $(FONTDIR)/src -name '*.png')
HFONTS := $(subst src/,,$(patsubst %.png,%.h,$(FONTS)))

.PHONY: all clean
.SECONDARY: $(FONTS)

all: $(HFONTS)

$(FONTDIR)/%.h: $(FONTDIR)/src/%.png png2points/png2points.py
	@mkdir -p "$(@D)"
	python3 png2points/png2points.py $< > $@

clean:
	@rm -fv $(HFONTS)
