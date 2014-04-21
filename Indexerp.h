#ifndef _INDEXERP_H
#define _INDEXERP_H

#include <map>
#include <vector>
#include <string>
#include <set>
#include "BoundingBox.h"

using namespace std;
class BoundingBox;
// This class is responsible for finding tiles given the query
// Assume fixed physical tiles
class Indexerp {

  public:
    // Parameters
    int nDim;
    int nAttr;
    vector<int64_t> ranges;
    string indexfile;

    map<BoundingBox *, string> * boxToTileID;
    map<string, BoundingBox *> * tileIDToBox;
    map<int, vector<string>> * attrToTileMap;
    vector<string> *tileids;
    //set<string> *tileidset;

    // Constructor
    Indexerp(int nDim, vector<int64_t> ranges, int nAttr, string indexfile);

    // Destructor
    ~Indexerp();

    vector<string> * findTilesByAttribute(int attrIndex);

    // Returns attribute tile given attribute index and tileid
    string getAttrTileById(int attrIndex, string tileid);

    // Returns RLE attribute tile given attribute index and tileid
    string getRLEAttrTileById(int attrIndex, string tileid);

    // Returns coordinate tile given tile id
    string getCoordTileById(string tileid);

    // Returns all attribute tiles given tileid
    vector<string> * getAllAttrTilesById(string tileid);

    // Returns all RLE attribute tiles given tileid
    vector<string> * getAllRLEAttrTilesById(string tileid);

    // Returns tile id given coordinates
    string getTileIdByCoords(vector<int64_t> * coords);

    // Returns all attribute tiles given coordinates
    vector<string> * getAllAttrTilesByCoords(vector<int64_t> * coords);

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

    // Populates boxToTileID and tileIDToBox
    void parseIndexFile();
};

#endif
