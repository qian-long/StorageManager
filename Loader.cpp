#include <iostream>
#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <cstdlib>
#include <string>
#include <set>
#include <cstring>
#include <cmath>
#include <bitset>
#include <algorithm>
#include <map>
#include <climits>
#include <ctime>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/time.h>
#include "Debug.h"
#include "Loader.h"

//#define TILENAME_TEMPLATE tile-%s-.dat
// fails for 1000000 and above, not enough memory??
//#define LIMIT 1048576 // 1 MB
#define LIMIT 100000000 // 100 MB

using namespace std;

// Constructor
Loader::Loader(string filename, int64_t nDim, vector<int64_t> ranges, int64_t nAttr) {
  this->filename = filename;
  this->nDim = nDim;
  this->ranges = ranges;
  this->nAttr = nAttr;
  this->arrayname = removeExtension(getFileName(filename));

  //this->stride = stride;
  //this->tileMemLimit = memLimit;
}

// Destructor
// TODO
Loader::~Loader() {

}

void Loader::loadl(int stride) {
  string outdir = "output-fl-" + arrayname;

  dbgmsg("Creating output dir: " + outdir);
  // Create output directory
  // TODO: add better error handling
  if (mkdir(outdir.c_str(), S_IRWXU) != 0) {
    perror("Error creating loaderl output directory");
    return;
  }


  string outpath = getFilePath(outdir, arrayname);
  ifstream infile(this->filename);
  string line;
  std::stringstream ss;
  ofstream outfile;
  string tmpname = outpath + ".tmp";
  outfile.open(tmpname, std::fstream::app);

  // Used for creating index file of non-empty tile IDs
  set<string> tileIDset;

  cout << "Creating tmp file" << endl;
  struct timeval tim;
  gettimeofday(&tim, NULL);
  double t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
  clock_t begin = clock();

  // TODO figure out what is happening with read buffer
  // Step 1: append tileid to the end of every line
  if (infile.is_open()) {
    while (getline(infile, line)) {
      string tileid = Loader::getTileID(line, stride);
      tileIDset.insert(tileid);
      outfile << line << "," << tileid << endl;
    }
  }

  infile.close();
  outfile.close();

  clock_t end = clock();
  gettimeofday(&tim, NULL);
  double t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
  double elapsed_cpu_secs = double(end - begin) / CLOCKS_PER_SEC;
  double elapsed_real_secs = t2 - t1;


  cout << "Elapsed CPU time in seconds: " << elapsed_cpu_secs << endl;
  cout << "Elapsed real time in seconds: " << elapsed_real_secs << endl;
  cout << "Calculated disk I/O time: " << elapsed_real_secs - elapsed_cpu_secs << endl << endl;



  cout << "External linux sort" << endl;
  gettimeofday(&tim, NULL);
  begin = clock();
  t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
 
  // Step 2, external sort
  int tileIDCol = this->nDim + this->nAttr + 1;
  string sortedfile = outpath + ".sorted";
  // TODO: this is incredibly unsecure...
  string cmd = "sort -t, -k" + std::to_string(tileIDCol) + " " + tmpname + " -o " + sortedfile;
  dbgmsg("cmd: " + cmd);
  std::system(cmd.c_str());

  // removes first temp file
  if (remove(tmpname.c_str()) != 0 ) {
    perror( "Error deleting tmp file");
  }

  // Create index file for the indexer
  Loader::createIndexFile(outdir, &tileIDset);

  end = clock();
  gettimeofday(&tim, NULL);
  t2 = tim.tv_sec+(tim.tv_usec/1000000.0);

  elapsed_cpu_secs = double(end - begin) / CLOCKS_PER_SEC;
  elapsed_real_secs = t2 - t1;
  cout << "Elapsed cpu time in seconds: " << elapsed_cpu_secs << endl;
  cout << "Elapsed real time in seconds: " << elapsed_real_secs << endl;
  cout << "Calculated disk I/O time: " << elapsed_real_secs - elapsed_cpu_secs << endl << endl;



  // Step 3. Divide sorted file into fixed logical tiles
  Loader::tilel(outdir, sortedfile, stride);

}

