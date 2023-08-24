CXX = g++
CPPFLAGS = -Wall -Wextra
CXXFLAGS = -std=c++17

CPPFLAGS += -Irendering -Itty -I. -Irendering/fonts
CXXFLAGS += -Og -g -fsanitize=address
CXXFLAGS += -fopenmp

CXXFLAGS += $(shell pkg-config sdl2 --cflags)
LDLIBS   += $(shell pkg-config sdl2 --libs)
LDLIBS   += -lutil # for forkpty

OBJS = \
	rendering/screen.o \
	tty/terminal.o \
	tty/forkpty.o \
	ctype.o \
	main.o

TARGET = main

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
