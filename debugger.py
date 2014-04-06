import struct
import os
import fnmatch

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
    num = struct.unpack("<Q", line)[0]
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

if __name__ == "__main__":
  main()
