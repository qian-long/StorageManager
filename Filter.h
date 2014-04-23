#ifndef _FILTER_H
#define _FILTER_H

#include "Indexer.h"

// Filters on compressed data
class Filter {
  public:
    Indexer * indexer; 

    enum FilterType { GT, GE, EQ, LT, LE };
    int64_t value; // value for comparison
    FilterType ftype; // type of filter
    int attrIndex; // which attribute to filter on
    string name; // name of output directory
    string outdir; // output directory

    // Constructor
    Filter(Indexer * indexer, int attrIndex, FilterType ftype, int64_t value, string name);

    // Destructor
    ~Filter();

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

