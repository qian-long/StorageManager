#!/usr/bin/python
import util
# To Test:
# process input file and fill out map of lines per file
#   compute tile id
# decompress tiles
# Check size of each tile and total number of tiles

def process_csv(filename, ndim, nattr, stride):
  lines_per_tile = {}
  f = open(filename, 'r')
  lines = f.readlines()
  for line in lines:
    values = [int(x) for x in line.split(',')]
    coords = values[0:ndim]
    attributes = values[ndim:]
    tile_id = get_tile_id(coords, stride)
    if tile_id not in lines_per_tile:
      lines_per_tile[tile_id] = 0
    lines_per_tile[tile_id] += 1

  return lines_per_tile

def get_num_lines(filename):
  f = open(filename, 'r')
  return len(f.readlines())

def get_tile_id(coords, stride):
  tile_id = str(coords[0] / stride)
  for i in xrange(1, len(coords)):
    tile_id = tile_id + "-" + str(coords[i] / stride)
  return tile_id

def red(s):
  return '\033[1;31m%s\033[m' % s

def green(s):
  return '\033[1;32m%s\033[m' % s

def error(msg):
  print red("ERROR: " + msg)
  exit(0)

def log_pass(msg):
  print green("PASS: " + msg)

def log(msg):
  print "LOG: " + msg

def check_tiling(csvfile, ndim, nattr, stride):
  log("Checking tile %s ndim: %d nattr: %d stride %d" % (csvfile, ndim, nattr, stride))
  tile_map = process_csv(csvfile, ndim, nattr, stride)

  # Check number of lines in coord tiles
  for tile_id in tile_map:
    util.binary_to_csv('tile-coords-' + tile_id + '.dat', ndim)
    num_lines = get_num_lines('tile-coords-' + tile_id + '.csv')
    if num_lines != tile_map[tile_id]:
      error("mismatch tile_id: " + tile_id)

  log_pass("all coord tiles have correct sizes")

  # Decompress each attribute file
  log(csvfile + "Decompressing attribute files...")
  for tile_id in tile_map:
    for attr in xrange(0, nattr):
      attrfile = "rle-tile-attrs[%d]-%s.dat" % (attr, tile_id)
      util.decompressRLE(attrfile)
      csv_attrfile = "decompressed-rle-tile-attrs[%d]-%s.csv" % (attr, tile_id)
      num_lines = get_num_lines(csv_attrfile)
      if num_lines != tile_map[tile_id]:
        error("Mismatch Attribute Tile: " + csv_attrfile)

  log_pass("All attribute tiles have correct sizes")

def check_filter():
  pass

def check_subarray():
  pass

def clean_up():
  pass

if __name__ == "__main__":
  check_tiling('data/tiny.csv', 2, 1, 2)
