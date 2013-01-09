#!/bin/sh
#
# All Rights Reversed - No Rights Reserved
#

ELEVATION_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73934/srtm_ramp2.world.86400x43200.bin.gz'
# January 2004
#TEXTURE_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73580/world.topo.bathy.200401.3x21600x10800.png'
# October 2004
TEXTURE_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73826/world.topo.bathy.200410.3x21600x10800.png'

wget -N $ELEVATION_DATA || exit 1
wget -N $TEXTURE_DATA || exit 1

echo "World heightmap downloaded. Unpack with:"
echo ""
echo "gunzip `basename $ELEVATION_DATA`"
