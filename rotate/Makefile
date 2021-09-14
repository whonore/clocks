FONTDIR := fonts
FONTS := $(addprefix $(FONTDIR)/,$(notdir $(patsubst %.png,%.h,$(wildcard $(FONTDIR)/src/*.png))))

.PHONY: all clean
.SECONDARY: $(FONTS)

all: $(FONTS)

$(FONTDIR)/%.h: $(FONTDIR)/src/%.png png2points/png2points.py
	python3 png2points/png2points.py $< > $@

clean:
	rm -f $(FONTS)
