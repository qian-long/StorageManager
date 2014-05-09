#include <iostream>
#include <fstream>
#include <sstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <set>
#include "Debug.h"
#include "Subarray.h"

# define LIMIT 100000000 // 100MB buffer
using namespace std;

Subarray::Subarray(string name, Indexer * indexer, vector<int64_t> * subranges, vector<int64_t> * ranges, int64_t stride) {
  this->name = name;
  this->indexer = indexer;
  this->subranges = subranges;
  this->ranges = ranges;
  this->stride = stride;
  this->outdir = indexer->arraydir + "/" + name;
}

Subarray::~Subarray() {

}

void Subarray::execute() {
  // TODO: add better error handling
  // Create directory for new array
  if (mkdir(outdir.c_str(), S_IRWXU) != 0) {
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
    ifstream sourceTile(indexer->arraydir + "/" + coordTile, ios::binary);
    ofstream destTile(outdir + "/" + coordTile, ios::binary);
    destTile << sourceTile.rdbuf();
    sourceTile.close();
    destTile.close();

    vector<string> * rleTiles = indexer->getAllRLEAttrTilesById(tileid);
    // Copy all the compressed attribute tiles
    for (vector<string>::iterator ita = rleTiles->begin(); ita != rleTiles->end(); ++ita) {
      string attrTile = *ita;
      ifstream source(indexer->arraydir + "/" + attrTile, ios::binary);
      ofstream dest(outdir + "/" + attrTile, ios::binary);
      dest << source.rdbuf();
      source.close();
      dest.close();
    }

    delete rleTiles;
  }

  // Iterate through partial files to find matching coordinates
  // TODO: use circular buffer later?
  // TODO adjust based on number of dimensions
  uint64_t limit = (LIMIT/8 + 1)*8;
  //char inCoordBuf[limit];
  char * inCoordBuf = new char[limit];

  for (vector<string>::iterator it = partialTiles->begin(); it != partialTiles->end(); ++it) {
    vector<uint64_t> inRangeCellNums; // cells in coordinate tile that is in range
    string tileid = *it;

    dbgmsg("");
    dbgmsg("Processing partial id: " + tileid);
    dbgmsg("inRangeCellNums.size(): " + to_string(inRangeCellNums.size()));
    // Start counting at 1
    uint64_t cellNum = 1;
    string coordTile = indexer->getCoordTileById(tileid);
    string coordTilePath = indexer->arraydir + "/" + coordTile;
    FILE * coordFilep;
    coordFilep = fopen(coordTilePath.c_str(), "r");
    if (!coordFilep) {
      perror("Coord tile doesn't exist");
    }
    ofstream outCoordFile;
    stringstream outCoordBuf;

    string cfilename = outdir + "/" + coordTile;
    uint64_t usedMem = 0;
    while (uint64_t creadsize = fread((char *)inCoordBuf, 1, limit, coordFilep)) {
      dbgmsg("creadsize: " + to_string(creadsize));
      // iterate through
      for (uint64_t i = 0; i < creadsize; i = i + 8*indexer->nDim) {

        // Build coordinates
        vector<int64_t> coords;
        for (int d = 0; d < indexer->nDim; ++d) {
          int64_t offset = i + 8*d;
          int64_t coord = *((int64_t *)(inCoordBuf + offset));
          dbgmsg("coord: " + to_string(coord));
          coords.push_back(coord);
        }

        if (Subarray::inRange(&coords)) {
          dbgmsg(" in range cellNum: " + to_string(cellNum));
          inRangeCellNums.push_back(cellNum);
          outCoordBuf.write((char *)(inCoordBuf + i), 8 * indexer->nDim);
          usedMem += 8 * indexer->nDim;
        }
        else {
          dbgmsg("\n");
        }

        // increment "coordinate line"
        cellNum++;
      }

      // write to output subarray tile file
      if (usedMem >= limit) {
        dbgmsg("flushing subarray coords to file");
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
    dbgmsg("remaining flush");
    outCoordFile << outCoordBuf.str();

    // Subarray each attribute file for this coordinate tile
    if (outCoordFile.is_open()) {
      for (int aindex = 0; aindex < indexer->nAttr; ++aindex) {
        Subarray::subarrayAttr(tileid, &inRangeCellNums, aindex);
      }
    }

    outCoordFile.close();
  }

  delete wholeTiles;
  delete partialTiles;
  delete [] inCoordBuf;
}

// cellNums: in range cell nums from the coordinate tile
// match up with attributes in this attribute tile

void Subarray::subarrayAttr(string tileid, vector<uint64_t> * cellNums, int attrIndex) {
  dbgmsg("SubArraying attribute cellNums: ");
  for (vector<uint64_t>::iterator it = cellNums->begin(); it != cellNums->end(); ++it) {
    dbgmsg(*it);
  }
  set<uint64_t> cellNumSet = set<uint64_t>(cellNums->begin(), cellNums->end());

  FILE * attrFilep;
  string attrTile = indexer->getRLEAttrTileById(attrIndex, tileid);
  string attrTilePath = indexer->arraydir + "/" + attrTile;
  attrFilep = fopen(attrTilePath.c_str(), "r");

  if (!attrFilep) {
    perror("Subarray RLE Attr tile doesn't exist");
  }

  // TODO adjust
  //uint64_t limit = 32;

  uint64_t limit = (LIMIT/8 + 1) * 8;
  //char inAttrBuf[limit];
  char * inAttrBuf = new char[limit];
  uint64_t cellCount = 1;
  uint64_t usedMem = 0;

  // output buffer and file
  stringstream outAttrBuf;
  ofstream outAttrFile;
  string afilename = outdir + "/" + attrTile;

  while (uint64_t areadsize = fread((char *) inAttrBuf, 1, limit, attrFilep)) {
    for (uint64_t i = 0; i < areadsize; i = i + 16) {
      uint64_t occurrence = *((uint64_t *)(inAttrBuf + i));
      int64_t attribute = *((int64_t *)(inAttrBuf + i + 8));
      dbgmsg(" occurrence: " + to_string(occurrence));
      dbgmsg("attribute: " + to_string(attribute));
      uint64_t new_occurrence = 0;
      for (uint64_t occur = 0; occur < occurrence; ++occur) {
        if (cellNumSet.find(cellCount + occur) != cellNumSet.end()) {
          dbgmsg(" found cellCount + occur: " + to_string(cellCount + occur));
          new_occurrence++;
        }
        else {
          dbgmsg(" not in cellnums");
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
      dbgmsg("flushing to subarray attribute file");
      outAttrFile << outAttrBuf.str();
      // resetting
      outAttrBuf.str(std::string());
      usedMem = 0;
    }
  }

  // write what's remaining in buffer
  dbgmsg("final flushing subarray attribute file");
  outAttrFile << outAttrBuf.str();
  outAttrFile.close();
  fclose(attrFilep);

  delete [] inAttrBuf;
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

