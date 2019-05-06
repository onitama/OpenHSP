TARGET = test_mc3008_duplex
CXX = g++
SRCS = ../../src/hsp3/linux/hsp3gr_linux.cpp
OBJS = $(SRCS:%.cpp=%.o)
CXXFLAGS = -DMCP3008TEST -ggdb3 -DHSPLINUX -DHSPRASPBIAN -DHSPDEBUG
LDFLAGS =

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $^

$(TARGET).out: $(TARGET).o $(OBJS)
	$(CXX) $(LDFLAGS) -o $@ $^

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET).o
