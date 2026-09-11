# Usage: make TARGET=handan rundish

PREFIX ?= .
HSPCMP := $(PREFIX)/../hspcmp
HSP3DISH := $(PREFIX)/../hsp3dish
HSP3CL := $(PREFIX)/../hsp3cl
COMPATH := $(PREFIX)/../common/
PYTHON ?= python3
COMMON ?= $(CURDIR)/../common

.SUFFIXES: .hsp .ax

.hsp.ax:
	$(HSPCMP) -d -i -u --compath=$(COMPATH) $<

rundish: $(TARGET).ax
	$(HSP3DISH) $<

runcl: $(TARGET).ax
	$(HSP3CL) $<

.PHONY: unicode-path

unicode-path:
	@if [ -z "$(HSPCMP_EXE)" ]; then \
		echo "Set HSPCMP_EXE to the hspcmp executable path."; \
		echo "Example: make -C test unicode-path HSPCMP_EXE=../src/hspcmp/Release64/hspcmp.exe"; \
		exit 2; \
	fi
	$(PYTHON) test_unicode_path_handling/run_tests.py --hspcmp "$(HSPCMP_EXE)" --common "$(COMMON)" \
		$(if $(HSPCMP_DLL),--hspcmp-dll "$(HSPCMP_DLL)")
