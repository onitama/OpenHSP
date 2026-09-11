CXX ?= c++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Werror -Wno-unused-but-set-variable -DHSPLINUX -DHSP64
LDLIBS ?= -lstdc++fs

TARGET := test_hspcmp_membuf
SOURCES := ../test_hspcmp_membuf.cpp ../../src/hspcmp/membuf.cpp ../../src/hsp3/hsp3pathio.cpp ../../src/hsp3/strbuf.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -I../../src/hspcmp -I../../src/hsp3 $(SOURCES) -o $@ $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
