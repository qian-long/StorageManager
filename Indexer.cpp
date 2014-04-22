#include <map>
#include "Indexer.h"

Indexer::Indexer(int nDim, vector<int64_t> ranges, int nAttr, string indexfile) {
  this->nDim = nDim;
  this->nAttr = nAttr;
  this->ranges = ranges;
  this->indexfile = indexfile;

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
  return NULL;
};

// Returns RLE attribute tile given attribute index and tileid
string Indexer::getRLEAttrTileById(int attrIndex, string tileid) {
  return NULL;
};

// Returns coordinate tile given tile id
string Indexer::getCoordTileById(string tileid) {
  return NULL;
}

// Returns all attribute tiles given tileid
vector<string> * Indexer::getAllAttrTilesById(string tileid) {
  return NULL;
};

// Returns all RLE attribute tiles given tileid
vector<string> * Indexer::getAllRLEAttrTilesById(string tileid) {
  return NULL;
};

// Returns all tile ids tha fall in subranges
vector<string> * Indexer::getTilesByDimSubRange(vector<int64_t> * subranges) {
  return NULL;
};

// Returns all whole tiles ids that fall completely within subranges
vector<string> * Indexer::getWholeTilesByDimSubRange(vector<int64_t> * subranges) {
  return NULL;
}

// Returns all tile ids that partially overlap with subranges
vector<string> * Indexer::getPartialTilesByDimSubRange(vector<int64_t> * subranges) {
  return NULL;
};


