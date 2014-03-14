#ifndef _LOADER_H
#define _LOADER_H

#include <vector>
#include <string>
#include "Cell.h"

using std::vector;
using std::string;
class Cell;

class Loader {
  protected:
    vector<Cell *> cells;

    // split string
    //void splitLine(string & line, string delim, vector<string> & values);

  public:

    // parameters
    int nDim;
    int nAttr;
    vector<int> ranges;
    
    // Constructor
    Loader(int nDim, vector<int> ranges, int attribute);    
    
    // Destructor
    ~Loader();

    // reads in input file
    void read();

    // creates a temporary sorted file
    void sort();

    // divides sorted file into tiles
    void tile();
    
    // writes tiles to file
    void store(); 
};

#endif
