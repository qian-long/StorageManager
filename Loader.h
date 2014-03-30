#ifndef _LOADER_H
#define _LOADER_H

#include <vector>
#include <string>
#include "Cell.h"

using std::vector;
using std::string;
class Cell;

class Loader {

  public:

    // parameters
    string filename;
    int64_t nDim;
    int64_t nAttr;
    vector<int64_t> ranges;
    vector<Cell *> cells;
    int stride; // fixed logical tile size, range of logical coord space in one dimension
    
    // Constructor
    Loader(string filename, int64_t nDim, vector<int64_t> ranges, int64_t nAttr, int stride);    
    
    // Destructor
    ~Loader();

    // reads in input file
    void read();

    // creates a temporary sorted file
    void sort();

    // divides sorted file into tiles and writes to disk
    void tile();

    // TODO: make private
    uint64_t mortonEncode2D(uint64_t x, uint64_t y);
    uint64_t shiftCoord(int64_t coord, int64_t min);
};

#endif
