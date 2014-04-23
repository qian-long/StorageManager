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

    int64_t stride; // size of square
    string outdir; // where output of query will end up

    // Constructor
    Subarray(string name, Indexer * indexer, vector<int64_t> * subranges, vector<int64_t> * ranges, int64_t stride);

    // Destructor
    ~Subarray();

    // Writes a new array in the corresponding subranges
    // TODO: retile?
    void execute();
  private:
    bool inRange(vector<int64_t> * coords);
    void subarrayAttr(string tileid, vector<uint64_t> * cellNums, int attrIndex);
};

#endif
