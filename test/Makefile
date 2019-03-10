# Usage: make TARGET=handan rundish

HSPCMP := ../hspcmp
HSP3DISH := ../hsp3dish
HSP3CL := ../hsp3CL
COMPATH := ../common/

.SUFFIXES: .hsp .ax

.hsp.ax:
	$(HSPCMP) -d -i -u --compath=$(COMPATH) $<

rundish: $(TARGET).ax
	$(HSP3DISH) $<

runcl: $(TARGET).ax
	$(HSP3CL) $<
