#ifndef _INDEXER_H
#define _INDEXER_H

#include <map>
#include <vector>
#include <string>

using namespace std;
// This class is responsible for finding tiles given the query
// Assume fixed logical tiles
class Indexer {

  public:
    // Parameters
    int nDim;
    int nAttr;
    vector<int64_t> ranges;
    int stride;

    map<int, vector<string>> attrToTileMap;

    // Constructor
    Indexer(int nDim, vector<int64_t> ranges, int nAttr, int stride);

    // Destructor
    ~Indexer();
    
    // Return tiles for attribute
    vector<string> findTilesByAttribute(int attrIndex);

  private:
    vector<string> *generateTileIds();
};

#endif
