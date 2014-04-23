#!/usr/bin/python

import struct
import os
import fnmatch

# Convert uncompressed binary to csv
def binaryToCSV(filename, numPerLine):
  in_file = open(filename, 'r')
  line = in_file.read(8)

  # open for appending
  ofname = filename.split(".")[0] + ".csv"
  # remove if file exists
  if (os.path.isfile(ofname)):
    os.remove(ofname)

  out_file = open(ofname, "a+")

  pos = 0
  while line:
    # Little Endian
    num = struct.unpack("<q", line)[0]
    line = in_file.read(8)
    out_file.write(str(num))
    if pos == numPerLine - 1:
      out_file.write("\n")
      pos = 0
    else:
      out_file.write(",")
      pos = pos + 1

  in_file.close()
  out_file.close()

# binary data: (occurrence, value)
# 8 bytes, 8 bytes
def uncompressRLE(filename):
  in_file = open(filename, 'r')
  ofname = filename.split(".")[0] + ".csv"

  oc_line = in_file.read(8)
  num_line = in_file.read(8)
  if (os.path.isfile(ofname)):
    os.remove(ofname)

  out_file = open(ofname, "a+")
  while oc_line and num_line:
    occurrence = struct.unpack("<q", oc_line)[0]
    number = struct.unpack("<q", num_line)[0]
    out_file.write(str(occurrence) + "," + str(number) + "\n")
    oc_line = in_file.read(8)
    num_line = in_file.read(8)

  in_file.close()
  out_file.close()

def convert_dir(dirname):
  print "dirname: ", dirname
  nDim = 2
  nAttr = 1
  # convert coordinate tiles from binary to csv
  coord_tiles = fnmatch.filter(os.listdir(dirname),'tile-coords-*.dat')
  for tile in coord_tiles:
    print "tile", tile
    binaryToCSV(dirname + "/" + tile, nDim)

  attr_tiles = fnmatch.filter(os.listdir(dirname), 'tile-attrs*.dat')
  for tile in attr_tiles:
    print "attr tile", tile
    binaryToCSV(dirname + "/" + tile, nAttr)

  # uncompress rle compressed attribute tiles
  rle_tiles = fnmatch.filter(os.listdir(dirname), 'rle-*.dat')
  for tile in rle_tiles:
    print "rle tile", tile
    uncompressRLE(dirname + "/" + tile)


def main():
  nDim = 2
  nAttr = 1
  # convert coordinate tiles from binary to csv
  coord_tiles = fnmatch.filter(os.listdir('.'),'tile-coords-*.dat')
  for tile in coord_tiles:
    binaryToCSV(tile, nDim)

  attr_tiles = fnmatch.filter(os.listdir('.'), 'tile-attrs*.dat')
  for tile in attr_tiles:
    binaryToCSV(tile, nAttr)

  # uncompress rle compressed attribute tiles
  rle_tiles = fnmatch.filter(os.listdir('.'), 'rle-*.dat')
  for tile in rle_tiles:
    uncompressRLE(tile)

if __name__ == "__main__":
  main()
  convert_dir("output-fl-bb2");
  convert_dir('output-fl-bb2/subarray0')
  convert_dir("output-FP-bb2");
  convert_dir("output-FP-bb2/subarray0")
  #convert_dir('output-filter-GT-4')
  #convert_dir('output-subarray0')
