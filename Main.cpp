#include <stdlib.h>
#include <iostream>
#include <map>
#include <climits>
#include "Loader.h"
#include "Filter.h"
#include "Subarray.h"
#include "Indexer.h"
#include "IndexerL.h"
#include "Indexerp.h"

using namespace std;

void printVector(vector<string> * vec) {
  for (vector<string>::iterator it = vec->begin(); it != vec->end(); ++it) {
    cout << *it << endl;
  }
}

// Main function
int main(int argc, char *argv[]) {

  // TODO: Read this from config file (probably postgres) later
  /*
  int64_t nDim = 2;
  int64_t nAttribute = 1;
  int stride = 2;
  uint64_t tile_size = 16*4;

  vector<int64_t> ranges;
  ranges.push_back(0);
  ranges.push_back(10);
  ranges.push_back(0);
  ranges.push_back(10);
  string csvfile = "data/bb1.csv";
  //string csvfile = "data/compress-tiny1.csv";
  //string csvfile = "data/compress-tiny.csv";
  //string csvfile = "data/compress-tiny2.csv";

  
  Loader *loader = new Loader(csvfile, nDim, ranges, nAttribute, stride, tile_size);
  cout << "Hello, world" << endl;

  cout << "loader->load()" << endl;
  loader->load();
  cout << "loader->tile()" << endl;
  loader->tile();


  Indexer *indexer = new Indexer(nDim, ranges, nAttribute, stride, "myindex.txt");
  int attrIndex = 0;
  vector<string> *tiles = indexer->findTilesByAttribute(attrIndex);
  cout << "After indexer construction" << endl;
  for (vector<string>::iterator itc = tiles->begin(); itc != tiles->end(); ++itc) {
      cout << *itc << endl;
  }


  Filter::FilterType ftype = Filter::FilterType::GT;
  int64_t val = 4;
  string filtername = "output-filter-GT-4";
  Filter * f1 = new Filter(indexer, attrIndex, ftype, val, filtername);
  cout << "\n\nFILTER: " << endl;
  f1->filter();

  vector<int64_t> subranges;
  subranges.push_back(3);
  subranges.push_back(7);
  subranges.push_back(3);
  subranges.push_back(7);

  vector<string> *subtiles = indexer->getTilesByDimSubRange(&subranges);
  vector<string> * wholeTiles = indexer->getWholeTilesByDimSubRange(&subranges);
  vector<string> * partialTiles = indexer->getPartialTilesByDimSubRange(&subranges);

  cout << "all sub tiles" << endl;
  printVector(subtiles);
  cout << "whole sub tiles" << endl;
  printVector(wholeTiles);
  cout << "partial sub tiles" << endl;
  printVector(partialTiles);

  cout << "Subarray: " << endl;
  string subarrayName = "output-subarray0";
  Subarray * s1 = new Subarray(subarrayName, indexer, &subranges, &ranges, stride);

  s1->execute();
  */
  int64_t nDim = 2;
  int64_t nAttribute = 1;
  int stride = 2;
  uint64_t tile_size = 16*4;

  vector<int64_t> ranges;
  ranges.push_back(0);
  ranges.push_back(10);
  ranges.push_back(0);
  ranges.push_back(10);
  string csvfile = "data/bb2.csv";
  //string csvfile = "data/compress-tiny1.csv";
  //string csvfile = "data/compress-tiny.csv";
  //string csvfile = "data/compress-tiny2.csv";


  Loader *loader = new Loader(csvfile, nDim, ranges, nAttribute, stride, tile_size);
  cout << "loader->loadp()" << endl;
  loader->loadp();
  cout << "loader->tilep()" << endl;
  loader->tilep();
  cout << "loader->load()" << endl;
  loader->load();
  cout << "loader->tile()" << endl;
  loader->tile();

  Indexerp *indexer = new Indexerp(nDim, ranges, nAttribute, "myindex-fp.txt");
  int attrIndex = 0;

  Filter::FilterType ftype = Filter::FilterType::GT;
  int64_t val = 4;
  string filtername = "output-fp-filter-GT-4";
  Filter * fp1 = new Filter(indexer, attrIndex, ftype, val, filtername);
  cout << "\n\nFILTER: " << endl;
  fp1->filter();

  vector<int64_t> subranges;
  subranges.push_back(4);
  subranges.push_back(6);
  subranges.push_back(3);
  subranges.push_back(6);

  cout << "all sub tiles [4,6] [3,6]" << endl;
  vector<string> * subtiles = indexer->getTilesByDimSubRange(&subranges);
  vector<string> * wholeTiles = indexer->getWholeTilesByDimSubRange(&subranges);
  vector<string> * partialTiles = indexer->getPartialTilesByDimSubRange(&subranges);

  cout << "all sub tiles" << endl;
  printVector(subtiles);
  cout << "whole sub tiles" << endl;
  printVector(wholeTiles);
  cout << "partial sub tiles" << endl;
  printVector(partialTiles);

  cout << "Subarray: " << endl;
  string subarrayName = "output-fp-subarray0";
  Subarray * s1 = new Subarray(subarrayName, indexer, &subranges, &ranges, stride);

  s1->execute();

  /*
  cout << "TESTING INDEXERS" << endl;
  Indexer *indexerp = new Indexerp(nDim, ranges, nAttribute, "myindex-fp.txt");
  Indexer *indexerl = new IndexerL(nDim, ranges, nAttribute, stride, "myindex.txt");

  cout << endl << "FIXED PHYSICAL TILE IDS" << endl;
  for (vector<string>::iterator itp = indexerp->tileids->begin(); itp != indexerp->tileids->end(); ++itp) {

    string attrTile = indexerp->getAttrTileById(0, *itp);
    string rletile = indexerp->getRLEAttrTileById(0, *itp);
    string coordTile = indexerp->getCoordTileById(*itp);
    cout << "attrTile: " << attrTile << endl;
    cout << "rletile: " << rletile << endl;
    cout << "coordTile: " << coordTile << endl;
  }

  cout << "FIXED LOGICAL TILE IDS" << endl;
  for (vector<string>::iterator it = indexerl->tileids->begin(); it != indexerl->tileids->end(); ++it) {
    string attrTile = indexerl->getAttrTileById(0, *it);
    string rletile = indexerl->getRLEAttrTileById(0, *it);
    string coordTile = indexerl->getCoordTileById(*it);
    cout << "attrTile: " << attrTile << endl;
    cout << "rletile: " << rletile << endl;
    cout << "coordTile: " << coordTile << endl;
  }
  */

  cout << "LLONG_MAX: " << LLONG_MAX << endl;
  return 0;
}


