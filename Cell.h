#ifndef _CELL_H
#define _CELL_H
#include <vector>
#include <stdint.h>

using std::vector;

class Cell {
  private:
    uint64_t mortonCode;

  public:
    vector<int64_t> coords;
    vector<int64_t> attributes;

    // Constructor
    Cell(vector<int64_t> coords, vector<int64_t> attributes);

    // Destructor
    ~Cell();

    // Setters
    void setMortonCode(uint64_t num);

    // Getters
    uint64_t getMortonCode();

    // Comparisions for sorting
    bool operator=(Cell * other);
};

#endif
