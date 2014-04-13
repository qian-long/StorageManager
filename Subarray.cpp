#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <set>
#include "Subarray.h"

using namespace std;

Subarray::Subarray(string name, Indexer * indexer, vector<int64_t> * subranges, vector<int64_t> * ranges, int64_t stride) {
  this->name = name;
  this->indexer = indexer;
  this->subranges = subranges;
  this->ranges = ranges;
  this->stride = stride;
}

Subarray::~Subarray() {

}

void Subarray::execute() {
  // TODO: add better error handling
  // Create directory for new array
  if (mkdir(this->name.c_str(), S_IRWXU) != 0) {
    perror ("Directory exists, query executed before");
    return;
  }


  vector<string> * wholeTiles = indexer->getWholeTilesByDimSubRange(subranges);
  vector<string> * partialTiles = indexer->getPartialTilesByDimSubRange(subranges);
  // Copy whole tiles over because they are entirely within the subarray boundaries
  for (vector<string>::iterator it = wholeTiles->begin(); it != wholeTiles->end(); ++it) {
    string tileid = *it;
    // Copy coordinate tile
    string coordTile = indexer->getCoordTileById(tileid);
    ifstream sourceTile(coordTile, ios::binary);
    ofstream destTile(this->name + "/" + coordTile, ios::binary);
    destTile << sourceTile.rdbuf();
    sourceTile.close();
    destTile.close();

    vector<string> * rleTiles = indexer->getAllRLEAttrTilesById(tileid);
    // Copy all the compressed attribute tiles
    for (vector<string>::iterator ita = rleTiles->begin(); ita != rleTiles->end(); ++ita) {
      string attrTile = *ita;
      ifstream source(attrTile, ios::binary);
      ofstream dest(this->name + "/" + attrTile, ios::binary);
      dest << source.rdbuf();
      source.close();
      dest.close();
    }

    delete rleTiles;
  }

  // Iterate through partial files to find matching coordinates
  // TODO: use circular buffer later?
  // TODO adjust based on number of dimensions
  uint64_t limit = 16;
  char inCoordBuf[limit];

  for (vector<string>::iterator it = partialTiles->begin(); it != partialTiles->end(); ++it) {
    vector<uint64_t> inRangeCellNums; // cells in coordinate tile that is in range
    string tileid = *it;

    cout << endl << "Processing partial id: " << tileid << endl;
    cout << "inRangeCellNums.size(): " << inRangeCellNums.size() << endl;
    // Start counting at 1
    uint64_t cellNum = 1;
    string coordTile = indexer->getCoordTileById(tileid);
    FILE * coordFilep;
    coordFilep = fopen(coordTile.c_str(), "r");
    if (!coordFilep) {
      perror("Coord tile doesn't exist");
    }
    ofstream outCoordFile;
    stringstream outCoordBuf;

    string cfilename = this->name + "/" + coordTile;
    uint64_t usedMem = 0;
    while (uint64_t creadsize = fread((char *)inCoordBuf, 1, limit, coordFilep)) {
      cout << "creadsize: " << creadsize << endl;

      // iterate through
      for (uint64_t i = 0; i < creadsize; i = i + 8*indexer->nDim) {

        // Build coordinates
        vector<int64_t> coords;
        for (int d = 0; d < indexer->nDim; ++d) {
          int64_t offset = i + 8*d;
          int64_t coord = *((int64_t *)(inCoordBuf + offset));
          cout << " coord: " << coord;
          coords.push_back(coord);
        }

        if (Subarray::inRange(&coords)) {
          cout << " in range cellNum: " << cellNum << endl;
          inRangeCellNums.push_back(cellNum);
          outCoordBuf.write((char *)(inCoordBuf + i), 8 * indexer->nDim);
          usedMem += 8 * indexer->nDim;
        }
        else {
          cout << endl;
        }

        // increment "coordinate line"
        //inRangeCellNums.push_back(cellNum);
        cellNum++;
      }

      // write to output subarray tile file
      if (usedMem >= limit) {
        cout << "flushing subarray coords to file" << endl;
        if (!outCoordFile.is_open()) {
          outCoordFile.open(cfilename, std::fstream::app);
        }
        outCoordFile << outCoordBuf.str();

        // resets
        outCoordBuf.str(std::string());
        usedMem = 0;
      }
    }

    // flush remaining contents of outCoordBuf
    cout << "remaining flush" << endl;
    outCoordFile << outCoordBuf.str();

    // Subarray each attribute file for this coordinate tile
    // TODO loop through nAttr
    cout << "ASDFASDFSAFSAFDS tileid: " << tileid << endl;
    if (outCoordFile.is_open()) {
      for (int aindex = 0; aindex < indexer->nAttr; ++aindex) {
        Subarray::subarrayAttr(tileid, &inRangeCellNums, aindex);
      }
    }

    outCoordFile.close();
  }

  delete wholeTiles;
  delete partialTiles;
}

