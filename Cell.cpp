#include "Cell.h"

using std::vector;

Cell::Cell(vector<int> coords, vector<int> attributes) {
  this->coords = coords;
  this->attributes = attributes;
}

Cell::~Cell() {

}
