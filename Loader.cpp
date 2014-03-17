#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <cmath>
#include <bitset>
#include <algorithm>
#include "Loader.h"

using namespace std;

// Constructor
Loader::Loader(int64_t nDim, vector<int64_t> ranges, int64_t nAttr, int stride) {
  this->nDim = nDim;
  this->ranges = ranges;
  this->nAttr = nAttr;
  this->stride = stride;
}

// Destructor
Loader::~Loader() {

}

void Loader::read() {
  // TODO: take in filepath as input
  ifstream file ("data/tiny.csv");
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

      // construct cell abstraction from each row
      vector<int64_t> coords;
      vector<int64_t> attributes;
      vector<string>::iterator it = tmp.begin();
      for (int64_t i = 0; i < nDim; i++) {
        //cout << "nDim: " << i << endl;
        coords.push_back(atoi((*it).c_str()));
        ++it;
      }

      for (int64_t i = 0; i < nAttr; i++) {

        //cout << "nAttr " << i << endl;
        attributes.push_back(atoi((*it).c_str()));
        ++it;
      }

      Cell * cell = new Cell(coords, attributes);
      // TODO: Maybe move z order calculation elsewhere...
      uint64_t morton = this->mortonEncode2D(coords.at(0), coords.at(1));
      cell->setMortonCode(morton);
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

}


void Loader::store() {

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

// Main function
// TODO: move somewhere else
int main(int argc, char *argv[]) {
  int64_t nDim = 2;
  int64_t nAttribute = 1;
  int stride = 20;

  // TODO: Read this from config file later
  vector<int64_t> ranges;
  ranges.push_back(0);
  ranges.push_back(100);
  ranges.push_back(0);
  ranges.push_back(100);

  Loader *loader = new Loader(nDim, ranges, nAttribute, 20);
  std::cout << "hi" << std::endl;
  std::cout << loader->nDim << std::endl;


  int test[10] = {0,1,2,3,4,5,6,7,8,9};
  int limit = 5;
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      uint64_t z = loader->mortonEncode2D(i, j);
      cout << "i: " << i << " j: " << j << " z: " << z << endl;
      //printf("i: %d %s, j: %d %s, z %d %d", i, std::bitset<16>(i).to_string(), j, std::bitset<16>(j).to_string(), z, std::bitset<16>(z).to_string());
    }
  }

/*
 loader->read();
  loader->sort();

  vector<Cell *>::iterator it = loader->cells.begin();
  while (it != loader->cells.end()) {
    cout << "x: " << (*it)->coords.at(0) << " y: " << (*it)->coords.at(1) << " morton: " << (*it)->getMortonCode() << endl;
    ++it;
  }
*/
  return 0;
}
