#include <stdlib.h>
#include <iostream>
#include <map>
#include "Loader.h"
#include "Indexer.h"
#include "Filter.h"
#include "Subarray.h"

using namespace std;
// Main function
int main(int argc, char *argv[]) {

  // TODO: Read this from config file (probably postgres) later
  int64_t nDim = 2;
  int64_t nAttribute = 1;
  int stride = 2;
  uint64_t mem_limit = 100; // number of bytes

  vector<int64_t> ranges;
  ranges.push_back(0);
  ranges.push_back(10);
  ranges.push_back(0);
  ranges.push_back(10);
  //string csvfile = "data/tiny.csv";
  string csvfile = "data/compress-tiny1.csv";
  //string csvfile = "data/compress-tiny.csv";
  //string csvfile = "data/compress-tiny2.csv";

  Loader *loader = new Loader(csvfile, nDim, ranges, nAttribute, stride, mem_limit);
  cout << "Hello, world" << endl;

  cout << "loader->load()" << endl;
  loader->load();
  cout << "loader->tile()" << endl;
  loader->tile();



  Indexer *indexer = new Indexer(nDim, ranges, nAttribute, stride);
  int attrIndex = 0;
  vector<string> *tiles = indexer->findTilesByAttribute(attrIndex);
  for (vector<string>::iterator itc = tiles->begin(); itc != tiles->end(); ++itc) {
      cout << *itc << endl;
  }


  Filter::FilterType ftype = Filter::FilterType::GT;
  int64_t val = 4;
  string filtername = "output-filter-GT-4";
  Filter * f1 = new Filter(indexer, attrIndex, ftype, val, filtername);
  cout << "\n\nFILTER: " << endl;
  f1->filter2();
  //f1->filterTile("2-1");

/*
  cout << "Subarray: " << endl;
  vector<int64_t> subranges;
  subranges.push_back(3);
  subranges.push_back(7);
  subranges.push_back(3);
  subranges.push_back(7);
  string subarrayName = "output-subarray0";
  Subarray * s1 = new Subarray(subarrayName, indexer, &subranges, &ranges, stride);
  s1->execute();
*/
  return 0;
}
