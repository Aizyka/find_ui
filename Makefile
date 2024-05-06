CXX = gcc
LDLIBS = $(shell pkg-config --libs ncursesw)

all: coursework

coursework: main.c
	$(CXX) -o $@ $< $(LDLIBS)

clean:
	rm -f coursework

