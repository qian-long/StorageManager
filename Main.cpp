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


  cout << "Loading csvfile: " << csvfile << endl;
  Loader *loader = new Loader(csvfile, nDim, ranges, nAttribute);
  cout << "Fixed Logical Tiles Loading..." << endl;
  loader->loadl(stride);

  cout << "Fixed Physical Tiles Loading..." << endl;
  loader->loadp(tile_size);

  cout << "TESTING INDEXERS" << endl;
  cout << "stride: " << stride << endl;
  Indexer *indexerp = new Indexerp(nDim, ranges, nAttribute, "output-FP-bb2");
  Indexer *indexerl = new IndexerL(nDim, ranges, nAttribute, stride, "output-fl-bb2");

  /*
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
  
  /*
  int attrIndex = 0;
  Filter::FilterType ftype = Filter::FilterType::GT;
  int64_t val = 4;
  string filtername = "filter-GT4";
  Filter * fp1 = new Filter(indexerp, attrIndex, ftype, val, filtername);
  cout << "\n\nFILTER on bb2 fixed physical: " << endl;
  fp1->filter();

  cout << "\nFILTER on bb2 fixed logical: " << endl;
  Filter * fp2 = new Filter(indexerl, attrIndex, ftype, val, filtername);
  fp2->filter();
  */
  vector<int64_t> subranges;
  subranges.push_back(4);
  subranges.push_back(6);
  subranges.push_back(3);
  subranges.push_back(6);
  string subarrayName = "subarray0";

  /*
  cout << "\nSUBARRAY on bb2 fixed physical tiles..." << endl;
  vector<string> * subtiles = indexerp->getTilesByDimSubRange(&subranges);
  vector<string> * wholeTiles = indexerp->getWholeTilesByDimSubRange(&subranges);
  vector<string> * partialTiles = indexerp->getPartialTilesByDimSubRange(&subranges);

  cout << "all sub tiles" << endl;
  printVector(subtiles);
  cout << "whole sub tiles" << endl;
  printVector(wholeTiles);
  cout << "partial sub tiles" << endl;
  printVector(partialTiles);
  */
  Subarray * s1 = new Subarray(subarrayName, indexerp, &subranges, &ranges, stride);

  s1->execute();
  cout << "\nSUBARRAY on bb2 fixed logical tiles..." << endl;
  cout << "indexerl->tileids" << endl;
  printVector(indexerl->tileids);
  vector<string> * subtiles1 = indexerl->getTilesByDimSubRange(&subranges);
  vector<string> * wholeTiles1 = indexerl->getWholeTilesByDimSubRange(&subranges);
  vector<string> * partialTiles1 = indexerl->getPartialTilesByDimSubRange(&subranges);

  cout << "\nSUBARRAY [4,6] [3,6]" << endl;
  cout << "fixed logical: all sub tiles" << endl;
  printVector(subtiles1);
  cout << "fixed logical: whole sub tiles" << endl;
  printVector(wholeTiles1);
  cout << "fixed logical: partial sub tiles" << endl;
  printVector(partialTiles1);

  vector<int64_t> subranges2;
  subranges2.push_back(5);
  subranges2.push_back(7);
  subranges2.push_back(3);
  subranges2.push_back(6);
  vector<string> * subtiles2 = indexerl->getTilesByDimSubRange(&subranges2);
  vector<string> * wholeTiles2 = indexerl->getWholeTilesByDimSubRange(&subranges2);
  vector<string> * partialTiles2 = indexerl->getPartialTilesByDimSubRange(&subranges2);

  cout << "\nSUBARRAY [5,7] [3,6]" << endl;
  cout << "fixed logical: all sub tiles" << endl;
  printVector(subtiles2);
  cout << "fixed logical: whole sub tiles" << endl;
  printVector(wholeTiles2);
  cout << "fixed logical: partial sub tiles" << endl;
  printVector(partialTiles2);

  vector<int64_t> subranges3;
  subranges3.push_back(5);
  subranges3.push_back(7);
  subranges3.push_back(2);
  subranges3.push_back(6);
  vector<string> * subtiles3 = indexerl->getTilesByDimSubRange(&subranges3);
  vector<string> * wholeTiles3 = indexerl->getWholeTilesByDimSubRange(&subranges3);
  vector<string> * partialTiles3 = indexerl->getPartialTilesByDimSubRange(&subranges3);

  cout << "\nSUBARRAY [5,7] [2,6]" << endl;
  cout << "fixed logical: all sub tiles" << endl;
  printVector(subtiles3);
  cout << "fixed logical: whole sub tiles" << endl;
  printVector(wholeTiles3);
  cout << "fixed logical: partial sub tiles" << endl;
  printVector(partialTiles3);

  vector<int64_t> subranges4;
  subranges4.push_back(4);
  subranges4.push_back(7);
  subranges4.push_back(2);
  subranges4.push_back(7);
  vector<string> * subtiles4 = indexerl->getTilesByDimSubRange(&subranges4);
  vector<string> * wholeTiles4 = indexerl->getWholeTilesByDimSubRange(&subranges4);
  vector<string> * partialTiles4 = indexerl->getPartialTilesByDimSubRange(&subranges4);

  cout << "\nSUBARRAY [4,7] [2,7]" << endl;
  cout << "fixed logical: all sub tiles" << endl;
  printVector(subtiles4);
  cout << "fixed logical: whole sub tiles" << endl;
  printVector(wholeTiles4);
  cout << "fixed logical: partial sub tiles" << endl;
  printVector(partialTiles4);




  /*
  Subarray * s2 = new Subarray(subarrayName, indexerl, &subranges, &ranges, stride);
  s2->execute();
  */

  return 0;
}


