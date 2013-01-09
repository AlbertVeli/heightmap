#include "map.h"
#include "hmpng.h"

/* Each pixel represent average height (meters above sea) of
 * area with a signed 16-bit integer. Dim 86400x43200.
 *
 * Upper left pixel is:
 * 180° W, each x-pixel: (360/86400)°.
 *  90° N, each y-pixel: (180/43200)°.
 * These two values shold be the same (0.004166667° per pixel, the grid spacing).
 *
 * Scandinavia is roughly:
 * x-direction: 3° E to 30° E
 * y-direction: 74° N to 55° N
 *
 * To cut it out, run:
 * ./hm 74 3 19 27
 *
 * Use for instance google maps to figure out longitude/latitude. Just zoom into an area
 * and click on the share button (right of the print button) and copy the URL. It should look
 * something like this:
 *
 * https://maps.google.com/?ll=59.611518,16.541634&spn=0.127302,0.42469
 *
 * Here the center is at 59.611518° N, 16.541634° E and the
 * height/width of the map in degrees are 0.127302°, 0.42469°.
 * The coordinates to the program in this case would be:
 * 59.611518 - 0.127302/2, # up    (map to y1)
 * 16.541634 - 0.42469/2,  # left  (map to x1)
 * 0.127302                # height span
 * 0.42469                 # widht span
 *
 * Remember that South and West are negative while North and East are positive degrees.
 */

void usage(char **argv)
{
   printf("usage: %s <latitude> <longitude> <latitude span> <longitude span> <png>\n", argv[0]);
   printf("Use decimal fractions for latidue/longitude and spans.\n");
   printf("latitude (North/South) is in range 90 (° N) to -90 (° S).\n");
   printf("longitude (West/East) is in range -180 (° W) to 180 (° E).\n\n");
   printf("EXAMPLE\n\nTo save Scandinavia (74° N, 3° E, span 19°, 27°)\nto heightmap out.png, run:\n\n");
   printf("%s 74 3 19 27 out.png\n\n", argv[0]);
}


int main(int argc, char *argv[])
{
   long double lat, lon, span_w, span_h;
   int x1, x2, y1, y2;
   char *outfile;

   /* parse parameters and check bounds */
   if (argc != 6) {
      usage(argv);
      return 1;
   }

   lat = strtold(argv[1], NULL);
   if (lat < -90 || lat > 90) {
      usage(argv);
      return 1;
   }

   lon = strtold(argv[2], NULL);
   if (lon < -180 || lon > 180) {
      usage(argv);
      return 1;
   }

   span_h = strtold(argv[3], NULL);
   if (span_h < 0 || span_h > 180) {
      usage(argv);
      return 1;
   }

   span_w = strtold(argv[4], NULL);
   if (span_w < 0 || span_w > 360) {
      usage(argv);
      return 1;
   }

   if (lat + span_h > 180) {
      usage(argv);
      return 1;
   }

   if (lon + span_w > 360) {
      usage(argv);
      return 1;
   }

   outfile = argv[5];

   /* This hughe height map can be downloaded from:
    * http://visibleearth.nasa.gov/view.php?id=73934
    */
   if (!map_map("srtm_ramp2.world.86400x43200.bin")) {
      return 1;
   }

   printf("Saving the following latitude/longitude range to '%s':\n", outfile);
   printf("%Lf° %c, %Lf° %c, span %Lf°, %Lf°\n",
          lat < 0 ? -lat : lat, lat < 0 ? 'N' : 'S',
          lon < 0 ? -lon : lon, lon < 0 ? 'W' : 'E',
          span_h, span_w);

   /* Map longitude/latidude to pixel coordinates */
   x1 = ((lon + 180.0) / (long double)360) * MAPW;
   x2 = (((lon + span_w) + 180.0) / (long double)360) * MAPW;
   y1 = ((90 - lat) / (long double)180) * MAPH;
   y2 = ((90 - (lat - span_h)) / (long double)180) * MAPH;
   printf("%dx%d pixels\n", x2 - x1 + 1, y2 - y1 + 1);

   save_hm_png(x1, x2, y1, y2, outfile);

   return 0;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
