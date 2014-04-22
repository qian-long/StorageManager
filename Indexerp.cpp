#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
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
  this->tileids = new vector<string>();

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
      tileids->push_back(tileid);
      for (int i = 0; i < nAttr; ++i) {
        string filename = "tile-attrs[" + to_string(i) + "]-" + tileid + "-fp.dat";
        (*attrToTileMap)[i].push_back(filename);

      }
    }
  }
}

/*
vector<string> * Indexerp::findTilesByAttribute(int attrIndex) {
  return &(*attrToTileMap)[attrIndex];
}
*/
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
    cout << "tileid: " << tileid << endl;
    vector<int64_t>::iterator itMin = box->minCoords->begin();
    vector<int64_t>::iterator itMax = box->maxCoords->begin();
    bool wholeTile = true;

    // compare subranges
    for (vector<int64_t>::iterator its = subranges->begin(); its != subranges->end(); its = its + 2) {
      int64_t lowCoord = *(its);
      int64_t hiCoord = *(its + 1);
      int64_t minCoord = *itMin;
      int64_t maxCoord = *itMax;

      cout << "minCoord: " << minCoord << " maxCoord: " << maxCoord;
      cout << " lowCoord: " << lowCoord << " hiCoord: " << hiCoord << endl;
      if (!(minCoord >= lowCoord && maxCoord<= hiCoord)) {
        cout << " false" << endl << endl;
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

