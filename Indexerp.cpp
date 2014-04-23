#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "Debug.h"
#include "Indexerp.h"
using namespace std;

// Constructor
Indexerp::Indexerp(int nDim, vector<int64_t> ranges, int nAttr, string indexfile): Indexer(nDim, ranges, nAttr, indexfile) {

  this->boxToTileID = new map<BoundingBox *, string>();
  this->tileIDToBox = new map<string, BoundingBox *>();
  this->suffix = "-fp";

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
  ifstream infile("myindex-fp.txt");
  string line;
  if (infile.is_open()) {
    while (getline(infile, line)) {
      BoundingBox * box = new BoundingBox(string(line), nDim);
      string tileid = box->tileid;
      (*boxToTileID)[box] = tileid;
      (*tileIDToBox)[tileid] = box;
      this->tileids->push_back(tileid);
      for (int i = 0; i < nAttr; ++i) {
        //string filename = "tile-attrs[" + to_string(i) + "]-" + tileid + "-fp.dat";
        string filename = Indexer::getAttrTileById(i, tileid);
        (*attrToTileMap)[i].push_back(filename);

      }
    }
  }
  else {
    perror("index file won't open");
  }
}


// Returns all tile ids tha fall in subranges
// subranges = [minX, maxX, minY, maxY, ...]
vector<string> * Indexerp::getTilesByDimSubRange(vector<int64_t> * subranges) {

  vector<string> * output = new vector<string>();

  // iterate through tile ids and see if subrange intersects the bounding boxes
  for (vector<string>::iterator it = tileids->begin(); it != tileids->end(); ++it) {
    bool overlapsTile = true;
    string tileid = *it;
    BoundingBox * box = (*tileIDToBox)[tileid];
    vector<int64_t>::iterator itMin = box->minCoords->begin();
    vector<int64_t>::iterator itMax = box->maxCoords->begin();

    // compare subranges
    for (vector<int64_t>::iterator its = subranges->begin(); its != subranges->end(); its = its + 2) {
      int64_t lowCoord = *(its);
      int64_t hiCoord = *(its + 1);
      int64_t minCoord = *itMin;
      int64_t maxCoord = *itMax;

      if (lowCoord > maxCoord || hiCoord < minCoord) {
        overlapsTile = false;
        break;
      }
      ++itMin;
      ++itMax;
    }

    if (overlapsTile) {
      output->push_back(tileid);
    }
  }

  return output;
}

// Returns all whole tiles ids that fall completely within subranges
vector<string> * Indexerp::getWholeTilesByDimSubRange(vector<int64_t> * subranges) {
  vector<string> * wholeTiles = new vector<string>();

  // iterate through tile ids and see if subrange intersects the bounding boxes
  for (vector<string>::iterator it = tileids->begin(); it != tileids->end(); ++it) {
    string tileid = *it;
    BoundingBox * box = (*tileIDToBox)[tileid];
    dbgmsg("tileid: " + tileid);
    vector<int64_t>::iterator itMin = box->minCoords->begin();
    vector<int64_t>::iterator itMax = box->maxCoords->begin();
    bool wholeTile = true;

    // compare subranges
    for (vector<int64_t>::iterator its = subranges->begin(); its != subranges->end(); its = its + 2) {
      int64_t lowCoord = *(its);
      int64_t hiCoord = *(its + 1);
      int64_t minCoord = *itMin;
      int64_t maxCoord = *itMax;

      dbgmsg("minCoord: " + to_string(minCoord) + " maxCoord: " + to_string(maxCoord) + " lowCoord: " + to_string(lowCoord) + "hiCoord: " + to_string(hiCoord));

      if (!(minCoord >= lowCoord && maxCoord<= hiCoord)) {
        dbgmsg("false");
        wholeTile = false;
      }
      ++itMin;
      ++itMax;
    }

    if (wholeTile) {
      wholeTiles->push_back(tileid);
    }
  }

  return wholeTiles;

}

// Returns all tile ids that partially overlap with subranges
vector<string> * Indexerp::getPartialTilesByDimSubRange(vector<int64_t> * subranges) {
  vector<string> * allTiles = Indexerp::getTilesByDimSubRange(subranges);
  vector<string> * wholeTiles = Indexerp::getWholeTilesByDimSubRange(subranges);

  set<string> setAll = set<string>(allTiles->begin(), allTiles->end());
  set<string> setWhole = set<string>(wholeTiles->begin(), wholeTiles->end());

  set<string> diffSet;
  std::set_difference(setAll.begin(), setAll.end(), setWhole.begin(), setWhole.end(), std::inserter(diffSet, diffSet.end()));

  vector<string> * output = new vector<string>();
  for (set<string>::iterator it = diffSet.begin(); it != diffSet.end(); ++it) {
    output->push_back(*it);
  }

  delete allTiles;
  delete wholeTiles;
  return output;

}

