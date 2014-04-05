CC=g++ -std=c++11
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=Loader.cpp Cell.cpp Main.cpp Indexer.cpp Filter.cpp Subarray.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=loader


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

all: $(SOURCES) $(EXECUTABLE)

clean:
	$(RM) -r $(EXECUTABLE) *.o *~ *.dat output* data/*.tmp*

cleandata:
	$(RM) *.dat*
