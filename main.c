#include "map.h"
#include "hmpng.h"

/* See README.md for raw heighmap data format description.
 *
 * Before running the program the first time, download height
 * data file srtm_ramp2.world.86400x43200.bin.gz from:
 *
 * http://visibleearth.nasa.gov/view.php?id=73934
 *
 * This can also be done with the script get_datafile.sh.
 * Then unpack the file with gunzip.
 *
 * All Rights Reversed - No Rights Reserved
 */

void usage(char **argv)
{
   printf("usage: %s <latitude> <longitude> <latitude span> <longitude span> <basename>\n\n", argv[0]);
   puts("Use decimal fractions for latitude/longitude/spans.");
   puts("latitude (North/South) in range 90 (° N) to -90 (° S).");
   puts("longitude (West/East) in range -180 (° W) to 180 (° E).\n");
   puts("ARGUMENTS\n");
   puts("latitude/longitude - upper left corner of the wanted area.");
   puts("latitude/longitude span - height and width of wanted area in degrees.");
   puts("basename - common beginning of the two filenames to save output to.\n");
   puts("EXAMPLE\n");
   puts("To generate images for Scandinavia area (74° N, 3° E, span 19°, 27°)");
   puts("scandinavia_heightmap.png and scandinavia_texture.png, run:\n");
   printf("%s 74 3 19 27 scandinavia\n\n", argv[0]);
}

#define FNAMESIZE 128

int main(int argc, char *argv[])
{
   long double lat, lon, span_w, span_h;
   int x1, x2, y1, y2;
   char *basename;
   char heightfile[FNAMESIZE];
   char texturefile[FNAMESIZE];

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

   basename = argv[5];
   snprintf(heightfile, FNAMESIZE, "%s_heightmap.png", basename);
   /* on some systems snprintf skips terminating 0 if truncated */
   heightfile[FNAMESIZE - 1] = 0;
   snprintf(texturefile, FNAMESIZE, "%s_texture.png", basename);
   texturefile[FNAMESIZE - 1] = 0;

   /* This hughe height map can be downloaded from:
    * http://visibleearth.nasa.gov/view.php?id=73934
    *
    * You can run get_datafiles.sh to fetch it
    * and the texture datafile.
    */
   if (!map_map("srtm_ramp2.world.86400x43200.bin")) {
      puts("run get_datafiles.sh to download gz-file");
      puts("and unpack it with gunzip");
      return 1;
   }

   printf("%Lf° %c, %Lf° %c, span %Lf°, %Lf°\n\n",
          lat < 0 ? -lat : lat, lat < 0 ? 'S' : 'N',
          lon < 0 ? -lon : lon, lon < 0 ? 'W' : 'E',
          span_h, span_w);

   /* Map longitude/latidude to pixel coordinates */
   x1 = ((lon + 180.0) / (long double)360) * MAPW;
   x2 = (((lon + span_w) + 180.0) / (long double)360) * MAPW;
   y1 = ((90 - lat) / (long double)180) * MAPH;
   y2 = ((90 - (lat - span_h)) / (long double)180) * MAPH;

   save_heightmap_png(x1, x2, y1, y2, heightfile);

   free_map();

   save_texture_png(lat, lon, span_h, span_w, texturefile);

   return 0;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
