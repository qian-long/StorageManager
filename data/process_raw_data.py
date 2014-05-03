#!/usr/bin/python

import sys
from time import strptime
from time import mktime

def process(filename):
  f = open(filename, 'r')
  ofilename = "processed_" + filename.split('.')[0] + '.csv'
  outfile = open(ofilename, 'w+')
  f.readline()
  for line in f:
    array = line.strip().split('\t')
    tweetid = array[0]
    time = array[1]
    lat = array[2]
    lon = array[3]
    if tweetid == "" or time == "" or lat == "" or lon == "":
      continue
    # converted as localtime, but doesn't matter
    # seconds
    timestamp = int(mktime(strptime(' '.join(time.split(' ')[0:2]), '%Y-%m-%d %H:%M:%S')))
    # round and shift coords so everything is an integer >= 0
    # latitude is from -90 to +90
    # longitude is from -180 to +180
    # want 10^3 precision
    shifted_lat = int(float(lat)*1000) + 90*1000
    shifted_long = int(float(lon)*1000) + 180*1000
    newline = str(shifted_lat) + ',' + str(shifted_long) + ',' + str(timestamp) + ',' + tweetid + '\n'
    outfile.write(newline)

  f.close()
  outfile.close()


if __name__ == "__main__":
  if len(sys.argv) < 2:
    sys.stderr.write('Usage: ./process_raw_data.py filename\n')
    sys.exit(1)
  process(sys.argv[1])
