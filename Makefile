CC=g++ -std=c++11
DFLAGS=-DDEBUG
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=Loader.cpp Cell.cpp Main.cpp Indexer.cpp Indexerp.cpp IndexerL.cpp BoundingBox.cpp Filter.cpp Subarray.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=loader


$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

debug: CC += -DDEBUG -g
debug: $(EXECUTABLE)

all: $(SOURCES) $(EXECUTABLE)


clean:
	$(RM) -r $(EXECUTABLE) *.o *~ *.dat *.csv output* data/*.tmp* myindex.txt myindex-fp.txt data/*.sorted*

cleandata:
	$(RM) -r *.dat* *.csv myindex.txt output* data/*.tmp data/*.sorted
