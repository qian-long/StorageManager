#!/usr/bin/python
import util
import copy
import fnmatch
import os

LOGGING = False

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
  if LOGGING:
    print "LOG: " + msg

# TODO: fix for target output
def check_logical_tiling(csvfile, ndim, nattr, stride):
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

# TODO
def setup_dir(arraydir, ndim, nattr):
  log("Setting up %s for testing..." % arraydir)
  coord_tiles = [os.path.join(filterdir, x) for x in fnmatch.filter(os.listdir(filterdir, 'tile-coords-*.dat'))]
  attr_tiles = [os.path.join(filterdir, x) for x in fnmatch.filter(os.listdir(filterdir, 'decompressed-rle-tile-attrs[%d]-*.dat'))]


  # convert coord .dat files to .csv
  # decompress rle-attr-tiles .dat files directly to csv
  pass

# TODO
def cleanup_dir(arraydir):
  log("Cleaning up %s" % arraydir)
  # remove all generated csv files in arraydir
  pass

# convert all binaries in all array roots
# maybe not good for large arrays (GB sized)
def convert_root_arraydirs(arraydirs):
  for rootdir in arraydirs:
    util.convert_dir_recursive(dirname)

# See if fixed logical and fixed physical grouping produce the same output
# Note: fixed logical tiles probably aren't sorted, fixed physical tiles should be sorted (can be different sorting keys)
def check_dual(dirname1, dirname2, ndim, nattr):
  dirnames = [dirname1, dirname2]
  # might have duplicate lines
  # line->count
  dirs_lines = [{}, {}]
  # read all coord tiles
  csv_coordtiles1 = [os.path.join(dirname1, x) for x in fnmatch.filter(os.listdir(dirname1), 'tile-coords-*.csv')]
  csv_coordtiles2 = [os.path.join(dirname2, x) for x in fnmatch.filter(os.listdir(dirname2), 'tile-coords-*.csv')]

  coordtiles_list = [csv_coordtiles1, csv_coordtiles2]

  for i in xrange(len(coordtiles_list)):
    coordtiles = coordtiles_list[i]
    dir_lines = dirs_lines[i]
    for tile in coordtiles:
      f = open(tile, 'r')
      for line in f.readlines():
        if line not in dir_lines:
          dir_lines[line] = 0
        else:
          log("duplicate line")
        dir_lines[line] += 1

  same = compare_maps(dirs_lines[0], dirs_lines[1])
  if same:
    log_pass("[%s] [%s] coords match" % (dirname1, dirname2))
  else:
    error("[%s] [%s] coords DON'T match" % (dirname1, dirname2))

  # read all attr tiles:
  for i in xrange(nattr):
    log("attribute: " + str(i))
    attribute = i
    attr_tiles = []
    coord_tiles = []

    # map coord line to (attrval, count)
    dirs = [{}, {}]

    # TODO: rereads coord tile for each attribute, oh well...
    for j in xrange(len(dirnames)):
      dirname = dirnames[j]
      dir_lines = dirs[j]
      # attr_tiles and coord_tiles should have same order
      attr_tiles = sorted([os.path.join(dirname, x) for x in fnmatch.filter(os.listdir(dirname), 'rle-tile-attrs[[]%d[]]-*.dat' % (i))])
      coord_tiles = sorted([os.path.join(dirname, x) for x in fnmatch.filter(os.listdir(dirname), 'tile-coords-*.csv')])

      if len(attr_tiles) != len(coord_tiles):
        error("attr_tiles: %d coord_tiles: %d, different!" % (len(attr_tiles), len(coord_tiles)))

      for k in xrange(len(attr_tiles)):
        if (os.path.getsize(attr_tiles[k]) % 8 != 0):
          error("%s size: %d not multiple of 8" % (attr_tiles[k], os.path.getsize(attr_tiles[k])))
        # Decompress attribute tile
        util.decompressRLE_to_csv(attr_tiles[k])
        basename = os.path.basename(attr_tiles[k]).split('.')[0] + '.csv'
        decompressed_csv = os.path.join(dirname, "decompressed-" + basename)

        fattr = open(decompressed_csv, 'r')
        fcoord = open(coord_tiles[k], 'r')
        coords = fcoord.readlines()
        attributes = fattr.readlines()

        log("attrfile: " + decompressed_csv + " coordfile: " + coord_tiles[k])
        if len(coords) != len(attributes):
          error("coord file and attribute file have different number of lines")
        for l in xrange(len(coords)):
          coord = coords[l].strip()
          attribute = attributes[l].strip()
          if coord not in dir_lines:
            dir_lines[coord] = {attribute: 0}
          dir_lines[coord][attribute] += 1

    #log("dirs[0].keys(): " + str(dirs[0].keys()))
    #log("dirs[1].keys(): " + str(dirs[1].keys()))
    same = compare_attr_maps(dirs[0], dirs[1])
    if same:
      log_pass("[%s] [%s] attribute [%d] match" % (dirname1, dirname2, i))
    else:
      error("[%s] [%s] attribute [%d] DON'T match" % (dirname1, dirname2, i))

