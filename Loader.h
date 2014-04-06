#ifndef _LOADER_H
#define _LOADER_H

#include <vector>
#include <string>
#include <map>
#include <sstream>
#include "Cell.h"

using std::vector;
using std::string;
using std::map;
using std::stringstream;
class Cell;

class Loader {

  public:

    // parameters
    string filename;
    int64_t nDim;
    int64_t nAttr;
    vector<int64_t> ranges;
    int stride; // fixed logical tile size, range of logical coord space in one dimension
    uint64_t mem_limit; // number of bytes used in main memory
    
    // Constructor
    Loader(string filename, int64_t nDim, vector<int64_t> ranges, int64_t nAttr, int stride, uint64_t mem_limit);    
    
    // Destructor
    ~Loader();

    // Read input csv file, creates .sorted csv file that is sorted by tileid
    void load();

    // divides sorted file into tiles and writes to disk
    void tile();

    void writeTileBufsToDisk(map<string, string> * attrBufMap, stringstream * coordBuf, string tileid);
    // TODO: make private
    uint64_t mortonEncode2D(uint64_t x, uint64_t y);
    uint64_t shiftCoord(int64_t coord, int64_t min);
    string getTileID(string line);
    void int64ToChar(char a[], int64_t n);
};

#endif
