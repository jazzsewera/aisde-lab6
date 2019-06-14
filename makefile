CC := g++

CC_FLAGS := -std=c++11 -O3 -Wall -pedantic

SOURCES := ./Sources
OBJECTS := ./Objects

all: optim graphgen

$(OBJECTS)/%.o: $(SOURCES)/%.cpp
	$(CC) $(CC_FLAGS) -I$(SOURCES) -c -o $@ $<

optim: $(OBJECTS)/Graph.o \
       $(OBJECTS)/Config.o \
	    $(OBJECTS)/Optim.o
	$(CC) $(CC_FLAGS) -o $@ $(OBJECTS)/*.o

graphgen: $(SOURCES)/Graphgen.cpp
	$(CC) $(CC_FLAGS) -o $@ $<

clean:
	-rm -f graph graphgen optim config core $(OBJECTS)/*
