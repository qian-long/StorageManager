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

// Populates boxToTileID and tileIDToBox
// Modifies boxToTileID, tileIDToBox
// Uses indexfile, nDim
void Indexerp::parseIndexFile() {
  ifstream infile(this->indexfile);
  string line;
  if (infile.is_open()) {
    while (getline(infile, line)) {
      BoundingBox * box = new BoundingBox(line, nDim);
      cout << "box->tileid: " << box->tileid << endl;
    }
  }
}
