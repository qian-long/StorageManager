import struct
import os

# binary data: (occurrence, value)
# 8 bytes, 8 bytes
"""
def uncompressRLE(filename):
  in_file = open(filename, 'r')
  ofname = "uncompressRLE" + filename.split(".")[0] + ".csv"

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
"""
def decompressRLE(filename):
  infile = open(filename, 'r')
  ofname = "decompressed-" + filename.split(".")[0] + ".csv" 
  oc_line = infile.read(8)
  val_line = infile.read(8)
  outfile = open(ofname, 'a+')
  while oc_line and val_line:
    occurrence = struct.unpack("<q", oc_line)[0]
    value = struct.unpack("<q", val_line)[0]
    for i in xrange(0, occurrence):
      outfile.write(str(value) + "\n")
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


