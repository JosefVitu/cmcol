CC=g++
CFLAGS=-c -O3 -I/usr/include/libxml2
LDFLAGS=-lxml2 -ltag
SOURCES=main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=cmcol

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -rf *o *~

distclean:clean
	rm -f $(EXECUTABLE)
