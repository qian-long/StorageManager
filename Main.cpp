#include <stdlib.h>
#include <iostream>
#include <map>
#include "Loader.h"
#include "Indexer.h"
#include "Filter.h"

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
  int attrIndex = 0;
  vector<string> *tiles = indexer->findTilesByAttribute(attrIndex);
  for (vector<string>::iterator itc = tiles->begin(); itc != tiles->end(); ++itc) {
      cout << *itc << endl;
  }


  Filter::FilterType ftype = Filter::FilterType::GT;
  int64_t val = 5;
  string filtername = "output-filter-GT-5";
  Filter * f1 = new Filter(indexer, attrIndex, ftype, val, filtername);
  cout << "Filter: " << endl;
  f1->filter();
  return 0;
}
