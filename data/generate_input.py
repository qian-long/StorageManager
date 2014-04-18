#!/usr/bin/python

import random

def generate(LIMIT, num_cells, num_col):
  for i in xrange(num_cells):
    string = ""
    for j in xrange(num_col-1):
      string = string + str(int(random.random()*LIMIT)) + ","

    string = string + str(int(random.random()*LIMIT))
    print string

if __name__ == "__main__":
  generate(100, 100, 3)
