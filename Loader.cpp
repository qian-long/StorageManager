#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include "Loader.h"

using namespace std;

// Constructor
Loader::Loader(int nDim, vector<int> ranges, int nAttr) {
  this->nDim = nDim;
  this->ranges = ranges;
  this->nAttr = nAttr;
}

// Destructor
Loader::~Loader() {

}

void Loader::read() {
  ifstream file ("data/small.csv");
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
/*
      vector<string>::iterator it = tmp.begin();
      while (it != tmp.end()) {
        cout << *it << endl;
        ++it;
      }
*/

      // construct cell abstraction from each row
      vector<int> coords;
      vector<int> attributes;
      vector<string>::iterator it = tmp.begin();
      for (int i = 0; i < nDim; i++) {
        cout << "nDim: " << i << endl;
        coords.push_back(atoi((*it).c_str()));
        ++it;
      }

      for (int i = 0; i < nAttr; i++) {

        cout << "nAttr " << i << endl;
        attributes.push_back(atoi((*it).c_str()));
        ++it;
      }

      Cell * cell = new Cell(coords, attributes);
      this->cells.push_back(cell);
    }

  }
  else {
    cout << "error reading csv file" << endl;
  }

  vector<Cell *>::iterator it = this->cells.begin();
  while (it != cells.end()) {
    cout << (*it)->coords.size() << endl;
    ++it;
  }
  cout << "end read" << endl;

}

void Loader::sort() {

}


void Loader::tile() {

}


void Loader::store() {

}

// private functions
/*
void splitLine(string & line, string delim, vector<string> & values) {
    size_t pos = 0;
    while ((pos = line.find(delim, (pos + 1))) != string::npos) {
        string p = line.substr(0, pos);
        values.push_back(p);
        line = line.substr(pos + 1);
    }

    if (!line.empty()) {
        values.push_back(line);
    }
}
*/

// Main function
// TODO: move somewhere else
int main(int argc, char *argv[]) {
  int nDim = 2;
  int nAttribute = 1;
  // TODO: look for a better way to do this
  vector<int> ranges;
  ranges.push_back(0);
  ranges.push_back(100);
  ranges.push_back(0);
  ranges.push_back(100);

  Loader *loader = new Loader(nDim, ranges, nAttribute);
  std::cout << "hi" << std::endl;
  std::cout << loader->nDim << std::endl;

  loader->read();
  return 0;
}