// cellNums: in range cell nums from the coordinate tile
// match up with attributes in this attribute tile

void Subarray::subarrayAttr(string tileid, vector<uint64_t> * cellNums, int attrIndex) {
  cout << "SubArraying attribute cellNums: " << endl;
  for (vector<uint64_t>::iterator it = cellNums->begin(); it != cellNums->end(); ++it) {
    cout << *it << " ";
  }
  cout << endl;
  set<uint64_t> cellNumSet = set<uint64_t>(cellNums->begin(), cellNums->end());

  FILE * attrFilep;
  string attrTile = indexer->getRLEAttrTileById(attrIndex, tileid);
  attrFilep = fopen(attrTile.c_str(), "r");

  if (!attrFilep) {
    perror("Subarray RLE Attr tile doesn't exist");
  }

  // TODO adjust
  uint64_t limit = 32;
  char inAttrBuf[limit];
  uint64_t cellCount = 1;
  uint64_t usedMem = 0;

  // output buffer and file
  stringstream outAttrBuf;
  ofstream outAttrFile;
  string afilename = this->name + "/" + attrTile;

  while (uint64_t areadsize = fread((char *) inAttrBuf, 1, limit, attrFilep)) {
    for (uint64_t i = 0; i < areadsize; i = i + 16) {
      uint64_t occurrence = *((uint64_t *)(inAttrBuf + i));
      int64_t attribute = *((int64_t *)(inAttrBuf + i + 8));
      cout << " occurrence: " << occurrence;
      cout << " attribute: " << attribute;
      uint64_t new_occurrence = 0;
      for (uint64_t occur = 0; occur < occurrence; ++occur) {
        if (cellNumSet.find(cellCount + occur) != cellNumSet.end()) {
          cout << " found cellCount + occur: " << cellCount + occur << endl;
          new_occurrence++;
        }
        else {
          cout << " not in cellnums" << endl;
        }
      }

      // write to buffer
      if (new_occurrence > 0) {
        if (!outAttrFile.is_open()) {
          outAttrFile.open(afilename, std::fstream::app);
        }
        outAttrBuf.write((char *)(&new_occurrence), 8);
        outAttrBuf.write((char *)(&attribute), 8);
        usedMem += 16;
      }

      cellCount += occurrence;
    }

    if (usedMem > limit) {
      cout << "flushing to subarray attribute file" << endl;
      outAttrFile << outAttrBuf.str();
      // resetting
      outAttrBuf.str(std::string());
      usedMem = 0;
    }
  }

  // write what's remaining in buffer
  cout << "final flushing subarray attribute file" << endl;
  outAttrFile << outAttrBuf.str();
  outAttrFile.close();
  fclose(attrFilep);
}

// Private Functions
bool Subarray::inRange(vector<int64_t> * coords) {
  if (coords->size() == 0) {
    return false;
  }
  vector<int64_t>::iterator itsub = this->subranges->begin();

  for (vector<int64_t>::iterator it = coords->begin(); it != coords->end(); ++it) {
    int64_t start = *(itsub++);
    int64_t end = *(itsub++);
    int64_t value = *it;

    if (value < start || value > end) {
      return false;
    }
  }

  return true;
}

