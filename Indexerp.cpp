#include <map>
#include <iostream>
#include <fstream>
#include "Indexerp.h"
using namespace std;

// Constructor
Indexerp::Indexerp(int nDim, vector<int64_t> ranges, int nAttr, string indexfile) {
  this->nDim = nDim;
  this->ranges = ranges;
  this->nAttr = nAttr;
  this->indexfile = indexfile;
  
  this->boxToTileID = new map<BoundingBox *, string>();
  this->tileIDToBox = new map<string, BoundingBox *>();
  this->attrToTileMap = new map<int, vector<string>>();
  // Basic version: indexerp contains a list of bounding boxes mapped to tileids and reverse mapping
  // TODO
  Indexerp::parseIndexFile();  

}

// Destructor
// TODO
Indexerp::~Indexerp() {
  delete boxToTileID;
  delete tileIDToBox;
  delete attrToTileMap;
}

// Populates boxToTileID and tileIDToBox and attrToTileMap
// Modifies boxToTileID, tileIDToBox
// Uses indexfile, nDim
void Indexerp::parseIndexFile() {
  ifstream infile(this->indexfile);
  string line;
  if (infile.is_open()) {
    while (getline(infile, line)) {
      BoundingBox * box = new BoundingBox(line, nDim);
      cout << "box->tileid: " << box->tileid << endl;
      string tileid = box->tileid;
      (*boxToTileID)[box] = tileid;
      (*tileIDToBox)[tileid] = box;
      for (int i = 0; i < nAttr; ++i) {
        string filename = "tile-attrs[" + to_string(i) + "]-" + tileid + "-fp.dat";
        (*attrToTileMap)[i].push_back(filename);

      }
    }
  }
}

vector<string> * Indexerp::findTilesByAttribute(int attrIndex) {
  return &(*attrToTileMap)[attrIndex];
}

// Returns attribute tile given attribute index and tileid
string Indexerp::getAttrTileById(int attrIndex, string tileid) {
  string filename = "tile-attrs[" + to_string(attrIndex) + "]-" + tileid + "-fp.dat";
  return filename;
}

// Returns RLE attribute tile given attribute index and tileid
string Indexerp::getRLEAttrTileById(int attrIndex, string tileid) {

  return "rle-tile-attrs[" + to_string(attrIndex) + "]-" + tileid + "-fp.dat";
}

// Returns coordinate tile given tile id
string Indexerp::getCoordTileById(string tileid) {
  string filename = "tile-coords-" + tileid + "-fp.dat";
  return filename;
}

// Returns all attribute tiles given tileid
vector<string> * Indexerp::getAllAttrTilesById(string tileid) {
  vector<string> * attrTiles = new vector<string>();
  for (int i = 0; i < this->nAttr; ++i) {
    attrTiles->push_back(Indexerp::getAttrTileById(i, tileid));
  }
  return attrTiles;
}

// Returns all RLE attribute tiles given tileid
vector<string> * Indexerp::getAllRLEAttrTilesById(string tileid) {
  vector<string> * attrTiles = new vector<string>();
  for (int i = 0; i < this->nAttr; ++i) {
    attrTiles->push_back(Indexerp::getRLEAttrTileById(i, tileid));
  }
  return attrTiles;

}

/*
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
*/

