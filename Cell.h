#ifndef _CELL_H
#define _CELL_H
#include <vector>
#include <stdint.h>
#include <string>

using std::vector;
using std::string;

class Cell {
  private:
    uint64_t mortonCode;
    string tileID;

  public:
    vector<int64_t> coords;
    vector<int64_t> attributes;

    // Constructor
    Cell(vector<int64_t> coords, vector<int64_t> attributes);

    // Destructor
    ~Cell();

    // Setters
    void setMortonCode(uint64_t num);
    void setTileID(string id);

    // Getters
    uint64_t getMortonCode();
    string getTileID();

    // Comparisions for sorting
    bool operator=(Cell * other);
};

#endif
