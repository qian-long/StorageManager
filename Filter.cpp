#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include "Filter.h"

// TODO take in as parameter from somewhere
#define LIMIT 50
using namespace std;
// Constructor
Filter::Filter(Indexer * indexer, int attrIndex, FilterType ftype, int64_t value, string name) {
  this->indexer = indexer;
  this->attrIndex = attrIndex;
  this->ftype = ftype;
  this->value = value;
  this->name = name;
}

// Destructor
// TODO
Filter::~Filter() {}

void Filter::filter() {

  // TODO: add better error handling
  if (mkdir(this->name.c_str(), S_IRWXU) != 0) {
    perror("something went wrong");
    return;
  }

  vector<string> * tileids = indexer->tileids;

  for (vector<string>::iterator it = tileids->begin(); it != tileids->end(); ++it) {
    string coordTile = indexer->getCoordTileById(*it);
    string attrTile = indexer->getAttrTileById(attrIndex, *it);

    string rleAttrTile = indexer->getRLEAttrTileById(attrIndex, *it);

    // input stream
    ifstream attrFile(attrTile);
    ifstream coordFile(coordTile);
    string line;
    string coordLine;

    // output stream
    ofstream newAttrFile;
    ofstream newCoordFile;
    string afilename = this->name + "/" + attrTile;
    string cfilename = this->name + "/" + coordTile;


    if (attrFile.is_open() && coordFile.is_open()) {

      // Iterate through attributes and coordinates at the same time
      while (getline(attrFile, line) && getline(coordFile, coordLine)) {
        int64_t value = atoi(line.c_str());
        if (Filter::evaluate(value)) {
          if (!newAttrFile.is_open()) {
            newAttrFile.open(afilename, std::fstream::app);
          }
          if (!newCoordFile.is_open()) {
            newCoordFile.open(cfilename, std::fstream::app);
          }
          newAttrFile << value << endl;
          newCoordFile << coordLine << endl;
        }
      }
    }

    // Close files
    attrFile.close();
    coordFile.close();
    newAttrFile.close();
    newCoordFile.close();
  }
}

void Filter::filter2() {
  // TODO: add better error handling
  if (mkdir(this->name.c_str(), S_IRWXU) != 0) {
    perror("something went wrong");
    return;
  }

  vector<string> * tileids = indexer->tileids;

  for (vector<string>::iterator it = tileids->begin(); it != tileids->end(); ++it) {
    string coordTile = indexer->getCoordTileById(*it);
    // work with compressed data
    string rleAttrTile = indexer->getRLEAttrTileById(attrIndex, *it);
    cout << "coordTile: " << coordTile << endl;
    cout << "rleAttrTile: " << rleAttrTile << endl;

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
    uint64_t limit = (LIMIT/8) * 8;
    char inCoordBuf[LIMIT];
    char inAttrBuf[LIMIT];
    stringstream outCoordBuf;
    stringstream outAttrBuf;

    // TODO finish!!!
    uint64_t creadsize = fread((char *)inCoordBuf, 1, limit, coordFilep);
    uint64_t areadsize = fread((char *)inAttrBuf,1, limit, attrFilep);
    uint64_t coordIndex = 0;
    while(creadsize && areadsize) {
      cout << "creadsize: " << creadsize << endl;
      cout << "areadsize: " << areadsize << endl;
      // Walk coordinate and attribute file at the same time
      for (uint64_t i = 0; i < areadsize; i = i + 16) {
        int64_t occurrence = *((int64_t *)(inAttrBuf + i));
        int64_t attribute = *((int64_t *)(inAttrBuf + i + 8));
        cout << "occurrence: " << occurrence << endl;
        cout << "attribute: " << attribute << endl;
        if (Filter::evaluate(attribute)) {
          outAttrBuf.write((char *)(&occurrence), 8);
          outAttrBuf.write((char *)(&attribute), 8);
          // write all the coordinates associated with that attribute
          outCoordBuf.write((char *)(inCoordBuf + coordIndex), 8 * indexer->nDim * occurrence);
        }
        // next coordinate index
        coordIndex += indexer->nDim * 8 * occurrence;
      }
      // Next read
      creadsize = fread((char *)inCoordBuf, 1, limit, coordFilep);
      areadsize = fread((char *)inAttrBuf,1, limit, attrFilep);

    }
  }
}

void Filter::filterTile(string tileid) {
  string coordTile = indexer->getCoordTileById(tileid);
  // work with compressed data
  string rleAttrTile = indexer->getRLEAttrTileById(attrIndex, tileid);
  cout << "coordTile: " << coordTile << endl;
  cout << "rleAttrTile: " << rleAttrTile << endl;

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
  uint64_t limit = 16;
  char inCoordBuf[1024];
  char inAttrBuf[LIMIT];
  stringstream outCoordBuf;
  stringstream outAttrBuf;

  stringstream coordLeftOver;
  stringstream attrLeftOver;
  // TODO finish!!!
  //uint64_t creadsize = fread((char *)inCoordBuf, 1, limit, coordFilep);
  uint64_t creadsize = 0;
  uint64_t areadsize = fread((char *)inAttrBuf,1, limit, attrFilep);
  uint64_t coordIndex = 0;
  // read attribute tile, scan it, then read the corresponding amount from coord tile
  while (areadsize) {
    cout << "areadsize: " << areadsize << endl;
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
    cout << "coordCount * indexer->nDim * 8:" << coordCount * indexer->nDim * 8 << endl;
    creadsize = fread((char *)inCoordBuf, 1, coordCount * indexer->nDim * 8, coordFilep);
    cout << "creadsize: " << creadsize << endl;
    // Next read
    areadsize = fread((char *)inAttrBuf,1, limit, attrFilep);
  }
  /*
  while(creadsize && areadsize) {
    // process leftover from previous iteration
    
    cout << "creadsize: " << creadsize << endl;
    cout << "areadsize: " << areadsize << endl;
    // Walk coordinate and attribute file at the same time
    uint64_t i;
    for (i = 0; i < areadsize; i = i + 16) {
      int64_t occurrence = *((int64_t *)(inAttrBuf + i));
      int64_t attribute = *((int64_t *)(inAttrBuf + i + 8));
      cout << "occurrence: " << occurrence << endl;
      cout << "attribute: " << attribute << endl;
      if (Filter::evaluate(attribute)) {
        outAttrBuf.write((char *)(&occurrence), 8);
        outAttrBuf.write((char *)(&attribute), 8);
        // write all the coordinates associated with that attribute
        outCoordBuf.write((char *)(inCoordBuf + coordIndex), 8 * indexer->nDim * occurrence);
      }
      // next coordinate index
      coordIndex += this->indexer->nDim * 8 * occurrence;
    }
    cout << "coordIndex: " << coordIndex << endl;
    cout << "coord left overs: " << creadsize - coordIndex << endl;
    coordLeftOver.write((char *)(inCoordBuf + coordIndex), creadsize - coordIndex);
    uint64_t prevI = i - 16;
    cout << "attr leftover: " << areadsize - prevI << endl;
    attrLeftOver.write((char *)(inAttrBuf + prevI), areadsize - prevI);
    // Next read
    creadsize = fread((char *)inCoordBuf, 1, limit, coordFilep);
    areadsize = fread((char *)inAttrBuf,1, limit, attrFilep);
  }

  */
  // process leftovers
}

bool Filter::evaluate(int64_t attrval) {

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
