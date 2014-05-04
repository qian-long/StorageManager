#include <stdlib.h>
#include <iostream>
#include <map>
#include <climits>
#include <ctime>
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

  /*
  int64_t nDim = 2;
  int64_t nAttribute = 1;
  int stride = 100;
  uint64_t tile_size = 16*10;

  vector<int64_t> ranges;
  ranges.push_back(0);
  ranges.push_back(1000);
  ranges.push_back(0);
  ranges.push_back(1000);
  string csvfile = "data/small.csv";


  cout << "Loading csvfile: " << csvfile << endl;
  Loader *loader = new Loader(csvfile, nDim, ranges, nAttribute);
  cout << "Fixed Logical Tiles Loading..." << endl;
  loader->loadl(stride);
  */

  /*
  cout << "Fixed Physical Tiles Loading..." << endl;
  loader->loadp(tile_size);

  Indexer *indexerp = new Indexerp(nDim, ranges, nAttribute, "output-FP-" + loader->arrayname);
  Indexer *indexerl = new IndexerL(nDim, ranges, nAttribute, stride, "output-fl-" + loader->arrayname);
  */

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
  int64_t val = 50;
  string filtername = "filter-GT50";
  Filter * fp1 = new Filter(indexerp, attrIndex, ftype, val, filtername);
  cout << "\n\nFILTER on bb2 fixed physical: " << endl;
  fp1->filter();

  cout << "\nFILTER on bb2 fixed logical: " << endl;
  Filter * fp2 = new Filter(indexerl, attrIndex, ftype, val, filtername);
  fp2->filter();

  vector<int64_t> subranges;
  subranges.push_back(1);
  subranges.push_back(505);
  subranges.push_back(2);
  subranges.push_back(499);
  string subarray0Name = "subarray1";

  cout << "\nSUBARRAY0 [1,505] [2,499]" << endl;
  cout << "\nSUBARRAY0 on bb2 fixed physical tiles..." << endl;

  Subarray * s1 = new Subarray(subarray0Name, indexerp, &subranges, &ranges, stride);
  s1->execute();

  cout << "\nSUBARRAY0 on bb2 fixed logical tiles..." << endl;
  Subarray * s2 = new Subarray(subarray0Name, indexerl, &subranges, &ranges, stride);
  s2->execute();


  vector<int64_t> subranges2;
  subranges2.push_back(710);
  subranges2.push_back(905);
  subranges2.push_back(710);
  subranges2.push_back(900);
  string subarray2Name = "subarray2";
  cout << "\nSUBARRAY [710,905] [710,900]" << endl;

  cout << "\nSUBARRAY0 on bb2 fixed physical tiles..." << endl;
  Subarray * s3 = new Subarray(subarray2Name, indexerp, &subranges2, &ranges, stride);
  s3->execute();

  cout << "\nSUBARRAY0 on bb2 fixed logical tiles..." << endl;
  Subarray * s4 = new Subarray(subarray2Name, indexerl, &subranges2, &ranges, stride);
  s4->execute();


  vector<int64_t> subranges3;
  subranges3.push_back(0);
  subranges3.push_back(1000);
  subranges3.push_back(0);
  subranges3.push_back(1000);
  string subarray3Name = "subarray3";
  cout << "\nSUBARRAY [0,1000] [0,1000]" << endl;
  cout << "\nSUBARRAY3 on bb2 fixed physical tiles..." << endl;
  Subarray * s5 = new Subarray(subarray3Name, indexerp, &subranges3, &ranges, stride);
  s5->execute();

  cout << "\nSUBARRAY3 on bb2 fixed logical tiles..." << endl;
  Subarray * s6 = new Subarray(subarray3Name, indexerl, &subranges3, &ranges, stride);
  s6->execute();


  vector<int64_t> subranges4;
  subranges4.push_back(253);
  subranges4.push_back(759);
  subranges4.push_back(904);
  subranges4.push_back(1000);
  string subarray4Name = "subarray4";
  cout << "\nSUBARRAY [253,759] [904,1000]" << endl;
  cout << "\nSUBARRAY3 on bb2 fixed physical tiles..." << endl;
  Subarray * s7 = new Subarray(subarray4Name, indexerp, &subranges4, &ranges, stride);
  s7->execute();

  cout << "\nSUBARRAY3 on bb2 fixed logical tiles..." << endl;
  Subarray * s8 = new Subarray(subarray4Name, indexerl, &subranges4, &ranges, stride);
  s8->execute();
  */

  int64_t nDim = 2;
  int64_t nAttribute = 2;
  int stride = 10000;
  uint64_t tile_size = 16*1000000; // 16000000, 16 MB coordinate tiles

  vector<int64_t> ranges;
  ranges.push_back(0);
  ranges.push_back(90*1000);
  ranges.push_back(0);
  ranges.push_back(180*1000);
  string csvfile = "data/processed_geo_tweets_2013_08_13.csv";

  cout << "Initializing loader for csvfile: " << csvfile << endl;
  Loader *loader = new Loader(csvfile, nDim, ranges, nAttribute);

  /*
  clock_t begin = clock();
  cout << "Fixed Logical Tiles Loading..." << endl;
  loader->loadl(stride);
  clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Loading total elapsed time in seconds: " << elapsed_secs << endl << endl;

  begin = clock();
  cout << "Fixed Physical Tiles Loading..." << endl;
  loader->loadp(tile_size);
  end = clock();
  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

  cout << "Loading total elapsed time in seconds: " << elapsed_secs << endl << endl;
  */

  Indexer *indexerp = new Indexerp(nDim, ranges, nAttribute, "output-FP-" + loader->arrayname);
  Indexer *indexerl = new IndexerL(nDim, ranges, nAttribute, stride, "output-fl-" + loader->arrayname);

  int attrIndex = 0;
  Filter::FilterType ftype = Filter::FilterType::GT;
  // timestamp
  int64_t val = 1376402182;
  string filtername = "filter-GT1376402182";

  clock_t begin = clock();
  Filter * fp1 = new Filter(indexerp, attrIndex, ftype, val, filtername);
  cout << "\n\nFILTER fixed physical on: " << csvfile << endl;
  fp1->filter();

  clock_t end = clock();
  double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

  cout << "Filter total elapsed time in seconds: " << elapsed_secs << endl << endl;

  cout << "\n\nFILTER fixed logical on: " << csvfile << endl;

  begin = clock();
  Filter * fp2 = new Filter(indexerl, attrIndex, ftype, val, filtername);
  fp2->filter();
  end = clock();
  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
  cout << "Filter total elapsed time in seconds: " << elapsed_secs << endl << endl;

  // subarray testing
  vector<int64_t> subranges4;
  subranges4.push_back(312);
  subranges4.push_back(45000);
  subranges4.push_back(130);
  subranges4.push_back(178000);
  string subarray4Name = "subarray0";
  cout << "\nSUBARRAY [0,45000] [0,90000]" << endl;

  begin = clock();
  cout << "\nSUBARRAY3 on fixed physical tiles..." << endl;
  Subarray * s7 = new Subarray(subarray4Name, indexerp, &subranges4, &ranges, stride);
  s7->execute();
  end = clock();

  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

  cout << "Subarray total elapsed time in seconds: " << elapsed_secs << endl << endl;

  begin = clock();
  cout << "\nSUBARRAY3 on bb2 fixed logical tiles..." << endl;
  Subarray * s8 = new Subarray(subarray4Name, indexerl, &subranges4, &ranges, stride);
  s8->execute();

  end = clock();

  elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

  cout << "Subarray total elapsed time in seconds: " << elapsed_secs << endl << endl;
  return 0;

}


