CC=g++ -std=c++11
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=Loader.cpp Cell.cpp Main.cpp Indexer.cpp Filter.cpp Subarray.cpp Indexerp.cpp BoundingBox.cpp Filterp.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=loader


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

all: $(SOURCES) $(EXECUTABLE)

clean:
	$(RM) -r $(EXECUTABLE) *.o *~ *.dat *.csv output* data/*.tmp* myindex.txt data/*.sorted*

cleandata:
	$(RM) -r *.dat* *.csv myindex.txt output* data/*.tmp data/*.sorted
