#ifndef _SUBARRAY_H
#define _SUBARRAY_H
#include <string>
#include <vector>
#include "Indexer.h"

using namespace std;
class Subarray {
  public:
    string name;
    Indexer * indexer;
    // xstart, xend, ystart, yend, zstart, zend,...
    // Inclusive
    vector<int64_t> * subranges;
    vector<int64_t> * ranges;

    // size of square
    int64_t stride;

    // Constructor
    Subarray(string name, Indexer * indexer, vector<int64_t> * subranges, vector<int64_t> * ranges, int64_t stride);

    // Destructor
    ~Subarray();

    // Writes a new array in the corresponding subranges
    // TODO: retile?
    void execute();

  private:
    bool inRange(vector<int64_t> * coords);
    vector<string>  getInRangeWholeTiles();
    vector<string>  getInRangePartialTiles();
};

#endif
