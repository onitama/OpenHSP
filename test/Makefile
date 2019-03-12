# Usage: make TARGET=handan rundish

PREFIX ?= .
HSPCMP := $(PREFIX)/../hspcmp
HSP3DISH := $(PREFIX)/../hsp3dish
HSP3CL := $(PREFIX)/../hsp3cl
COMPATH := $(PREFIX)/../common/

.SUFFIXES: .hsp .ax

.hsp.ax:
	$(HSPCMP) -d -i -u --compath=$(COMPATH) $<

rundish: $(TARGET).ax
	$(HSP3DISH) $<

runcl: $(TARGET).ax
	$(HSP3CL) $<
