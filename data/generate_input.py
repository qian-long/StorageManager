#!/usr/bin/python

import random

def generate(LIMIT):
  for i in xrange(10):
    x = int(random.random()*LIMIT)
    y = int(random.random()*LIMIT)
    attr = int(random.random()*LIMIT)
    print str(x) + ',' + str(y) + ',' + str(attr)

if __name__ == "__main__":
  generate(10)
