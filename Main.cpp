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
  string subarray0Name = "subarray1";

  cout << "\nSUBARRAY0 [4,6] [3,6]" << endl;
  cout << "\nSUBARRAY0 on bb2 fixed physical tiles..." << endl;

  Subarray * s1 = new Subarray(subarray0Name, indexerp, &subranges, &ranges, stride);
  s1->execute();

  cout << "\nSUBARRAY0 on bb2 fixed logical tiles..." << endl;
  Subarray * s2 = new Subarray(subarray0Name, indexerl, &subranges, &ranges, stride);
  s2->execute();


  vector<int64_t> subranges2;
  subranges2.push_back(5);
  subranges2.push_back(7);
  subranges2.push_back(3);
  subranges2.push_back(6);
  string subarray2Name = "subarray2";
  cout << "\nSUBARRAY [5,7] [3,6]" << endl;

  cout << "\nSUBARRAY0 on bb2 fixed physical tiles..." << endl;
  Subarray * s3 = new Subarray(subarray2Name, indexerp, &subranges2, &ranges, stride);
  s3->execute();

  cout << "\nSUBARRAY0 on bb2 fixed logical tiles..." << endl;
  Subarray * s4 = new Subarray(subarray2Name, indexerl, &subranges2, &ranges, stride);
  s4->execute();


  vector<int64_t> subranges3;
  subranges3.push_back(5);
  subranges3.push_back(7);
  subranges3.push_back(2);
  subranges3.push_back(6);
  string subarray3Name = "subarray3";
  cout << "\nSUBARRAY [5,7] [2,6]" << endl;
  cout << "\nSUBARRAY3 on bb2 fixed physical tiles..." << endl;
  Subarray * s5 = new Subarray(subarray3Name, indexerp, &subranges3, &ranges, stride);
  s5->execute();

  cout << "\nSUBARRAY3 on bb2 fixed logical tiles..." << endl;
  Subarray * s6 = new Subarray(subarray3Name, indexerl, &subranges3, &ranges, stride);
  s6->execute();


  vector<int64_t> subranges4;
  subranges4.push_back(4);
  subranges4.push_back(7);
  subranges4.push_back(2);
  subranges4.push_back(7);
  string subarray4Name = "subarray4";
  cout << "\nSUBARRAY [4,7] [2,7]" << endl;
  cout << "\nSUBARRAY3 on bb2 fixed physical tiles..." << endl;
  Subarray * s7 = new Subarray(subarray4Name, indexerp, &subranges4, &ranges, stride);
  s7->execute();

  cout << "\nSUBARRAY3 on bb2 fixed logical tiles..." << endl;
  Subarray * s8 = new Subarray(subarray4Name, indexerl, &subranges4, &ranges, stride);
  s8->execute();



  return 0;
}


