#include <vector>
#include <string>
#include "BoundingBox.h"
#include <iostream>
// Bounding Box

using namespace::std;
BoundingBox::BoundingBox(string line, int nDim) {
  this->nDim = nDim;
  this->minCoords = new vector<int64_t>();
  this->maxCoords = new vector<int64_t>();
  // Parse line
  vector<string> lineElements;
  size_t pos = 0;
  // TODO: look into Boost library for more robust parsing
  while ((pos = line.find_first_of(',')) != string::npos) {
    string p = line.substr(0, pos);
    lineElements.push_back(p);
    line = line.substr(pos + 1);
  }
  if (!line.empty()) {
    lineElements.push_back(line);
  }

  vector<string>::iterator it = lineElements.begin();
  for (int i = 0; i < nDim; ++i) {
    int64_t coord = (int64_t)strtoll((*it).c_str(), NULL, 10);
    minCoords->push_back(coord);
    ++it;
  }
  for (int i = 0; i < nDim; ++i) {
    int64_t coord = (int64_t)strtoll((*it).c_str(), NULL, 10);
    maxCoords->push_back(coord);
    ++it;
  }
  this->tileid = string(*it);
}

BoundingBox::~BoundingBox() {
  delete minCoords;
  delete maxCoords;
}
