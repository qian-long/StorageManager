#ifndef _INDEXERP_H
#define _INDEXERP_H

#include <map>
#include <vector>
#include <string>
#include <set>
#include "BoundingBox.h"
#include "Indexer.h"

using namespace std;
class BoundingBox;
// This class is responsible for finding tiles given the query
// Assume fixed physical tiles
class Indexerp: public virtual Indexer {

  public:
    map<BoundingBox *, string> * boxToTileID;
    map<string, BoundingBox *> * tileIDToBox;
    //set<string> *tileidset;

    // Constructor
    Indexerp(int nDim, vector<int64_t> ranges, int nAttr, string arraydir);

    // Destructor
    ~Indexerp();

    // Returns all tile ids tha fall in subranges
    vector<string> * getTilesByDimSubRange(vector<int64_t> * subranges);

    // Returns all whole tiles ids that fall completely within subranges
    vector<string> * getWholeTilesByDimSubRange(vector<int64_t> * subranges);

    // Returns all tile ids that partially overlap with subranges
    vector<string> * getPartialTilesByDimSubRange(vector<int64_t> * subranges);

  private:

    // Populates boxToTileID and tileIDToBox
    void parseIndexFile();
};

#endif