void Loader::tilel(string outdir, string sortedfile, int stride) {
  cout << "Tiling Fixed Logical tiles..." << endl;
  struct timeval tim;
  gettimeofday(&tim, NULL);
  double t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
  clock_t begin = clock();


  ifstream infile(sortedfile);

  string line;
  string currentTileID;

  // Hashmap of filename for attribute to its contents
  // One filename per attribute/tile combination
  map<string, string> attrBufMap;
  stringstream coordBuf; // use same buffer for all lines
  uint64_t usedMem = 0;
  bool veryfirst = true;
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
        coords.push_back(coord);
        ++it;
      }

      // Attributes
      for (int i = 0; i < nAttr; i++) {
        int64_t attr = (int64_t)strtoll((*it).c_str(), NULL, 10);
        attributes.push_back(attr);
        ++it;
      }

      if (veryfirst) {
        currentTileID = *it;
        veryfirst = false;
      }

      // write to disk when tileid changes
      if (currentTileID.compare(*it) != 0) {
        // new tile
        dbgmsg("NEW TILE, flushing to disk");
        dbgmsg("currentTileID: " + currentTileID);
        dbgmsg("*it: " + *it);
        Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, currentTileID, outdir);
        // assign new tileid
        currentTileID = *it;
      }
      else {
        currentTileID = *it;
        if (usedMem > LIMIT) {
          // flush to file if buffers are full
          dbgmsg("memory reached, flushing to disk");
          // flush buffers to disk
          Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, currentTileID, outdir);
          usedMem = 0;
        }
      }
      // write to coordbuf
      for (vector<int64_t>::iterator itc = coords.begin(); itc != coords.end(); ++itc) {
        int64_t coord = *itc;
        // Serializing data into 8 bytes
        coordBuf.write((char *)(&coord), 8);
      }

      // *it points to tileid
      // write to attribute buffers
      int attrCounter = 0;
      for (vector<int64_t>::iterator ita = attributes.begin(); ita != attributes.end(); ++ita) {

        string attrfilename = getFilePath(outdir, "tile-attrs[" + to_string(attrCounter) + "]-" + (*it) + ".dat");
        int64_t attr = *ita;
        // Serializing data into 8 bytes
        attrBufMap[attrfilename].append((char *)(&attr), 8);
        ++attrCounter;
      }
      usedMem += (nDim + nAttr) * 8;

    }

    // Final flush
    dbgmsg("FINAL FLUSH");
    Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, currentTileID, outdir);
  }

  clock_t end = clock();
  gettimeofday(&tim, NULL);
  double t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
  double elapsed_cpu_secs = double(end - begin) / CLOCKS_PER_SEC;
  double elapsed_real_secs = t2 - t1;


  cout << "Elapsed CPU time in seconds: " << elapsed_cpu_secs << endl;
  cout << "Elapsed real time in seconds: " << elapsed_real_secs << endl;
  cout << "Calculated disk I/O time: " << elapsed_real_secs - elapsed_cpu_secs << endl << endl;


  cout << "COMPRESSING" << endl;
  gettimeofday(&tim, NULL);
  begin = clock();
  t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
 

  // Create compressed binary attributes tiles
  
  for (map<string, string>::iterator it = attrBufMap.begin(); it != attrBufMap.end(); ++it) {
    string key = it->first;
    Loader::compressTile(key);
  }
  
  end = clock();
  gettimeofday(&tim, NULL);
  t2 = tim.tv_sec+(tim.tv_usec/1000000.0);

  elapsed_cpu_secs = double(end - begin) / CLOCKS_PER_SEC;
  elapsed_real_secs = t2 - t1;
  cout << "Elapsed cpu time in seconds: " << elapsed_cpu_secs << endl;
  cout << "Elapsed real time in seconds: " << elapsed_real_secs << endl;
  cout << "Calculated disk I/O sort time: " << elapsed_real_secs - elapsed_cpu_secs << endl << endl;

}

void Loader::writeTileBufsToDisk(map<string, string> * attrBufMap, stringstream * coordBuf, string tileid, string outdir) {
  dbgmsg("writing Tile bufs to disk");
  string fileCoords = getFilePath(outdir, "tile-coords-" + tileid + ".dat");
  ofstream coordFile;
  coordFile.open(fileCoords, std::fstream::app);
  dbgmsg("writing to fileCoords: " + fileCoords);
  coordFile << coordBuf->str();

  // reset coordBuf
  coordBuf->str(std::string());

  // Write attribute buffers to corresponding files
  for (map<string, string>::iterator it = attrBufMap->begin(); it != attrBufMap->end(); ++it) {
    string key = it->first;
    ofstream attrfile;
    attrfile.open(key, std::fstream::app);
    // TODO:figure out erasing keys
    if ((*attrBufMap)[key].size() > 0) {
      dbgmsg("writing to key: " + key);
      attrfile << (*attrBufMap)[key];

      // reset attfBuf
      (*attrBufMap)[key].clear();
      attrfile.close();
    }
  }

  coordFile.close();
}


