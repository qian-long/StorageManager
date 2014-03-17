#include <stdint.h>
#include "Cell.h"

using std::vector;

Cell::Cell(vector<int64_t> coords, vector<int64_t> attributes) {
  this->coords = coords;
  this->attributes = attributes;
  this->mortonCode = 0;
}

Cell::~Cell() {

}

void Cell::setMortonCode(uint64_t num) {
  this->mortonCode = num;
}

uint64_t Cell::getMortonCode() {
  return this->mortonCode;
}


bool Cell::operator=(Cell * other) {
  return this->mortonCode == other->mortonCode;
}

