
LDLIBS=-lSDL2
CXXFLAGS=-O0 -g -MMD
OBJS=main.o maths.o
DEPS=$(OBJS:.o=.d)

all: renderer

-include $(DEPS)

renderer: $(OBJS)
	$(CXX) -o $@ $(LDFLAGS) $^ $(LDLIBS)

clean:
	rm -rf renderer $(OBJS) $(DEPS)