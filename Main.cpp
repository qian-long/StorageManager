#include <stdlib.h>
#include <iostream>
#include "Loader.h"

using namespace std;

// Main function
// TODO: move somewhere else
int main(int argc, char *argv[]) {
  int64_t nDim = 2;
  int64_t nAttribute = 1;
  int stride = 2;

  // TODO: Read this from config file later
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

  return 0;
}
