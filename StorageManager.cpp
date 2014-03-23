#include <stdio.h>
#include <string>
#include "StorageManager.h"

using namespace std;
// Constructor
StorageManager::StorageManager(uint64_t blocksize) {
  this->blocksize = blocksize;
}

// Destructor
StorageManager::~StorageManager() {

}

FILE * StorageManager::openFile(char * filename, char * mode) {
  FILE * filep;
  filep = fopen(filename, mode);
  return filep;
}

// Read a block from file beginning at offset
char * StorageManager::readBlockFromFile(FILE * filep, int offset, uint64_t * result) {
  char * buffer;

  fseek(filep, offset, SEEK_SET);

  // buffer can store a blocksize worth of chars
  buffer = (char *) malloc(sizeof(char) * this->blocksize);
  if (buffer == NULL) {
    fputs ("Memory error",stderr); exit (2);
  }

  *result = fread(buffer, 1, this->blocksize, filep);

  // TODO: figure out how much was actually read
  return buffer;
}

void StorageManager::writeBlockToFile(FILE * filep, char data[]) {
  int size;
  if (sizeof(data) < this->blocksize) {
    size = sizeof(data);
  }
  fwrite(data, sizeof(char), size, filep);
}

void StorageManager::closeFile(FILE * filep) {
  fclose(filep);
}
