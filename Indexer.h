#ifndef _INDEXER_BASE_H
#define _INDEXER_BASE_H

#include <vector>
#include <map>
#include <string>

using namespace std;

class Indexer {
  public:
    // TODO make private
    int nDim;
    int nAttr;
    vector<int64_t> ranges;
    string indexfile;
    vector<string> * tileids;
    map<int, vector<string>> * attrToTileMap;

    // Constructor
    Indexer(int nDim, vector<int64_t> ranges, int nAttr, string indexfile);

    // Destructor
    ~Indexer();

    vector<string> * findTilesByAttribute(int attrIndex);
    // Returns attribute tile given attribute index and tileid
    virtual string getAttrTileById(int attrIndex, string tileid);

    // Returns RLE attribute tile given attribute index and tileid
    virtual string getRLEAttrTileById(int attrIndex, string tileid);

    // Returns coordinate tile given tile id
    virtual string getCoordTileById(string tileid);

    // Returns all attribute tiles given tileid
    virtual vector<string> * getAllAttrTilesById(string tileid);

    // Returns all RLE attribute tiles given tileid
    virtual vector<string> * getAllRLEAttrTilesById(string tileid);

    // Returns all tile ids tha fall in subranges
    virtual vector<string> * getTilesByDimSubRange(vector<int64_t> * subranges);

    // Returns all whole tiles ids that fall completely within subranges
    virtual vector<string> * getWholeTilesByDimSubRange(vector<int64_t> * subranges);

    // Returns all tile ids that partially overlap with subranges
    virtual vector<string> * getPartialTilesByDimSubRange(vector<int64_t> * subranges);

};
#endif
