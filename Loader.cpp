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
  cout << "final dumping" << endl;
  cout << ss.str();
  outfile << ss.str();
  // clears string stream buffer
  ss.str(std::string());

  infile.close();
  outfile.close();

  // Step 2, external sort
  int tileIDCol = this->nDim + this->nAttr + 1;
  string tmpfile2 = this->filename + ".sorted";
  // TODO: this is incredibly unsecure...
  string cmd = "sort -t, -k" + std::to_string(tileIDCol) + " " + tmpfile + " -o " + tmpfile2;
  cout << "cmd: " << cmd;
  std::system(cmd.c_str());

  // removes first temp file
  if (remove(tmpfile.c_str()) != 0 ) {
    perror( "Error deleting file" );
  }
}

void Loader::tile() {
  string sortedfile = this->filename + ".sorted";
  ifstream infile(sortedfile);

  string line;
  string currentTileID;

  // Hashmap of filename for attribute to its contents
  // One filename per attribute/tile combination
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
        // writing to coordBuf stringstream
        coordBuf.write((char *)(&coord), 8);

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

      // *it points to tileid
      // write to attribute buffers
      int attrCounter = 0;
      for (vector<int64_t>::iterator ita = attributes.begin(); ita != attributes.end(); ++ita) {

        string attrfilename = "tile-attrs[" + to_string(attrCounter) + "]-" + (*it) + ".dat";
        int64_t attr = *ita;
        // Serializing data into 8 bytes
        attrBufMap[attrfilename].append((char *)(&attr), 8);
        ++attrCounter;
      }
      usedMem += (nDim + nAttr) * 8;

      // write to disk when tileid changes
      if (currentTileID.compare(*it) != 0) {
        // new tile

        Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, *it);
        // assign new tileid
        currentTileID = *it;
      }
      else {
        currentTileID = *it;
        if (usedMem > LIMIT) {
          // flush to file if buffers are full
          cout << "memory reached, flushing to disk" << endl;
          // flush buffers to disk
          Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, currentTileID);
        }
      }
    }

  }
}

void Loader::writeTileBufsToDisk(map<string, string> * attrBufMap, stringstream * coordBuf, string tileid) {
  cout << "writing Tile bufs to disk" << endl;
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

  coordFile.close();
}


// attributes take up 8 bytes
// output format: 8 bytes for occurrence, 8 bytes for value
// Uses Run Length Encoding
void Loader::compressTile(const char * filename) {
  FILE * filep;
  filep = fopen(filename, "r");
  if (filep == NULL) {
    perror("file doesn't exist");
  }
  // Read exact int64_t boundaries
  uint64_t limit = (LIMIT/8) * 8;
  char buffer[LIMIT];
  stringstream outBuf;

  // holds current number for occurrence counting
  // read first number
  bool veryfirst = true;
  int64_t currentNum = 0;
  int64_t occurrence = 1;
  int64_t readNum = 0;

  ofstream outFile;
  outFile.open("rle-" + string(filename), std::fstream::app);
  while (uint64_t readsize = fread((char *)buffer, 1, limit, filep)) {
    // process the block
    // Little endian

    // start at second int64
    readNum = *((int64_t *)(buffer));
    if (!veryfirst) {
      if (currentNum != readNum) {

        //cout << "writing occurrence: " << occurrence << endl;
        //cout << "writing currentNum: " << currentNum << endl;
        outBuf.write((char *)(&occurrence), 8);
        outBuf.write((char *)(&currentNum), 8);
        currentNum = readNum;
        occurrence = 1;
      }
      else {
          occurrence++;
      }
    }
    else {
      veryfirst = false;
      currentNum = readNum;
    }

    for (uint64_t i = 8; i < readsize; i = i + 8) {
      readNum = *((int64_t *)(buffer + i));

      if (currentNum != readNum) {
        //cout << "writing occurrence: " << occurrence << endl;
        //cout << "writing currentNum: " << currentNum << endl;
        outBuf.write((char *)(&occurrence), 8);
        outBuf.write((char *)(&currentNum), 8);
        currentNum = readNum;
        occurrence = 1;
      }
      else {
        occurrence++;
      }
    }

    // Write to output file
    outFile << outBuf.str();
    // reset buffer
    outBuf.str(std::string());
  }

  // compare last int64
  //cout << "last number: " << readNum << endl;
  if (currentNum == readNum) {
    // write to out buffer
    //cout << "writing occurrence: " << occurrence << endl;
    //cout << "writing currentNum: " << currentNum << endl;
    outBuf.write((char *)(&occurrence), 8);
    outBuf.write((char *)(&currentNum), 8);
  }

  // Write to output file
  outFile << outBuf.str();
  // reset buffer
  outBuf.str(std::string());
  outFile.close();
}


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
    int dimID = strtoll((*it).c_str(), NULL, 10) / stride;
    output += std::to_string(dimID) + "-";
    ++it;
  }

  int last = strtoll((*it).c_str(), NULL, 10) / stride;
  output += std::to_string(last);
  return output;
}

bool sortByMorton(Cell *c1, Cell *c2) {
  return (c1->getMortonCode() < c2->getMortonCode());
}


// Private functions

// TODO: fix for negative numbers, shift the ranges...
//       use shiftCoord helper
// http://www-graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
// TODO: only works for 32 bit x, y
// Unused
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
// Unused
uint64_t Loader::shiftCoord(int64_t coord, int64_t min) {
  if (min < 0) {
    return (uint64_t) (-1 * min + coord);
  }
  return (uint64_t) coord;
}
