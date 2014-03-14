#!/usr/bin/python

import random

def gen_tiny():
  pass

def gen_small():
  LIMIT = 1000
  for i in xrange(100):
    x = int(random.random()*LIMIT)
    y = int(random.random()*LIMIT)
    attr = int(random.random()*LIMIT)
    print str(x) + ',' + str(y) + ',' + str(attr)

if __name__ == "__main__":
  gen_small()
