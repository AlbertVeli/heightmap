#!/bin/sh
#
# All Rights Reversed - No Rights Reserved
#
#
# DESCRIPTION
#
# Run this script once to download the big
# data files used by the program.
#
###

TOPOGRAPHY_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73934/srtm_ramp2.world.86400x43200.bin.gz'
BATHYMETRY_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73963/gebco_bathy.21601x10801.bin.gz'
#BATHYMETRY_DATA='ftp://neo.sci.gsfc.nasa.gov/bluemarble/bmng/topography/gebco_bathy.43200x21600.0-8000_geo.tif'

# Topography/Bathymetry images for January-December is available at:
# Homepage: http://visibleearth.nasa.gov/view_cat.php?categoryID=1484
# Mirror 1: ftp://neo.sci.gsfc.nasa.gov/bluemarble/bmng/
# Mirror 2: ftp://ftp.cscs.ch/out/stockli/bluemarble/bmng/
# Change the image name in texpng.c to use another month image for texture.
# October 2004
TEXTURE_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73826/world.topo.bathy.200410.3x21600x10800.png'

wget -N $TOPOGRAPHY_DATA || exit 1
#wget -N $BATHYMETRY_DATA || exit 1
wget -N $TEXTURE_DATA || exit 1

echo "World heightmap downloaded. Unpack with:"
echo ""
echo "gunzip `basename $TOPOGRAPHY_DATA`"
