Little program to parse raw heightmap data from the file srtm_ramp2.world.86400x43200.bin
and write the result to a png image. The raw srtm data file can be downloaded from here:
http://visibleearth.nasa.gov/view.php?id=73934

Usage: hm <latitude> <longitude> <latitude span> <longitude span> <png>

In the heightmap data each pixel represent average height
(meters above sea) of area with a signed 16-bit integer.
Dimension of the data is 86400x43200.

Upper left pixel is:
180° W, each x-pixel: (360/86400)°.
 90° N, each y-pixel: (180/43200)°.
These two values shold be the same (0.004166667° per pixel, the grid spacing).


EXAMPLE

Scandinavia is roughly at:
x-direction: 3° E to 30° E
y-direction: 74° N to 55° N

To cut it out, run:

./hm 74 3 19 27 out.png

Use for instance google maps to figure out longitude/latitude. Just zoom into an area
and click on the share button (right of the print button) to copy the URL. It should look
something like this:

https://maps.google.com/?ll=59.611518,16.541634&spn=0.127302,0.42469

Here the center is at 59.611518° N, 16.541634° E and the
height/width span of the map in degrees are 0.127302°, 0.42469°.
The coordinates to the program in this case would be:

* 59.611518 - 0.127302/2  ; up    (y1)
* 16.541634 - 0.42469/2   ; left  (x1)
* 0.127302                ; height span
* 0.42469                 ; width span

Remember that South and West are negative while North and East are positive degrees.
