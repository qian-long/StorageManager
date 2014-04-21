#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Filterp.h"

// TODO take in as parameter from somewhere
#define LIMIT 50
using namespace std;
// Constructor
Filterp::Filterp(Indexerp * indexer, int attrIndex, FilterType ftype, int64_t value, string name) {
  this->indexer = indexer;
  this->attrIndex = attrIndex;
  this->ftype = ftype;
  this->value = value;
  this->name = name;
}

// Destructor
// TODO
Filterp::~Filterp() {}

void Filterp::filter() {
  // Create output directory
  // TODO: add better error handling
  if (mkdir(this->name.c_str(), S_IRWXU) != 0) {
    perror("something went wrong");
    return;
  }

  vector<string> * tileids = indexer->tileids;

  for (vector<string>::iterator it = tileids->begin(); it != tileids->end(); ++it) {
    Filterp::filterTile(*it);
  }
}

// Filters one tile/attribute pair
// Read from unfiltered tiles, write into new tiles
void Filterp::filterTile(string tileid) {
  string coordTile = indexer->getCoordTileById(tileid);
  // work with compressed data
  string rleAttrTile = indexer->getRLEAttrTileById(attrIndex, tileid);

  // input files
  FILE * coordFilep;
  FILE * attrFilep;
  coordFilep = fopen(coordTile.c_str(), "r");
  if (!coordFilep) {
    perror("Coord tile doesn't exist");
  }
  attrFilep = fopen(rleAttrTile.c_str(), "r");
  if (!attrFilep) {
    perror("RLE Attr tile doesn't exist");
  }
  //uint64_t limit = (LIMIT/8) * 8;
  // TODO adjust
  uint64_t limit = 16;
  char inCoordBuf[1024];
  char inAttrBuf[LIMIT];
  stringstream outCoordBuf;
  stringstream outAttrBuf;

  ofstream outCoordFile;
  ofstream outAttrFile;
  string cfilename = this->name + "/" + coordTile;
  string afilename = this->name + "/" + rleAttrTile;

  if (!outCoordFile.is_open()) {
    outCoordFile.open(cfilename, std::fstream::app);
  }
  if (!outAttrFile.is_open()) {
    outAttrFile.open(afilename, std::fstream::app);
  }

  uint64_t creadsize = 0;
  uint64_t areadsize = fread((char *)inAttrBuf,1, limit, attrFilep);
  uint64_t coordIndex = 0;
  uint64_t usedMem = 0;
  // read attribute tile, scan it, then read the corresponding amount from coord tile
  while (areadsize) {
    // scan through attribute tile to figure how how much of coords tile to read
    uint64_t coordCount = 0;
    for (uint64_t i = 0; i < areadsize; i = i + 16) {
      uint64_t occurrence = *((uint64_t *)(inAttrBuf + i));
      int64_t attribute = *((int64_t *)(inAttrBuf + i + 8));
      cout << "occurrence: " << occurrence << endl;
      cout << "attribute: " << attribute << endl;

      coordCount += occurrence;
    }
    // read coordCount * nDim * 8 bytes from coordFile
    cout << "num bytes to read from coordFilep: " << coordCount * indexer->nDim * 8 << endl;
    creadsize = fread((char *)inCoordBuf, 1, coordCount * indexer->nDim * 8, coordFilep);
    cout << "creadsize: " << creadsize << endl;

    // reset coordIndex
    coordIndex = 0;
    // Walk coordinate and attribute file at the same time
    for (uint64_t i = 0; i < areadsize; i = i + 16) {
      uint64_t occurrence = *((int64_t *)(inAttrBuf + i));
      int64_t attribute = *((int64_t *)(inAttrBuf + i + 8));
      if (Filterp::evaluate(attribute)) {
        outAttrBuf.write((char *)(&occurrence), 8);
        outAttrBuf.write((char *)(&attribute), 8);
        // write all the coordinates associated with that attribute
        outCoordBuf.write((char *)(inCoordBuf + coordIndex), 8 * indexer->nDim * occurrence);
        usedMem += 16 + 8 * indexer->nDim * occurrence;
      }

      // next coordinate index
      coordIndex += this->indexer->nDim * 8 * occurrence;
    }

    // Next read
    areadsize = fread((char *)inAttrBuf,1, limit, attrFilep);

    // TODO check memory limit
    // write to file
    if (usedMem > LIMIT) { 
      cout << endl;
      cout << "memory reached, writing to file" << endl;
      outCoordFile << outCoordBuf.str();
      outAttrFile << outAttrBuf.str();

      // reset buffers and usedMem
      outCoordBuf.str(std::string());
      outAttrBuf.str(std::string());
      usedMem = 0;
    }
  }

  // Final flush
  cout << "final flush" << endl;
  outCoordFile << outCoordBuf.str();
  outAttrFile << outAttrBuf.str();

  // Close all files
  fclose(attrFilep);
  fclose(coordFilep);
  outCoordFile.close();
  outAttrFile.close();
}

bool Filterp::evaluate(int64_t attrval) {

  switch(this->ftype) {
    case GT:
      return attrval > this->value;
    case GE:
      return attrval >= this->value;
    case EQ:
      return attrval == this->value;
    case LT:
      return attrval < this->value;
    case LE:
      return attrval <= this->value;
    default:
      return false;
  }

}
