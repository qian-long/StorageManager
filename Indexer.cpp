#include <iostream>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include <unistd.h>
#include "Indexer.h"

using namespace std;

// Constructor
Indexer::Indexer(int nDim, vector<int64_t> ranges, int nAttr, int stride) {
  this->nDim = nDim;
  this->nAttr = nAttr;
  this->ranges = ranges;
  this->stride = stride;

  // Initialize maps
  this->tileids = this->generateTileIds();
  for (vector<string>::iterator it = tileids->begin(); it != tileids->end(); ++it) {
    string suffix = *it;
    for (int i = 0; i < nAttr; ++i) {
      string filename = "tile-attrs[" + to_string(i) + "]-" + (*it) + ".dat";

      // Checks that file exists
      if( access( filename.c_str(), F_OK ) != -1 ) {
        // file exists
        attrToTileMap[i].push_back(filename);
      }
    }
  }

}

// Destructor
// TODO
Indexer::~Indexer() {
  delete tileids;
}


vector<string> * Indexer::findTilesByAttribute(int attrIndex) {
  return &attrToTileMap[attrIndex];
}

string Indexer::getAttrTileById(int attrIndex, string tileid) {

  string filename = "tile-attrs[" + to_string(attrIndex) + "]-" + tileid + ".dat";
  return filename;
}


string Indexer::getCoordTileById(string tileid) {

  string filename = "tile-coords-" + tileid + ".dat";
  return filename;

}

vector<string> * Indexer::getAllAttrTilesById(string tileid) {
  vector<string> * attrTiles = new vector<string>();
  for (int i = 0; i < this->nAttr; ++i) {
    attrTiles->push_back(Indexer::getAttrTileById(i, tileid));
  }
  return attrTiles;
}

// Returns tile id given coordinates
string Indexer::getTileIdByCoords(vector<int64_t> * coords) {
  vector<int64_t>::iterator it = coords->begin();
  string id = to_string(*it/this->stride);
  it++;

  for (; it != coords->end(); ++it) {
    id = id + "-" + to_string(*it/this->stride);
  }

  return id;
}

// Returns all attribute tiles given coordinates
vector<string> * Indexer::getAllAttrTilesByCoords(vector<int64_t> * coords) {
  string id = getTileIdByCoords(coords);
  return Indexer::getAllAttrTilesById(id);
}
// Private functions
// Produce all combinations of [(0,...,x), (0,...,y), ...]
vector<string> * Indexer::generateTileIds() {
  vector<int> splits;
  vector<int64_t>::iterator itr = this->ranges.begin();

  for (int i = 0; i < nDim; ++i) {
    int64_t begin = *(itr++);
    int64_t end = *(itr++);
    int numSplits = (end - begin) / this->stride;
    // If not even split, add an extra one
    if ((end - begin) % this->stride != 0) {
      numSplits++;
    }
    splits.push_back(numSplits);
  }


  vector<string> output;
  vector<int>::iterator it = splits.begin();
  int first = *it;
  for (int i = 0; i < first; ++i) {
    output.push_back(to_string(i));
  }
  ++it;

  while (it != splits.end()) {
    int numSplits = *it;
    vector<string> copy;
    for (vector<string>::iterator ito = output.begin(); ito != output.end(); ++ito) {
      copy.push_back(*ito);

      // modify output
      *ito = "0-" + *ito;
    }

    for (int i = 1; i < numSplits; ++i) {
      for (vector<string>::iterator ito = copy.begin(); ito != copy.end(); ++ito) {
        output.push_back(to_string(i) + "-" + *ito);
      }
    }
    ++it;
  }

  vector<string> * filteredOutput = new vector<string>();
  // Filter output to only files that exist
  for (vector<string>::iterator it = output.begin(); it != output.end(); ++it) {
    string filename = this->getCoordTileById(*it);
    if (Indexer::fileExists(filename)) {
      filteredOutput->push_back(*it);
    }
  }

  return filteredOutput;
}

// Checks that file exists
bool Indexer::fileExists(string filename) {
  if( access( filename.c_str(), F_OK ) != -1 ) {
    // file exists
    return true;
  }
  return false;
}
