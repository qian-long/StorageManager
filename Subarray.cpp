#include <iostream>
#include <fstream>
#include <sys/stat.h>
#include <sys/types.h>
#include <set>
#include "Subarray.h"

using namespace std;

Subarray::Subarray(string name, Indexer * indexer, vector<int64_t> * subranges, vector<int64_t> * ranges, int64_t stride) {
  this->name = name;
  this->indexer = indexer;
  this->subranges = subranges;
  this->ranges = ranges;
  this->stride = stride;
}

Subarray::~Subarray() {

}

void Subarray::execute() {

  // TODO: add better error handling
  // Create directory for new array
  if (mkdir(this->name.c_str(), S_IRWXU) != 0) {
    perror ("Directory exists");
    return;
  }

  vector<string> * tileids = indexer->tileids;
  set<string> inRangeTiles;

  // Maps tile id to lines that are in range
  map<string, set<int>> tileToLinesMap;

  for (vector<string>::iterator it = tileids->begin(); it != tileids->end(); ++it) {
    string coordTile = indexer->getCoordTileById(*it);
    ifstream coordFile(coordTile);
    string line;

    ofstream newCoordFile;
    string cfilename = this->name + "/" + coordTile;

    int linenum = 0;
    if (coordFile.is_open()) {
      while (getline(coordFile, line)) {
        vector<int64_t> coords;
        char buffer[line.length() + 1];
        line.copy(buffer, line.length(), 0);
        buffer[line.length()] = '\0';
        size_t pos = 0;
        // TODO: look into Boost library for more robust parsing
        // Split by space
        while ((pos = line.find_first_of(' ')) != string::npos) {

            string p = line.substr(0, pos);
            coords.push_back(atoi(p.c_str()));
            line = line.substr(pos + 1);
        }
        if (!line.empty()) {
          coords.push_back(atoi(line.c_str()));
        }

        if (Subarray::inRange(&coords)) {
          if (!newCoordFile.is_open()) {
            // open for appending
            newCoordFile.open(cfilename, std::fstream::app);
          }

          string lineCopy(buffer);
          newCoordFile << lineCopy << endl;

          // Keep track of corresponding lines and tiles ids
          //inRangeLines.push_back(linenum);
          inRangeTiles.insert(indexer->getTileIdByCoords(&coords));

          tileToLinesMap[indexer->getTileIdByCoords(&coords)].insert(linenum);
        }

        linenum++;
      }
    }
  }

  // Go through each attribute tile and write new tiles
  for(set<string>::iterator itt = inRangeTiles.begin(); itt != inRangeTiles.end(); ++itt) {
    vector<string> * attrTiles = indexer->getAllAttrTilesById(*itt);
    for (vector<string>::iterator ita = attrTiles->begin(); ita != attrTiles->end(); ++ita) {
      string attrTile = *ita;
      ifstream attrFile(attrTile);
      string line;

      // output stream
      ofstream newAttrFile;
      string afilename = this->name + "/" + attrTile;
      newAttrFile.open(afilename, std::fstream::app);

      int lineCounter = 0;
      if (attrFile.is_open()) {
        while (getline(attrFile, line)) {
          // Found a line that is in the subarray
          if (tileToLinesMap[*itt].find(lineCounter) != tileToLinesMap[*itt].end()) {
            newAttrFile << line << endl;
          }

          lineCounter++;
        }
      }
      attrFile.close();
      newAttrFile.close();
    }
  }
}

// Private Functions
bool Subarray::inRange(vector<int64_t> * coords) {
  vector<int64_t>::iterator itsub = this->subranges->begin();

  for (vector<int64_t>::iterator it = coords->begin(); it != coords->end(); ++it) {
    int64_t start = *(itsub++);
    int64_t end = *(itsub++);
    int64_t value = *it;

    if (value < start || value > end) {
      return false;
    }
  }

  return true;
}

// TODO: finish

/*
vector<string> Subarray::getInRangeWholeTiles() {
  vector<string> output;
  vector<int> splits;
  map<int, set<int>> splitsMap;
  vector<int64_t>::iterator itr = this->ranges->begin();


  for (int i = 0; i < nDim; ++i) {
    int64_t begin = *(itr++);
    int64_t end = *(itr++);
    int numSplits = (end - begin) / this->stride;

    // If not even split, add an extra one
    if ((end - begin) % this->stride != 0) {
      numSplits++;
    }
    splits.push_back(numSplits);
    for (int j = 0; j < numSplits; ++j) {
      splitsMap[nDim].push_back(j * numSplits);
    }
  }



}

vector<string> Subarray::getInRangePartialTiles() {

}
*/
