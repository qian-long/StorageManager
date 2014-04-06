#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <cstring>
#include <cmath>
#include <bitset>
#include <algorithm>
#include <map>
#include "Loader.h"

//#define TILENAME_TEMPLATE tile-%s-.dat
// fails for 10000000 and above, not enough memory??
//#define LIMIT 1048576 // 1 MB
#define LIMIT 50

using namespace std;

// Constructor
Loader::Loader(string filename, int64_t nDim, vector<int64_t> ranges, int64_t nAttr, int stride, uint64_t mem_limit) {
  this->filename = filename;
  this->nDim = nDim;
  this->ranges = ranges;
  this->nAttr = nAttr;
  this->stride = stride;
  this->mem_limit = mem_limit;
}

// Destructor
// TODO
Loader::~Loader() {

}

void Loader::load() {

  ifstream infile(this->filename);
  string line;
  std::stringstream ss;
  ofstream outfile;
  string tmpfile = this->filename + ".tmp";
  outfile.open(tmpfile, std::fstream::app);
  // TODO figure out what is happening with read buffer
  if (infile.is_open()) {
    while (getline(infile, line)) {
      ss << line << "," << Loader::getTileID(line) << endl;
      if (ss.str().size() >= LIMIT) {
        cout << "dumping" << endl;
        cout << ss.str();
        outfile << ss.str();
        // clears string stream buffer
        ss.str(std::string());
      }
    }
  }
  infile.close();
  outfile.close();

  // Step 2, external sort
  int tileIDCol = this->nDim + this->nAttr;
  string tmpfile2 = this->filename + ".sorted";
  // TODO: this is incredibly unsecure...
  string cmd = "sort -k" + std::to_string(tileIDCol) + " " + tmpfile + " -o " + tmpfile2;
  std::system(cmd.c_str());

  // removes first temp file
  if (remove(tmpfile.c_str()) != 0 ) {
    perror( "Error deleting file" );
  }

}

void Loader::tile2() {
  string sortedfile = this->filename + ".sorted";
  ifstream infile(sortedfile);

  string line;
  string currentTileID;

  // Hashmap of filename for attribute to its contents
  // One filename per attribute/tile combination
  //map<string, stringstream> attrBufMap;
  map<string, string> attrBufMap;
  stringstream coordBuf; // use same buffer for all lines

  uint64_t usedMem = 0;

  if (infile.is_open()) {
    while (getline(infile, line)) {
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

      // iterate through split vector to separate into coordinates and attributes

      // Construct cell abstraction from each row
      vector<int64_t> coords;
      vector<int64_t> attributes;
      vector<string>::iterator it = lineElements.begin();

      // Coordinates
      for (int i = 0; i < nDim; i++) {
        int64_t coord = (int64_t)strtoll((*it).c_str(), NULL, 10);

        // Serializing data into 8 bytes
        char a[8];
        memcpy(a, &coord, 8);

        // writing to coordBuf stringstream
        coordBuf.write((char *) a, 8);
        // TODO: unused
        coords.push_back(atoi((*it).c_str()));

        ++it;
      }

      // Attributes
      for (int i = 0; i < nAttr; i++) {
        int64_t attr = (int64_t)strtoll((*it).c_str(), NULL, 10);
        attributes.push_back(attr);
        ++it;
      }

      // write to attribute buffers
      int attrCounter = 0;
      for (vector<int64_t>::iterator ita = attributes.begin(); ita != attributes.end(); ++ita) {

        string attrfilename = "tile-attrs[" + to_string(attrCounter) + "]-" + (*it) + ".dat";
        int64_t attr = *ita;
        // Serializing data into 8 bytes
        char a[8];
        memcpy(a, &attr, 8);
        attrBufMap[attrfilename].append(a, 8);
        ++attrCounter;
      }
      usedMem += (nDim + nAttr) * 8;

      // flush to file if buffers are full
      if (currentTileID.compare(*it) != 0) {
        // new tile
        // write current tile buffers to tile files

        if (currentTileID.compare("") != 0) {
          Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, currentTileID);
        }
        // assign new tileid
        currentTileID = *it;
      }
      else {
        currentTileID = *it;
        if (usedMem > LIMIT) {
          cout << "memory reached, flushing to disk" << endl;
          // flush buffers to disk
          Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, currentTileID);
        }
      }
    }

  }
}

void Loader::writeTileBufsToDisk(map<string, string> * attrBufMap, stringstream * coordBuf, string tileid) {
  string fileCoords = "tile-coords-" + tileid + ".dat";
  ofstream coordFile;
  coordFile.open(fileCoords, std::fstream::app);
  coordFile << coordBuf->str();

  // reset coordBuf
  coordBuf->str(std::string());

  // Write attribute buffers to corresponding files
  for (map<string, string>::iterator it = attrBufMap->begin(); it != attrBufMap->end(); ++it) {
    string key = it->first;

    ofstream attrfile;
    attrfile.open(key, std::fstream::app);
    attrfile << (*attrBufMap)[key];

    // reset attfBuf
    (*attrBufMap)[key].clear();
    attrfile.close();
  }

  // close all files
  coordFile.close();
}
void Loader::int64ToChar(char a[], int64_t n) {
  std::memcpy(a, &n, 8);
}

/*
void Loader::compress() {

}
*/

string Loader::getTileID(string line) {
  string output = string("");
  vector<string> tmp;

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

  vector<string>::iterator it = tmp.begin();
  for (int i = 0; i < nDim - 1; ++i) {
    int dimID = std::atoi((*it).c_str()) / stride;
    output += std::to_string(dimID) + "-";
    ++it;
  }

  output += std::to_string(std::atoi((*it).c_str()));
  return output;
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
