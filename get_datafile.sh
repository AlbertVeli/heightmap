#!/bin/sh

DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73934/srtm_ramp2.world.86400x43200.bin.gz'

wget -N $DATA || exit 1

echo "World heightmap downloaded. Unpack with:"
echo ""
echo "gunzip `basename $DATA`"
