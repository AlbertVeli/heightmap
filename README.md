Proggy to read raw heightmap data from the file *srtm_ramp2.world.86400x43200.bin*
and write the wanted area of the world to one 16-bit grayscale png heightmap image
and one texture png image. The wanted area is specified in latitudes/longitudes.

Before running the program first time, download the srtm elevation data file:

Webpage: http://visibleearth.nasa.gov/view.php?id=73934  
Mirror: ftp://neo.sci.gsfc.nasa.gov/bluemarble/bmng/

The file can also be downloaded with the script [get_datafiles.sh](heightmap/blob/master/get_datafiles.sh).
After download is finished, unpack the file with gunzip:

    gunzip srtm_ramp2.world.86400x43200.bin.gz


**USAGE**

    hm <latitude> <longitude> <latitude span> <longitude span> <basename>

Use decimal fractions for latitude/longitude/spans.  

> *latitude* (North/South) - in range 90 (° N) to -90 (° S)  
> *longitude* (West/East) - in range -180 (° W) to 180 (° E)  
> *latitude span* - height of the wanted area in degrees  
> *longitude span* - width of the wanted area in degrees  
> *basename* - common beginning of the two filenames to save output to


**EXAMPLE**

Scandinavia is roughly at:

> x-direction: 3° E to 30° E  
> y-direction: 74° N to 55° N

To cut it out, run:

    ./hm 74 3 19 27 scandinavia.png

Use for instance google maps to figure out longitude/latitude. Zoom into an area
and click on the share button (right of the print button) to copy the URL.
You should get an URL like:

https://maps.google.com/?ll=59.611518,16.541634&spn=0.127302,0.42469

Here the center is at 59.611518° N, 16.541634° E and the
height/width span of the map in degrees are 0.127302°, 0.42469°.
To get to the upper left corner, move one half span in each direction.

The arguments to cut out the corresponding area with this program:

* 59.611518 + 0.127302/2  ; up half height span
* 16.541634 - 0.42469/2   ; left half width span
* 0.127302                ; height span
* 0.42469                 ; width span

Remember that South and West are negative while North and East are positive degrees.


**DATAFILE**

In the heightmap data each pixel represent average height
(meters above sea) of area with a signed 16-bit integer (big endian).

Dimension of the srtm data is 86400x43200. Upper left pixel is at
90° N, 180° W and bottom right is at is at 90° S, 180° E.
Each pixel is thus 360/86400 = 180/43200 = 15 arcseconds ~= 500m (at the equator).
1 arcsecond = 1/3600:th of a degree. Longitudes are distorted towards the poles while
latitudes are roughly 500m per pixel through the whole range.


**LICENSE**

    ~~=) All Rights Reversed - No Rights Reserved (=~~

Prickle-Prickle, the 9th day of Chaos in the YOLD 3179

Albert Veli
