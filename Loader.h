#ifndef _LOADER_H
#define _LOADER_H

#include <vector>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include "Cell.h"

using std::vector;
using std::string;
using std::map;
using std::set;
using std::stringstream;
class Cell;

class Loader {

  public:
    // parameters
    string filename; // csv path
    int64_t nDim; // number of dimensions
    int64_t nAttr; // number of attributes
    vector<int64_t> ranges; // ranges of dimensions
    string arrayname; // name of array
    
    // Constructor
    Loader(string filename, int64_t nDim, vector<int64_t> ranges, int64_t nAttr);    
    // Destructor
    ~Loader();

    // Read input csv file, creates .sorted csv file that is sorted by tileid
    // creates index file containing all non-empty tile ids, used for indexing later
    void loadl(int stride);

    // Read input csv file, sort by row major, divide into fixed physical tiles
    void loadp(uint64_t tileMemLimit);

    // Helper function to write buffers to disk
    void writeTileBufsToDisk(map<string, string> * attrBufMap, stringstream * coordBuf, string tileid, string outdir);

    // write out new file
    // TODO: delete uncompressed tile?
    void compressTile(string& attrfilename);


    // TODO: make private
    void createIndexFile(string dirpath, set<string> * tileIDs);
    uint64_t mortonEncode2D(uint64_t x, uint64_t y);
    uint64_t shiftCoord(int64_t coord, int64_t min);
    string getTileID(string line, int stride);
    string getSortKey(string line);
  private:
    static string getFileName(const string& s);
    static string removeExtension(const string& filename);
    static string getFilePath(const string& dirpath, const string& filename);
    static string getDirPath(const string& filepath);

    // Divides sorted file into fixed logical tiles and writes to disk
    void tilel(string outdir, string sortedfile, int stride);

    // Divides sorted file into fixed physical tiles, write tiles to disk, write index file along the way
    void tilep(string outdir, string sortedfile, uint64_t tileMemLimit);
};

#endif
