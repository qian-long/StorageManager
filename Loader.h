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
    int64_t nDim;
    int64_t nAttr;
    vector<int64_t> ranges;
    vector<Cell *> cells;
    
    // Constructor
    Loader(int64_t nDim, vector<int64_t> ranges, int64_t nAttr);    
    
    // Destructor
    ~Loader();

    // reads in input file
    void read();

    // creates a temporary sorted file
    void sort();

    // divides sorted file into tiles
    void tile();
    
    // writes tiles to file
    void store(); 

  // TODO: make private
    uint64_t mortonEncode2D(uint64_t x, uint64_t y);
    uint64_t shiftCoord(int64_t coord, int64_t min);
};

#endif
