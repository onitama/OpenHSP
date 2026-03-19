PREFIX = ..
HSPCMP := $(PREFIX)/../hspcmp
HSP3CL := $(PREFIX)/../hsp3cl
COMPATH := $(PREFIX)/../common/

HSP_FILES  := $(wildcard *.hsp)
AX_FILES   := $(HSP_FILES:.hsp=.ax)
OUT_FILES  := $(HSP_FILES:.hsp=.out)

.PHONY: all run expect diff clean

all: $(AX_FILES)

%.ax: %.hsp
	$(HSPCMP) -d -i -u --compath=$(COMPATH) $<

%.out: %.ax
	$(HSP3CL) $< > $@

run: $(OUT_FILES)

expect: $(OUT_FILES)
	for f in $(OUT_FILES); do cp $$f $${f%.out}.expect; done

diff: $(OUT_FILES)
	for f in $(OUT_FILES); do \
		if diff -u $${f%.out}.expect $$f; then echo "OK: $$f"; else echo "FAIL: $$f"; fi; \
	done

clean:
	rm -f *.ax *.out
