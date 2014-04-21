#ifndef _BOUNDINGBOX_H
#define _BOUNDINGBOX_H
#include <vector>
#include <string>

using std::vector;
using std::string;
// Bounding Box
class BoundingBox {
  public:
    vector<int64_t> * minCoords;
    vector<int64_t> * maxCoords;
    int nDim;
    string tileid;

    // Constructor
    BoundingBox(string line, int nDim);

    // Destructor
    ~BoundingBox();
};
#endif