// attributes take up 8 bytes
// output format: 8 bytes for occurrence, 8 bytes for value
// Run Length Encoding Implementation
void Loader::compressTile(string& attrfilename) {
  dbgmsg("Compressing tile: " + attrfilename);
  string outdir = getDirPath(attrfilename);
  FILE * filep;
  filep = fopen(attrfilename.c_str(), "r");
  if (filep == NULL) {
    perror("file doesn't exist");
  }
  // Read exact int64_t boundaries
  uint64_t limit = (LIMIT/8 + 1) * 8;
  
  char * buffer = new char[limit];
  stringstream outBuf;

  // holds current number for occurrence counting
  // read first number
  bool veryfirst = true;
  int64_t currentNum = 0;
  int64_t occurrence = 1;
  int64_t readNum = 0;

  ofstream outFile;
  string outname = getFilePath(getDirPath(attrfilename), "rle-" + getFileName(attrfilename));
  dbgmsg("outname: " + outname);
  outFile.open(outname, std::fstream::app);
  while (uint64_t readsize = fread((char *)buffer, 1, limit, filep)) {
    // process the block
    // Little endian

    // start at second int64
    readNum = *((int64_t *)(buffer));
    if (!veryfirst) {
      if (currentNum != readNum) {
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
  if (currentNum == readNum) {
    // write to out buffer
    outBuf.write((char *)(&occurrence), 8);
    outBuf.write((char *)(&currentNum), 8);
  }

  // Write to output file
  outFile << outBuf.str();
  // reset buffer
  outBuf.str(std::string());
  outFile.close();
  // deallocate buffer
  delete [] buffer;
}


string Loader::getTileID(string line, int stride) {
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

string Loader::getSortKey(string line) {
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
    output += (*it) + "-";
    ++it;
  }

  // last one
  output += (*it);
  return output;

}
// Write each id to a new line
// TODO: make more compact later
void Loader::createIndexFile(string dirpath, set<string> * tileIDs) {
  ofstream out;
  // TODO: change name later
  string outfile = getFilePath(dirpath, "index.txt");
  out.open(outfile);
  for (set<string>::iterator it = tileIDs->begin(); it != tileIDs->end(); ++it) {
    out << *it << endl;
  }
  out.close();
}

bool sortByMorton(Cell *c1, Cell *c2) {
  return (c1->getMortonCode() < c2->getMortonCode());
}

// Loading for fixed physical coordinate tiles
void Loader::loadp(uint64_t tileMemLimit) {
  string outdir = "output-FP-" + arrayname;
  dbgmsg("Creating output dir: " + outdir);

  // Create output directory
  // TODO: add better error handling
  if (mkdir(outdir.c_str(), S_IRWXU) != 0) {
    perror("Error creating loadp output directory");
    return;
  }

  ifstream infile(this->filename);
  string line;
  stringstream ss;

  ofstream tmpfile;
  string tmpname = getFilePath(outdir, arrayname + ".tmp");
  tmpfile.open(tmpname, std::fstream::app);
  cout << "Creating tmp file" << endl;

  struct timeval tim;
  gettimeofday(&tim, NULL);
  double t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
  clock_t begin = clock();

  // Step 1: append sort key to the end of every line
  if (infile.is_open()) {
    while (getline(infile, line)) {
      string sortkey = Loader::getSortKey(line);
      tmpfile << line << "," << sortkey << endl;
    }
  }

  infile.close();
  tmpfile.close();

  clock_t end = clock();
  gettimeofday(&tim, NULL);
  double t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
  double elapsed_cpu_secs = double(end - begin) / CLOCKS_PER_SEC;
  double elapsed_real_secs = t2 - t1;


  cout << "Elapsed CPU time in seconds: " << elapsed_cpu_secs << endl;
  cout << "Elapsed real time in seconds: " << elapsed_real_secs << endl;
  cout << "Calculated disk I/O time: " << elapsed_real_secs - elapsed_cpu_secs << endl << endl;



  // Step 2: external sort
  cout << "External linux sort" << endl;
  gettimeofday(&tim, NULL);
  begin = clock();
  t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
 
  int sortCol = this->nDim + this->nAttr + 1;
  string sortedfile = getFilePath(outdir, arrayname + ".sorted");
  // TODO: this is incredibly unsecure...
  // The -k m,n option lets you sort on a particular field (start at m, end at n):
  // ex) sort -gt, -k1,1 -k2,2 small.csv
  //string cmd = "sort -gt, -k1," + to_string(nDim) + " " + tmpname + " -o " + sortedfile;
  string cmd = "sort -gt,";
  for (int i = 1; i <= nDim; ++i) {
    cmd += " -k" + to_string(i) + "," + to_string(i) + " ";
  }
  cmd += tmpname + " -o " + sortedfile;
  std::system(cmd.c_str());

  // removes first temp file
  if (remove(tmpname.c_str()) != 0 ) {
    perror( "Error deleting file" );
  }

  end = clock();
  gettimeofday(&tim, NULL);
  t2 = tim.tv_sec+(tim.tv_usec/1000000.0);

  elapsed_cpu_secs = double(end - begin) / CLOCKS_PER_SEC;
  elapsed_real_secs = t2 - t1;

  cout << "Elapsed cpu time in seconds: " << elapsed_cpu_secs << endl;
  cout << "Elapsed real time in seconds: " << elapsed_real_secs << endl;
  cout << "Calculated external sort time: " << elapsed_real_secs - elapsed_cpu_secs << endl << endl;


  // Step 3: divide into fixed physical tiles and create index file
  Loader::tilep(outdir, sortedfile, tileMemLimit);
}

void Loader::tilep(string outdir, string sortedfname, uint64_t tileMemLimit) {
  cout << "Tiling..." << endl;
  struct timeval tim;
  gettimeofday(&tim, NULL);
  double t1 = tim.tv_sec+(tim.tv_usec/1000000.0);
  clock_t begin = clock();

  ifstream infile(sortedfname);
  string line;
  int tileIDCounter = 0;
  uint64_t curTileSize = 0;
  uint64_t usedMem = 0; // for controlling when to write to disk

  // Hashmap of filename for attribute to its contents
  // One filename per attribute/tile combination
  map<string, string> attrBufMap;
  stringstream coordBuf; // use same buffer for all lines
  stringstream indexBuf; // used for index
  vector<int64_t> coords;
  vector<int64_t> attributes;

  vector<int64_t> minCoords;
  vector<int64_t> maxCoords;
  for (int i = 0; i < nDim; ++i) {
    minCoords.push_back(LLONG_MAX);
    maxCoords.push_back(LLONG_MIN);
  }

  if (infile.is_open()) {
    while (getline(infile, line)) {
      // clear coords and attributes vectors
      // TODO: only coords needs to be in outer scope
      coords.clear();
      attributes.clear();

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
      vector<string>::iterator it = lineElements.begin();

      // Coordinates
      for (int i = 0; i < nDim; i++) {
        int64_t coord = (int64_t)strtoll((*it).c_str(), NULL, 10);
        coords.push_back(coord);
        ++it;
      }

      // Attributes
      for (int i = 0; i < nAttr; i++) {
        int64_t attr = (int64_t)strtoll((*it).c_str(), NULL, 10);
        attributes.push_back(attr);

        string attrfname = getFilePath(outdir, "tile-attrs[" + to_string(i) + "]-" + to_string(tileIDCounter) + "-fp.dat");
        // Serializing data into 8 bytes
        attrBufMap[attrfname].append((char *)(&attr), 8);
        ++it;
      }

      // write to coordbuf
      for (vector<int64_t>::iterator itc = coords.begin(); itc != coords.end(); ++itc) {
        int64_t coord = *itc;
        // Serializing data into 8 bytes
        coordBuf.write((char *)(&coord), 8);
      }
      curTileSize += 8 * nDim;
      usedMem += 8 * nDim;

      // get min and max coords
      vector<int64_t>::iterator itmin = minCoords.begin();
      vector<int64_t>::iterator itmax = maxCoords.begin();
      vector<int64_t>::iterator itcd = coords.begin();
      for (int i = 0; i < nDim; ++i) {
        if (*(itcd + i) < *(itmin + i)) {
          *(itmin + i) = *(itcd + i);
        }
        if (*(itcd + i) > *(itmax + i)) {
          *(itmax + i) = *(itcd + i);
        }
      }

      if (curTileSize >= tileMemLimit) {

        dbgmsg("Reached tileMemLimit: " + to_string(curTileSize));

        // Writing min and max bounding box to index
        for (vector<int64_t>::iterator itm = minCoords.begin(); itm != minCoords.end(); ++itm) {
          indexBuf << *itm << ",";
        }
        for (vector<int64_t>::iterator itm = maxCoords.begin(); itm != maxCoords.end(); ++itm) {
          indexBuf << *itm << ",";
        }
        indexBuf << tileIDCounter << endl;
        // write to disk
        Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, to_string(tileIDCounter) + "-fp", outdir);
        // Increment tile id
        tileIDCounter++;

        // reset variables
        curTileSize = 0;
        minCoords.clear();
        maxCoords.clear();
        for (int i = 0; i < nDim; ++i) {
          minCoords.push_back(LLONG_MAX);
          maxCoords.push_back(LLONG_MIN);
        }
      }

    }
  }

  // Write final tile to disk
  if (coordBuf.str().size() > 0) {
    Loader::writeTileBufsToDisk(&attrBufMap, &coordBuf, to_string(tileIDCounter) + "-fp", outdir);
  // Write index line of last tile
    for (vector<int64_t>::iterator itm = minCoords.begin(); itm != minCoords.end(); ++itm) {
      indexBuf << *itm << ",";
    }
    for (vector<int64_t>::iterator itm = maxCoords.begin(); itm != maxCoords.end(); ++itm) {
      indexBuf << *itm << ",";
    }
    indexBuf << tileIDCounter << endl;
  }

  // Write index to disk
  ofstream indexfile;
  indexfile.open(getFilePath(outdir, "index.txt"));
  indexfile << indexBuf.str();
  indexfile.close();

  clock_t end = clock();
  gettimeofday(&tim, NULL);
  double t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
  double elapsed_cpu_secs = double(end - begin) / CLOCKS_PER_SEC;
  double elapsed_real_secs = t2 - t1;


  cout << "Elapsed CPU time in seconds: " << elapsed_cpu_secs << endl;
  cout << "Elapsed real time in seconds: " << elapsed_real_secs << endl;
  cout << "Calculated disk I/O time: " << elapsed_real_secs - elapsed_cpu_secs << endl << endl;

  cout << "Compressing..." << endl;

  gettimeofday(&tim, NULL);
  begin = clock();
  t1 = tim.tv_sec+(tim.tv_usec/1000000.0);

  // Compress each attribute tile
  for (map<string, string>::iterator it = attrBufMap.begin(); it != attrBufMap.end(); ++it) {
    string key = it->first;
    //Loader::compressTile(key.c_str());

    Loader::compressTile(key);
  }

  end = clock();
  gettimeofday(&tim, NULL);
  t2 = tim.tv_sec+(tim.tv_usec/1000000.0);
  elapsed_cpu_secs = double(end - begin) / CLOCKS_PER_SEC;
  elapsed_real_secs = t2 - t1;

  cout << "Elapsed CPU time in seconds: " << elapsed_cpu_secs << endl;
  cout << "Elapsed real time in seconds: " << elapsed_real_secs << endl;
  cout << "Calculated disk I/O: " << elapsed_real_secs - elapsed_cpu_secs << endl << endl;


}

// Private functions
// Returns filename from path
// TODO: use Boost library instead?
string Loader::getFileName(const string& s) {

  char sep = '/';
  size_t i = s.rfind(sep, s.length());
  if (i != string::npos) {
    return(s.substr(i+1, s.length() - i));
  }

  return s;
}

string Loader::removeExtension(const string& filename) {
  size_t lastdot = filename.find_last_of(".");
  if (lastdot == std::string::npos) {
    return filename;
  }
  return filename.substr(0, lastdot);
}

string Loader::getFilePath(const string& dirpath, const string& filename) {
  return dirpath + "/" + filename;
}

string Loader::getDirPath(const string& filepath) {
  char sep = '/';
  size_t i = filepath.rfind(sep, filepath.length());
  if (i != string::npos) {
    return filepath.substr(0, i);
  }

  return filepath;
}

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
