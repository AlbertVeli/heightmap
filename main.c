#include "map.h"
#include "hmpng.h"
#include "parseargs.h"

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

#define FNAMESIZE 128

int main(int argc, char **argv)
{
   char heightfile[FNAMESIZE];
   char texturefile[FNAMESIZE];

   /* parse parameters and check bounds */
   if (!parse_args(argc, argv)) {
      print_usage();
      return 1;
   }

   snprintf(heightfile, FNAMESIZE, "%s_heightmap.png", opt.basename);
   snprintf(texturefile, FNAMESIZE, "%s_texture.png", opt.basename);

   /* This hughe height map can be downloaded from:
    * http://visibleearth.nasa.gov/view.php?id=73934
    *
    * You can run get_datafiles.sh to fetch it
    * and the texture datafile.
    */
   if (!map_map(opt.heightmap, 0)) {
      puts("run get_datafiles.sh to download gz-file");
      puts("and unpack it with gunzip");
      return 1;
   }

   save_heightmap_png(opt.latitude, opt.longitude, opt.span_h, opt.span_w, heightfile);

   free_map(0);

   save_texture_png(opt.latitude, opt.longitude, opt.span_h, opt.span_w, texturefile);

   return 0;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
