CXX ?= c++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -Werror

TARGET := test_hsp3pathio
SOURCES := ../test_hsp3pathio.cpp ../../src/hsp3/hsp3pathio.cpp

.PHONY: all run clean

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CXX) $(CXXFLAGS) -I../../src/hsp3 $(SOURCES) -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) hsp3pathio-日本語-😀.tmp
