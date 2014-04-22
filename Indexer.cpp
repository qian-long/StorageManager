#include <map>
#include <iostream>
#include "Indexer.h"

using namespace std;
Indexer::Indexer(int nDim, vector<int64_t> ranges, int nAttr, string indexfile) {
  this->nDim = nDim;
  this->nAttr = nAttr;
  this->ranges = ranges;
  this->indexfile = indexfile;
  this->suffix = "";

  // Initialize maps
  this->tileids = new vector<string>();
  this->attrToTileMap = new map<int, vector<string>>();
}

// Destructor
Indexer::~Indexer() {
  delete tileids;
  delete attrToTileMap;
}

vector<string> * Indexer::findTilesByAttribute(int attrIndex) {
  return &(*attrToTileMap)[attrIndex];
}

// Returns attribute tile given attribute index and tileid
string Indexer::getAttrTileById(int attrIndex, string tileid) {
  return "tile-attrs[" + to_string(attrIndex) + "]-" + tileid + suffix + ".dat";
};

// Returns RLE attribute tile given attribute index and tileid
string Indexer::getRLEAttrTileById(int attrIndex, string tileid) {
  return "rle-tile-attrs[" + to_string(attrIndex) + "]-" + tileid + suffix + ".dat";
};

// Returns coordinate tile given tile id
string Indexer::getCoordTileById(string tileid) {
  return "tile-coords-" + tileid + suffix + ".dat";
}

// Returns all attribute tiles given tileid
vector<string> * Indexer::getAllAttrTilesById(string tileid) {
  vector<string> * attrTiles = new vector<string>();
  for (int i = 0; i < this->nAttr; ++i) {
    attrTiles->push_back(Indexer::getAttrTileById(i, tileid));
  }
  return attrTiles;

};

// Returns all RLE attribute tiles given tileid
vector<string> * Indexer::getAllRLEAttrTilesById(string tileid) {
  vector<string> * attrTiles = new vector<string>();
  for (int i = 0; i < this->nAttr; ++i) {
    attrTiles->push_back(Indexer::getRLEAttrTileById(i, tileid));
  }
  return attrTiles;
};

// Returns all tile ids tha fall in subranges
// Implemented by Subclasses
vector<string> * Indexer::getTilesByDimSubRange(vector<int64_t> * subranges) {
  return NULL;
};

// Returns all whole tiles ids that fall completely within subranges
// Implemented by subclasses
vector<string> * Indexer::getWholeTilesByDimSubRange(vector<int64_t> * subranges) {
  return NULL;
}

// Returns all tile ids that partially overlap with subranges
// Implemented by subclasses
vector<string> * Indexer::getPartialTilesByDimSubRange(vector<int64_t> * subranges) {
  return NULL;
};


