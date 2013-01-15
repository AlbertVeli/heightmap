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
#BATHYMETRY_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73963/gebco_bathy.21601x10801.bin.gz'
BATHYMETRY_DATA='ftp://neo.sci.gsfc.nasa.gov/bluemarble/bmng/topography/gebco_bathy.43200x21600.0-8000_geo.tif'

# Topography/Bathymetry images for January-December is available at:
# Homepage: http://visibleearth.nasa.gov/view_cat.php?categoryID=1484
# Mirror 1: ftp://neo.sci.gsfc.nasa.gov/bluemarble/bmng/
# Mirror 2: ftp://ftp.cscs.ch/out/stockli/bluemarble/bmng/
# Change the image name in texpng.c to use another month image for texture.
# October 2004
TEXTURE_DATA='http://eoimages.gsfc.nasa.gov/images/imagerecords/73000/73826/world.topo.bathy.200410.3x21600x10800.png'

clean_up_download_and_quit ()
{
    echo "$1 failed to download, removing partial download"
    rm $1
    exit 1
}

download_file ()
{
    wget -N $1 || clean_up_download_and_quit $1
}

ask ()
{
    while true; do
	read -p "$1 [Yes/No] " yn
	case $yn in
            [Yy]* )
		return 0
		break;;
            [Nn]* ) break;;
            * ) echo "Please answer yes or no.";;
	esac
    done

    return 1
}

fail_md5 ()
{
    if ask "md5sum check failed for $1, remove it?"; then
	rm -f $1
    fi

    exit 1
}

check_md5_bsd ()
{
    wantedsum=`awk '{print $1}' $1.md5`
    mysum=`md5 -q $1`
    if test "$mysum" = "$wantedsum"; then
	echo "$1: OK"
    else
	fail_md5 $1
    fi
}

check_md5 ()
{
    if test -f "/usr/bin/md5sum"; then
	/usr/bin/md5sum -c "$1.md5" || fail_md5 $1
    else
	check_md5_bsd $1
    fi
}

# Don't download if unzipped file already exists
if ! test -f `basename $TOPOGRAPHY_DATA .gz`; then
    download_file $TOPOGRAPHY_DATA
    echo "World heightmap downloaded. Unpack with:"
    echo ""
    echo "gunzip `basename $TOPOGRAPHY_DATA`"
else
    echo "Already have `basename $TOPOGRAPHY_DATA .gz`"
fi

if ! test -f `basename $TEXTURE_DATA`; then
    download_file $TEXTURE_DATA
else
    echo "Already have `basename $TEXTURE_DATA`"
fi

if ! test -f `basename $BATHYMETRY_DATA`; then
    if ask "Do you wish to download bathymetry datafile (optional)?"; then
	download_file $BATHYMETRY_DATA
	echo "Run ./merge_topo_bathy_tif to build the merged elevation datafile"
    fi
else
    echo "Already have `basename $BATHYMETRY_DATA`"
fi

if ask "Got all files, check md5sums?"; then
    check_md5 `basename $TOPOGRAPHY_DATA`
    check_md5 `basename $TEXTURE_DATA`
    if test -f `basename $BATHYMETRY_DATA`; then
	check_md5 `basename $BATHYMETRY_DATA`
    fi
fi
