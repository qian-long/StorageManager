#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <bitset>
#include <algorithm>
#include <map>
#include "Loader.h"

#define TILENAME_TEMPLATE tile-%s-.dat
using namespace std;

// Constructor
Loader::Loader(string filename, int64_t nDim, vector<int64_t> ranges, int64_t nAttr, int stride) {
  this->filename = filename;
  this->nDim = nDim;
  this->ranges = ranges;
  this->nAttr = nAttr;
  this->stride = stride;
}

// Destructor
// TODO
Loader::~Loader() {

}

void Loader::read() {
  // TODO: take in filepath as input
  ifstream file (filename);
  string line;
  // reads file and store each row in a cell
  if (file.is_open()) {
    // get one line
    while (getline(file, line)) {
      vector<string> tmp;
      cout << line << endl;

      size_t pos = 0;
      // TODO: look into Boost library for more robust parsing
      while ((pos = line.find_first_of(',')) != string::npos) {

          string p = line.substr(0, pos);
          tmp.push_back(p);
          line = line.substr(pos + 1);
      }
      if (!line.empty()) {
        tmp.push_back(line);
      }
      // iterate through split vector to separate into coordinates and attributes

      // Construct cell abstraction from each row
      vector<int64_t> coords;
      vector<int64_t> attributes;
      vector<string>::iterator it = tmp.begin();

      // Coordinates
      for (int64_t i = 0; i < nDim; i++) {
        coords.push_back(atoi((*it).c_str()));
        ++it;
      }

      // Attributes
      for (int64_t i = 0; i < nAttr; i++) {
        attributes.push_back(atoi((*it).c_str()));
        ++it;
      }

      Cell * cell = new Cell(coords, attributes);
      // TODO: Maybe move z order calculation elsewhere...
      uint64_t morton = this->mortonEncode2D(coords.at(0), coords.at(1));
      cell->setMortonCode(morton);

      // Calculate tileID
      // TODO: generalize to rectangles

      string id = string("");
      vector<int64_t>::iterator itc = cell->coords.begin();

      while (itc != cell->coords.end()) {
        id += "-" + string(std::to_string(*itc / stride));
        itc++;
      }

      cell->setTileID(id);

      this->cells.push_back(cell);

    }

  }
  else {
    cout << "error reading csv file" << endl;
  }

  cout << "end read" << endl;

}

bool sortByMorton(Cell *c1, Cell *c2) {
  return (c1->getMortonCode() < c2->getMortonCode());
}

// TODO: implement own sorting with merge sort
void Loader::sort() {
  std::sort(this->cells.begin(), this->cells.end(), sortByMorton);
}


void Loader::tile() {

  // Hashmap of filename for attribute to its contents
  // One filename per attribute/tile combination
  map<string, vector<string>> attrMap;

  // divide array into stride-length squares
  // assume array is shifted to start at (0,0)
  vector<Cell *>::iterator it  = this->cells.begin();
  while (it != this->cells.end()) {
    string fileCoords = "tile-coords" + (*it)->getTileID() + ".dat";
    ofstream tilefile;
    // std::fstream::app = append mode
    tilefile.open(fileCoords, std::fstream::app);
    vector<int64_t>::iterator it1 = (*it)->coords.begin();
    while (it1 != (*it)->coords.end()) {
      tilefile << (*it1) << " ";
      it1++;
    }
    tilefile << endl;


    vector<int64_t>::iterator it2 = (*it)->attributes.begin();
    int attrCounter = 0;
    while (it2 != (*it)->attributes.end()) {
      string filename = "tile-attrs[" + to_string(attrCounter) + "]" + (*it)->getTileID() + ".dat";
      attrMap[filename].push_back(to_string(*it2));
      it2++;
      attrCounter++;
    }

    // Next cell
    it++;
  }

  // Write attributes to corresponding files
  for (map<string, vector<string>>::iterator it = attrMap.begin(); it != attrMap.end(); ++it) {
    string key = it->first;
    vector<string> contents = attrMap[key];

    ofstream attrfile;
    attrfile.open(key, std::fstream::app);
    for (vector<string>::iterator itv = contents.begin(); itv != contents.end(); ++itv) {
      attrfile << (*itv) << endl;
    }
  }

  // TODO: close all files
}


// Private functions

// TODO: fix for negative numbers, shift the ranges...
//       use shiftCoord helper
// http://www-graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
// TODO: only works for 32 bit x, y
uint64_t Loader::mortonEncode2D(uint64_t x, uint64_t y) {


  x = (x | (x << 16)) & 0x0000FFFF0000FFFF;
  x = (x | (x << 8 )) & 0x00FF00FF00FF00FF;
  x = (x | (x << 4 )) & 0x0F0F0F0F0F0F0F0F;
  x = (x | (x << 2 )) & 0x3333333333333333;
  x = (x | (x << 1 )) & 0x5555555555555555;

  y = (y | (y << 16)) & 0x0000FFFF0000FFFF;
  y = (y | (y << 8 )) & 0x00FF00FF00FF00FF;
  y = (y | (y << 4 )) & 0x0F0F0F0F0F0F0F0F;
  y = (y | (y << 2 )) & 0x3333333333333333;
  y = (y | (y << 1 )) & 0x5555555555555555;

  return x | (y << 1);
}

// Morton ordering algorithm needs nonegative numbers
uint64_t Loader::shiftCoord(int64_t coord, int64_t min) {
  if (min < 0) {
    return (uint64_t) (-1 * min + coord);
  }
  return (uint64_t) coord;
}
