#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include "Filter.h"

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
    return;
  }

  vector<string> * tileids = indexer->tileids;

  for (vector<string>::iterator it = tileids->begin(); it != tileids->end(); ++it) {
    string coordTile = indexer->getCoordTileById(*it);
    string attrTile = indexer->getAttrTileById(attrIndex, *it);

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
