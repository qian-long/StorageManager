#include <stdlib.h>
#include <iostream>
#include <map>
#include "Loader.h"
#include "Indexer.h"

using namespace std;

// Main function
int main(int argc, char *argv[]) {

  // TODO: Read this from config file (probably postgres) later
  int64_t nDim = 2;
  int64_t nAttribute = 1;
  int stride = 2;

  vector<int64_t> ranges;
  ranges.push_back(0);
  ranges.push_back(10);
  ranges.push_back(0);
  ranges.push_back(10);
  string csvfile = "data/tiny.csv";

  Loader *loader = new Loader(csvfile, nDim, ranges, nAttribute, stride);
  cout << "Hello, world" << endl;
  cout << loader->nDim << endl;


  cout << "loader->read()" << endl;
  loader->read();
  cout << "loader->sort()" << endl;
  loader->sort();
  cout << "loader->tile()" << endl;
  loader->tile();

  Indexer *indexer = new Indexer(nDim, ranges, nAttribute, stride);
  map<int, vector<string>> mymap = indexer->attrToTileMap;
  for (map<int, vector<string>>::iterator it = mymap.begin(); it != mymap.end(); ++it) {
    int key = it->first;
    vector<string> contents = mymap[key];
    cout << "key: " << key << endl;
    for (vector<string>::iterator itc = contents.begin(); itc != contents.end(); ++itc) {
      cout << *itc << endl;
    }
  }

  return 0;
}
