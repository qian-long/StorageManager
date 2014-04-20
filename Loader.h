#ifndef _LOADER_H
#define _LOADER_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include "Cell.h"

using std::vector;
using std::string;
using std::map;
using std::set;
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
    uint64_t memLimit; // number of bytes in fixed physical coord tile
    
    // Constructor
    Loader(string filename, int64_t nDim, vector<int64_t> ranges, int64_t nAttr, int stride, uint64_t memLimit);    
    
    // Destructor
    ~Loader();

    // Read input csv file, creates .sorted csv file that is sorted by tileid
    // creates index file containing all non-empty tile ids, used for indexing later
    void load();

    // Divides sorted file into tiles and writes to disk
    void tile();

    // Read input csv file, sort by row major
    void loadp();
    void tilep();
    // Helper function to write buffers to disk
    void writeTileBufsToDisk(map<string, string> * attrBufMap, stringstream * coordBuf, string tileid);

    // write out new file
    // TODO: delete uncompressed tile?
    void compressTile(const char * filename);


    // TODO: make private
    void createIndexFile(set<string> * tileIDs);
    uint64_t mortonEncode2D(uint64_t x, uint64_t y);
    uint64_t shiftCoord(int64_t coord, int64_t min);
    string getTileID(string line);
    string getSortKey(string line);
};

#endif
