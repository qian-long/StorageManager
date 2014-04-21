#ifndef _FILTERP_H
#define _FILTERP_H

#include "Indexerp.h"

// Filters on compressed data
class Filterp {
  public:
    Indexerp * indexer; 

    enum FilterType { GT, GE, EQ, LT, LE };
    int64_t value;
    FilterType ftype;
    int attrIndex;
    string name;

    // Constructor
    Filterp(Indexerp * indexer, int attrIndex, FilterType ftype, int64_t value, string name);

    // Destructor
    ~Filterp();

    /*
    void filterGT(int64_t num);
    void filterGE(int64_t num);    
    void filterEQ(int64_t num);
    void filterLT(int64_t num);
    void filterLE(int64_t num);
    */
    //void filter();
    void filter();
    void filterTile(string tileid);
    // Assumes that expression has been linearlized into ands and ors
    // TODO: implement later for composite expressions
    void buildLinearExpression();

  private:
    bool evaluate(int64_t attrval);
};

#endif

