#!/bin/sh
#
# All Rights Reversed - No Rights Reserved.
#
# Prickle-Prickle, the 9th day of Chaos in the YOLD 3179
#
# Albert Veli
###

ELEVATION_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73934/srtm_ramp2.world.86400x43200.bin.gz'
TEXTURE_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73580/world.topo.bathy.200401.3x21600x10800.png'

wget -N $ELEVATION_DATA || exit 1
wget -N $TEXTURE_DATA || exit 1

echo "World heightmap downloaded. Unpack with:"
echo ""
echo "gunzip `basename $ELEVATION_DATA`"
