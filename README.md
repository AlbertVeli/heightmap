Proggy to read raw heightmap data from the file *srtm_ramp2.world.86400x43200.bin*
and write the wanted area of the world a png 16-bit grayscale heightmap image.

Before running the program first time, download the raw srtm data file:

http://visibleearth.nasa.gov/view.php?id=73934

The file can also be downloaded with the script get_datafile.sh. Then unpack the file with gunzip:

<code>
gunzip srtm_ramp2.world.86400x43200.bin.gz
</code>

**Usage: hm &lt;latitude&gt; &lt;longitude&gt; &lt;latitude span&gt; &lt;longitude span&gt; &lt;png&gt;**

> *latitude* (North/South) is in range 90 (° N) to -90 (° S).  
> *longitude* (West/East) is in range -180 (° W) to 180 (° E).  
> *latitude span* is the height of the wanted area in degrees.  
> *longitude span* is the width of the wanted area in degrees.

In the heightmap data each pixel represent average height
(meters above sea) of area with a signed 16-bit integer.

Dimension of the data is 86400x43200. Upper left pixel is at
90° N, 180° W and bottom right is at is at 90° S, 180° E.
Each pixel is thus 360/86400 = 180/43200 = 30 arcseconds.
1 arcsecond = 1/3600:th of a degree.

NASA has higher resolutions available (1 and 3 arcseconds) for download, but those are divided into several parts and the wanted area may be divided between parts which complicates the program. This one has the whole world in one big file (7GB unpacked). The humongous size slows down reading of the source data, but it simplifies the program to only have one datafile to deal with.


**EXAMPLE**

Scandinavia is roughly at:
> x-direction: 3° E to 30° E  
> y-direction: 74° N to 55° N

To cut it out, run:

<code>
./hm 74 3 19 27 scandinavia.png
</code>

Use for instance google maps to figure out longitude/latitude. Zoom into an area
and click on the share button (right of the print button) to copy the URL.
You should get an URL like this:

https://maps.google.com/?ll=59.611518,16.541634&spn=0.127302,0.42469

Here the center is at 59.611518° N, 16.541634° E and the
height/width span of the map in degrees are 0.127302°, 0.42469°.
To get to the upper left corner, move one half span in each direction.

The arguments to cut out the corresponding area with this program:

* 59.611518 + 0.127302/2  ; up
* 16.541634 - 0.42469/2   ; left
* 0.127302                ; height span
* 0.42469                 ; width span

Remember that South and West are negative while North and East are positive degrees.
