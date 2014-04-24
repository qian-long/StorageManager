#!/usr/bin/python

import random

def generate(LIMIT, num_cells, num_col):
  for i in xrange(num_cells):
    string = ""
    for j in xrange(num_col-1):
      string = string + str(int(random.random()*LIMIT)) + ","

    string = string + str(int(random.random()*LIMIT))
    print string

def uniform(start, end, num_lines, num_cols):
 for i in xrange(num_cells):
  string = ""
  for j in xrange(num_col-1):
    string = string + str(random.randint(start, end)) + ","

  string = string + str(random.randint(start, end))
  print string

def skewed(start, end, num_lines, num_cols):
  pass

if __name__ == "__main__":
  uniform(0, 1000, 100, 3)