# map1/map2 = {coordline: {attr: count, attr2: count2}, coordline1: {blah}}
def compare_attr_maps(map1, map2):
  # check keys first
  diff = set(map1.keys()) - set(map2.keys())
  if len(diff) != 0:
    log("map1 %d and map2 %d have different keys" % (len(map1.keys()), len(map2.keys())))
    log("diff: " + str(diff))
    return False

  map3 = copy.deepcopy(map1)

  for key in map2.keys():
    diff = set(map2[key].keys()) - set(map1[key].keys())
    if len(diff) != 0:
      log("map1[%s] and map2[%s] have different attributes for a coord" % (key, key))
      log("map1[key]: " + str(map1[key]))
      log("map2[key]: " + str(map2[key]))
      return False

    # check attribute counts for each key
    for key1 in map2[key]:
      if map2[key][key1] != map1[key][key1]:
        log("different counts for attirbutes")
        return False

  return True

def compare_maps(map1, map2):
  # check keys first
  diff = set(map1.keys()) - set(map2.keys())
  if len(diff) != 0:
    log("map1 and map2 have different keys")
    return False
  map3 = copy.deepcopy(map1)

  # iterate through map2 and subtract val differences for each key
  for key in map2.keys():
    map3[key] = map3[key] - map2[key]

  # if any key has non zero val, then return error
  for key in map3:
    if map3[key] != 0:
      log("map3 has non zero key")
      return False
  return True

# TODO: add setup and cleanup
def check_filter(csvfile, ndim, attr_index, filterdir, filtertype, value):
  # get filtering from original csv file map: {coords:value} that satisfy filter

  # {coordline: {attribute: count}} to account for possible duplicate coordinates from auto generated csvs
  filter_map = {}
  f = open(csvfile, 'r')
  for line in f:
    line = line.strip()
    coords = line.split(',')[0:ndim]
    attributes = line.split(',')[ndim:]
    attribute = attributes[attr_index] # strings
    if evaluate_filterexpr(filtertype, value, long(attribute)):
      coords_str = ",".join(coords)
      if coords_str not in filter_map:
        filter_map[coords_str] = {}
      if attribute not in filter_map[coords_str]:
        filter_map[coords_str][attribute] = 0

      filter_map[coords_str][attribute] += 1

  # read tiles from filterdir and see if it matches map produced from original csv file
  filter_map2 = {}
  coord_tiles = sorted([os.path.join(filterdir, x) for x in fnmatch.filter(os.listdir(filterdir), 'tile-coords-*.csv')])
  attr_tiles = sorted([os.path.join(filterdir, x) for x in fnmatch.filter(os.listdir(filterdir), 'decompressed-rle-tile-attrs[[]%d[]]-*.csv' % attr_index)])

  if len(attr_tiles) != len(coord_tiles):
    error("attr_tiles: %d coord_tiles: %d, different!" % (len(attr_tiles), len(coord_tiles)))

  for i in xrange(len(coord_tiles)):
    attrfile = open(attr_tiles[i], 'r')
    coordfile = open(coord_tiles[i], 'r')

    attrs = attrfile.readlines()
    coords = coordfile.readlines()
    if len(coords) != len(attrs):
      error("coord file and attribute file have different number of lines")

    for j in xrange(len(coords)):
      coord_s = coords[j].strip()
      attr_s = attrs[j].strip()
      if coord_s not in filter_map2:
        filter_map2[coord_s] = {}

      if attr_s not in filter_map2[coord_s]:
        filter_map2[coord_s][attr_s] = 0
      filter_map2[coord_s][attr_s] += 1

  check = compare_attr_maps(filter_map, filter_map2)
  if check:
    log_pass("check_filter %s matches" % (filterdir))
  else:
    error("check_filter %s doesn't match" % (filterdir))

  # TODO add cleanup


def evaluate_filterexpr(filtertype, value, attribute):
  if filtertype == 'LT':
    return attribute < value
  elif filtertype == 'LE':
    return attribute <= value
  elif filtertype == 'EQ':
    return attribute == value
  elif filtertype == 'GE':
    return attribute >= value
  elif filtertype == 'GT':
    return attribute > value
  else:
    error("unknown filtertype : " + filtertype)

def check_subarray():
  pass

def check_loading():
  pass

if __name__ == "__main__":
  csvfiles = ['data/small.csv']
  arraydirs = ['output-fl-small', 'output-FP-small']
  subarrays = ['output-fl-small/subarray1', 'output-FP-small/subarray1']
  filters = ['output-fl-small/filter-GT50', 'output-FP-small/filter-GT50']
  check_dual(arraydirs[0], arraydirs[1], 2, 1)
  check_dual(subarrays[0], subarrays[1], 2, 1)
  check_dual(filters[0], filters[1], 2, 1)
  check_filter('data/small.csv', 2, 0, 'output-fl-small/filter-GT50', 'GT', 50)
