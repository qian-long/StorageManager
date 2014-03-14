#ifndef _CELL_H
#define _CELL_H
#include <vector>

using std::vector;

class Cell {
  public:
    vector<int> coords;
    vector<int> attributes;

    // Constructor
    Cell(vector<int> coords, vector<int> attributes);

    // Destructor
    ~Cell();
};

#endif
