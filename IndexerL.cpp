#include <iostream>
#include <fstream>
#include <string>
#include <math.h>
#include <map>
#include <vector>
#include <unistd.h>
#include <algorithm>
#include "Debug.h"
#include "IndexerL.h"

using namespace std;

// Constructor
IndexerL::IndexerL(int nDim, vector<int64_t> ranges, int nAttr, int pstride, string arraydir): Indexer(nDim, ranges, nAttr, arraydir), stride(pstride) {

  dbgmsg("In IndexerL Constructor");
  dbgmsg("indexfile: " + indexfile);
  dbgmsg("pstride: " + to_string(pstride));
  // Open and read index file
  ifstream indexIn(indexfile);
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
  dbgmsg("tileidset.size(): " + to_string(tileidset->size()));
  dbgmsg("this->stride: " + to_string(stride));
}

// Destructor
// TODO
IndexerL::~IndexerL() {
  delete tileids;
  delete tileidset;
}

vector<string> * IndexerL::getTilesByDimSubRange(vector<int64_t> * subranges) {
  dbgmsg("In getTileByDimSubRange");
  dbgmsg("stride: " + to_string(stride));
  vector<int64_t>::iterator it = subranges->begin();
  //map<int, vector<int>> wholeTilesMap;
  // maps dimension to all id ranges for that dimension
  int totalmapsize = 0;
  map<int, vector<int>> allTilesMap;

  for (int i = 0; i < nDim; ++i) {
    int64_t startRange = *(it++);
    int64_t endRange = *(it++);

    dbgmsg("dim: " + to_string(i) + " startRange : " + to_string(startRange) + " endRange: " + to_string(endRange));
    //int pbegin = (int) floor((double) startRange / stride);
    //int pend = (int) ceil((double) endRange / stride);
    int pbegin = startRange / stride;
    int pend = endRange / stride;

    dbgmsg("pbegin: " + to_string(pbegin) + " pend: " + to_string(pend));
    for (int j = pbegin; j <= pend; ++j) {
      allTilesMap[i].push_back(j);
      totalmapsize++;
    }

  }

  dbgmsg("allTilesMap->size(): " + to_string(allTilesMap.size()));
  dbgmsg("totalmapsize: " + to_string(totalmapsize));
  vector<string> * allTileIDs = IndexerL::combination(&allTilesMap);
  dbgmsg("allTilesIDS.size(): " + to_string(allTileIDs->size()));
  for(vector<string>::iterator it = allTileIDs->begin(); it != allTileIDs->end(); ++it) {
    dbgmsg("potential tile id: " + *it);
  }
  // take intersection with all tileids
  vector<string> * output = new vector<string>();
  for (vector<string>::iterator it = allTileIDs->begin(); it != allTileIDs->end(); ++it) {
    if (tileidset->find(*it) != tileidset->end()) {
      dbgmsg("Found overlapping tile: " + *it);
      output->push_back(*it);
    }
  }

  delete allTileIDs;
  return output;
}

vector<string> * IndexerL::getWholeTilesByDimSubRange(vector<int64_t> * subranges) {
  dbgmsg("In getWholeTiles");
  vector<int64_t>::iterator it = subranges->begin();
  map<int, vector<int>> wholeTilesMap;


  for (int i = 0; i < this->nDim; ++i) {

    int64_t startRange = *(it++);
    int64_t endRange = *(it++);

    dbgmsg("dim: " + to_string(i) + " startRange : " + to_string(startRange) + " endRange: " + to_string(endRange));
    /*
    int wbegin = (int) ceil((double) startRange / this->stride);
    int wend = (int) floor((double) endRange / this->stride);
    */
    int wbegin;
    int wend;

    // on the edge
    if (startRange / stride != (startRange - 1) / stride) {
      wbegin = startRange / stride;
    }
    else {
      wbegin = startRange / stride + 1;
    }

    // on the edge
    if (endRange / stride != (endRange + 1) / stride) {
      wend = endRange / stride;
    }
    else {
      wend = endRange / stride - 1;
    }

    dbgmsg("wbegin: " + to_string(wbegin) + " wend: " + to_string(wend));
    for (int k = wbegin; k <= wend; ++k) {
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
  dbgmsg("End getWholeTile");
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

  dbgmsg("In combination");
  vector<string> * output = new vector<string>();

  //map<int, vector<int>>::iterator it = tileIDMaps->begin();
  //int first = *it;
  vector<int> firstVector = (*tileIDMaps)[0];

  for (vector<int>::iterator it = firstVector.begin(); it != firstVector.end(); ++it) {

    dbgmsg("In combination, first insert to output: " + to_string(*it));
    output->push_back(to_string(*it));
  }

  for (int i = 1; i < this->nDim; ++i) {
    dbgmsg("i: " + to_string(i));

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
      dbgmsg("In combination, insert to output: " + *itc);
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
    dbgmsg("combination filename: " + filename);
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
