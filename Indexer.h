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
    vector<string> *tileids;

    // Constructor
    Indexer(int nDim, vector<int64_t> ranges, int nAttr, int stride);

    // Destructor
    ~Indexer();
    
    vector<string> * findTilesByAttribute(int attrIndex);

    // Returns attribute tile given attribute index and tileid
    string getAttrTileById(int attrIndex, string tileid);
    string getCoordTileById(string tileid);

    // Returns all attribute tiles given tileid
    vector<string> * getAllAttrTilesById(string tileid);
    
    // Returns tile id given coordinates
    string getTileIdByCoords(vector<int64_t> * coords);

    // Returns all attribute tiles given coordinates
    vector<string> * getAllAttrTilesByCoords(vector<int64_t> * coords);
  private:
    vector<string> * generateTileIds();
    
    bool fileExists(string filename);
};

#endif
