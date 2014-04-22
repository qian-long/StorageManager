#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include "IndexerL.h"

using namespace std;

// Constructor
IndexerL::IndexerL(int nDim, vector<int64_t> ranges, int nAttr, int stride, string indexfile): Indexer(nDim, ranges, nAttr, indexfile) {

  // Open and read index file
  ifstream indexIn(this->indexfile);
  string line;
  if (indexIn.is_open()) {
    while (getline(indexIn, line)) {
      tileids->push_back(line);
      for (int i = 0; i < nAttr; ++i) {
        string filename = "tile-attrs[" + to_string(i) + "]-" + line + ".dat";
        (*attrToTileMap)[i].push_back(filename);
      }
    }
  }
  else {
    perror("Unable to open index file");
  }

  this->tileidset = new set<string>(tileids->begin(), tileids->end());
}

// Destructor
// TODO
IndexerL::~IndexerL() {
  delete tileids;
  delete tileidset;
}

vector<string> * IndexerL::getTilesByDimSubRange(vector<int64_t> * subranges) {
  vector<int64_t>::iterator it = subranges->begin();
  //map<int, vector<int>> wholeTilesMap;
  map<int, vector<int>> allTilesMap;

  for (int i = 0; i < nDim; ++i) {
    int64_t startRange = *(it++);
    int64_t endRange = *(it++);

    int pbegin = (int) floor((double) startRange / this->stride);
    int pend = (int) ceil((double) endRange / this->stride);

    for (int j = pbegin; j < pend; ++j) {
      allTilesMap[i].push_back(j);
    }

  }

  vector<string> * allTileIDs = IndexerL::combination(&allTilesMap);
  // take intersection with all tileids
  vector<string> * output = new vector<string>();
  for (vector<string>::iterator it = allTileIDs->begin(); it != allTileIDs->end(); ++it) {
    if (tileidset->find(*it) != tileidset->end()) {
      output->push_back(*it);
    }
  }

  delete allTileIDs;
  return output;
}

vector<string> * IndexerL::getWholeTilesByDimSubRange(vector<int64_t> * subranges) {
  vector<int64_t>::iterator it = subranges->begin();
  map<int, vector<int>> wholeTilesMap;

  for (int i = 0; i < this->nDim; ++i) {
    int64_t startRange = *(it++);
    int64_t endRange = *(it++);
    int wbegin = (int) ceil((double) startRange / this->stride);
    int wend = (int) floor((double) endRange / this->stride);
    for (int k = wbegin; k < wend; ++k) {
      wholeTilesMap[i].push_back(k);

    }
  }

  vector<string> * wholeTileIDs = IndexerL::combination(&wholeTilesMap);
  // take intersection with all tileids
  vector<string> * output = new vector<string>();
  for (vector<string>::iterator it = wholeTileIDs->begin(); it != wholeTileIDs->end(); ++it) {
    if (tileidset->find(*it) != tileidset->end()) {
      output->push_back(*it);
    }
  }

  delete wholeTileIDs;
  return output;

}

vector<string> * IndexerL::getPartialTilesByDimSubRange(vector<int64_t> * subranges) {
  vector<string> * allTiles = IndexerL::getTilesByDimSubRange(subranges);
  vector<string> * wholeTiles = IndexerL::getWholeTilesByDimSubRange(subranges);

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

// Keys are ints from 0 to nDim - 1
vector<string> * IndexerL::combination(map<int, vector<int>> * tileIDMaps) {

  vector<string> * output = new vector<string>();

  //map<int, vector<int>>::iterator it = tileIDMaps->begin();
  //int first = *it;
  vector<int> firstVector = (*tileIDMaps)[0];

  for (vector<int>::iterator it = firstVector.begin(); it != firstVector.end(); ++it) {
    output->push_back(to_string(*it));
  }

  for (int i = 1; i < this->nDim; ++i) {

    // copy output
    vector<string> copy;
    for (vector<string>::iterator it = output->begin(); it != output->end(); ++it) {

      string idPrefix = *it;
      for (vector<int>::iterator itd = (*tileIDMaps)[i].begin(); itd != (*tileIDMaps)[i].end(); ++itd) {
        string newIdPrefix = idPrefix + "-" + to_string(*itd);
        copy.push_back(newIdPrefix);
      }

    }
    // clear output
    output->clear();
    // deallocates memory
    output->shrink_to_fit();

    // insert to output
    for (vector<string>::iterator itc = copy.begin(); itc != copy.end(); ++itc) {
      output->push_back(*itc);
    }
  }

  return output;
}

// PRIVATE FUNCTIONS
// Produce all combinations of [(0,...,x), (0,...,y), ...]
vector<string> * IndexerL::generateTileIds() {
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
    cout << "combination filename: " << filename << endl;
    if (IndexerL::fileExists(filename)) {
      filteredOutput->push_back(*it);
    }
  }

  return filteredOutput;
}

// Checks that file exists
bool IndexerL::fileExists(string filename) {
  if( access( filename.c_str(), F_OK ) != -1 ) {
    // file exists
    return true;
  }
  return false;
}
