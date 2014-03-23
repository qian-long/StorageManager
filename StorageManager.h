#ifndef _STORAGEMANAGER_H
#define _STORAGEMANAGER_H
#include <stdio.h>
#include <stdlib.h>
#include <string>

// Handles all the i/o with disk. No other class should interface directly
// with disk    
class StorageManager {
  private:
    char buffer[]; 
  public:
    uint64_t blocksize; // 50 megabytes

    StorageManager(uint64_t blocksize);
    ~StorageManager();

    FILE * openFile(char * filename, char *  mode);
    char * readBlockFromFile(FILE * filep, int offset, uint64_t * result);

    // Maintains buffer, write to disk in blocksize chunks
    void writeBlockToFile(FILE * filep, char data[]);
    void closeFile(FILE *);
};
#endif
