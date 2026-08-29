CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -Wno-unused-but-set-variable -DHSP_PATHIO_UTF8 -DHSP64
LDLIBS ?= -lstdc++fs

TARGET := test_hsp3pathio
SOURCES := ../test_hsp3pathio.cpp ../../src/hsp3/hsp3pathio.cpp ../../src/hsp3/strbuf.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -I../../src/hsp3 $(SOURCES) -o $@ $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) hsp3pathio-日本語-😀.tmp
