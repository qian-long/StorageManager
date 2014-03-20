#include <stdint.h>
#include<string>
#include "Cell.h"

using std::vector;
using std::string;

Cell::Cell(vector<int64_t> coords, vector<int64_t> attributes) {
  this->coords = coords;
  this->attributes = attributes;
  this->mortonCode = 0;
  this->tileID = "";
}

Cell::~Cell() {

}

void Cell::setMortonCode(uint64_t num) {
  this->mortonCode = num;
}

void Cell::setTileID(string id) {
  this->tileID = id;
}

uint64_t Cell::getMortonCode() {
  return this->mortonCode;
}

string Cell::getTileID() {
  return this->tileID;
}

bool Cell::operator=(Cell * other) {
  return this->mortonCode == other->mortonCode;
}

