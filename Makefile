CXX = g++
CPPFLAGS = -Wall -Wextra
CXXFLAGS = -std=c++17

CPPFLAGS += -Irendering -Itty -I. -Irendering/fonts
CXXFLAGS += -Og -g -O0 -fsanitize=address
CXXFLAGS += -fopenmp

CXXFLAGS += $(shell pkg-config sdl2 --cflags)
LDLIBS   += $(shell pkg-config sdl2 --libs)
LDLIBS   += -lutil # for forkpty

CPPFLAGS += -MP -MMD -MF$(subst .o,.d,$(addprefix .deps/,$(subst /,_,$@)))

CXXFLAGS += -pg

OBJS = \
	rendering/screen.o \
	rendering/person.o \
	tty/terminal.o \
	tty/forkpty.o \
	ctype.o \
	main.o

-include $(addprefix .deps/,$(subst /,_,$(OBJS:.o:.d)))

TARGET = main.out

$(TARGET): $(OBJS)
	$(CXX) -o $@ $(OBJS) $(CXXFLAGS) $(LDLIBS)

.PHONY: clean
clean:
	rm -f $(OBJS) $(TARGET)
