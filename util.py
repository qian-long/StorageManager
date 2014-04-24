import struct
import os

# binary data: (occurrence, value)
# 8 bytes, 8 bytes
# decompress RLE compressed binary data to csv
def decompressRLE_to_csv(filepath):
  infile = open(filepath, 'r')
  filename = os.path.basename(filepath)
  ofname = os.path.join(os.path.dirname(filepath), "decompressed-" + filename.split(".")[0] + ".csv")
  # remove if file exists
  if (os.path.isfile(ofname)):
    os.remove(ofname)
  oc_line = infile.read(8)
  val_line = infile.read(8)
  outfile = open(ofname, 'a+')
  while oc_line and val_line:
    occurrence = struct.unpack("<q", oc_line)[0]
    value = struct.unpack("<q", val_line)[0]
    for i in xrange(0, occurrence):
      outfile.write(str(value) + "\n")
      #outfile.write(struct.pack("<q", value))
      
    oc_line = infile.read(8)
    val_line = infile.read(8)
  infile.close()
  outfile.close()

# Convert uncompressed binary to csv
def binary_to_csv(filename, numPerLine):
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

def convert_dir(dirname):
  print "Converting binary to csv in " + dirname
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

def convert_dir_recursive(dirname):
  convert_dir(dirname)
  child_dirs = [os.path.join(dirname, o) for o in os.listdir(dirname) if os.path.isdir(os.path.join(dirname, o))]
  for d in child_dirs:
    convert_dir_recursive(d)


