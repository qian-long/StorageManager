#ifndef _INDEXERL_H
#define _INDEXERL_H

#include <map>
#include <vector>
#include <string>
#include <set>
#include "Indexer.h"
using namespace std;
// This class is responsible for finding tiles given the query
// Assume fixed logical tiles
class IndexerL: public virtual Indexer {

  public:
    int stride;

    set<string> *tileidset;

    // Constructor
    IndexerL(int nDim, vector<int64_t> ranges, int nAttr, int stride, string indexfile);

    // Destructor
    ~IndexerL();
    
    // Returns all tile ids tha fall in subranges
    vector<string> * getTilesByDimSubRange(vector<int64_t> * subranges);

    // Returns all whole tiles ids that fall completely within subranges
    vector<string> * getWholeTilesByDimSubRange(vector<int64_t> * subranges);

    // Returns all tile ids that partially overlap with subranges
    vector<string> * getPartialTilesByDimSubRange(vector<int64_t> * subranges);

  private:
    // Populates tileids
    vector<string> * generateTileIds();

    // Checks if filename exists
    bool fileExists(string filename);

    vector<string> * combination(map<int, vector<int>> * tileIDMaps);
};

#endif
