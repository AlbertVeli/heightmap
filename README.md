Proggy to read heightmap data from the file *srtm_ramp2.world.86400x43200.bin*
and write the wanted area of the world to one 16-bit grayscale png heightmap image
and one texture rgb png image. The wanted area is specified in latitudes/longitudes.

**PREPARATION**

Before running the program the first time, download the files:

* srtm_ramp2.world.86400x43200.bin.gz
* world.topo.bathy.200410.3x21600x10800.png

from:

Webpage: http://visibleearth.nasa.gov/view.php?id=73934  
Mirror: ftp://neo.sci.gsfc.nasa.gov/bluemarble/bmng/

The script [get_datafiles.sh](heightmap/blob/master/datafiles/get_datafiles.sh)
automatically downloads these files. After download is finished, unpack the
heightmap file with gunzip:

    gunzip srtm_ramp2.world.86400x43200.bin.gz


**USAGE**

    hm [OPTIONS] <latitude> <longitude> <latitude span> <longitude span> <basename>

Use decimal fractions for latitude/longitude/spans.  

> *latitude* (North/South) - in range 90 (° N) to -90 (° S)  
> *longitude* (West/East) - in range -180 (° W) to 180 (° E)  
> *latitude span* - height of the wanted area in degrees  
> *longitude span* - width of the wanted area in degrees  
> *basename* - common beginning of the two filenames to save output to


**OPTIONS**

    -h <FILE> - Datafile for heightmap instead of default
    -t <FILE> - Datafile for texture instead of default
    -v        - Verbose output


**EXAMPLE**

To generate images *scandinavia_heightmap.png* and *scandinavia_texture.png*
for the Scandinavia area (74° N, 3° E, span 19°, 27°), run:

    ./hm 74 3 19 27 scandinavia

Use a map, terrestrial globe, google maps or other web service to figure
out latitudes/longitudes. In google maps, zoom into an area and
copy the URL (click on the share button). It should look something like:

https://maps.google.com/?ll=59.611518,16.541634&spn=0.127302,0.42469

Here the center is at *59.611518° N*, *16.541634°* E and the
height/width span of the map in degrees are *0.127302°*, *0.42469°*.
To get to the upper left corner, move one half span in each direction.

The arguments to cut out the corresponding area with this program:

    59.611518 + 0.127302/2  ; up half height span
    16.541634 - 0.42469/2   ; left half width span
    0.127302                ; height span
    0.42469                 ; width span

Remember that South and West are negative while North and East are positive degrees.


**ELEVATION DATAFILE**

In the heightmap data each pixel represent average height
(meters above sea) of an area with a signed 16-bit
integer (big endian).

Dimension of the srtm data is 86400x43200. Upper left pixel is at
90° N, 180° W and bottom right is at is at 90° S, 180° E.
Each pixel is thus 360/86400° = 15 arcseconds ~= 500m (at the equator).
1 arcsecond = 1/3600:th of a degree. Longitudes are distorted towards the
poles while latitudes are roughly 500m per pixel through the whole range.


**INSTRUCTION VIDEO**

This video shows how to compile and use the program. It also shows how
the generated heightmap and texture files can be rendered with
[Blender](http://www.blender.org/).

http://www.youtube.com/watch?v=1nKxmyudoLI


**LICENSE**

    ~~=) All Rights Reversed - No Rights Reserved (=~~

Prickle-Prickle, the 9th day of Chaos in the YOLD 3179

Albert Veli
