/*
 * Standalone proglet to merge two hughe .bin files
 * containing topography and bathygraphy into one
 * .bin file containing elevation data for both
 * ocean floor and land.
 *
 * All Rights Reversed - No Rights Reserved
 */

#include "map.h"

#include <stdint.h>
#include <png.h>
#include <arpa/inet.h> /* ntohs */

/* Datafiles. Dimensions are in map.h */
#define SRTMFILE "datafiles/srtm_ramp2.world.86400x43200.bin"
#define BATHYFILE "datafiles/gebco_bathy.21601x10801.bin"
#define OUTFILE "datafiles/merged_topo_bathy.86400x43200.bin"

static int16_t min = 0;
static int16_t max = 0;

static void find_extremes(char *mp, int w, int h)
{
   int x, y;
   int16_t *p = (int16_t *)mp;
   int16_t elevation, lmin, lmax;

   lmin = 0;
   lmax = 0;

   for (y = 0; y < h; y++) {
      for (x = 0; x < w; x++) {
         elevation = ntohs(*p++);
         if (elevation < lmin) {
            lmin = elevation;
         }
         if (elevation > lmax) {
            lmax = elevation;
         }
      }
   }

   printf(" - min: %d\n", lmin);
   printf(" - max: %d\n", lmax);

   if (lmin < min) {
      min = lmin;
   }
   if (lmax > max) {
      max = lmax;
   }
}

static void merge_topo_bathy(void)
{
   FILE *fp;
   int x, y;
   int16_t *sp, *bp;
   int16_t s_elevation, b_elevation, elevation;
   int count, county;
   int16_t offset;

   if (!(fp = fopen(OUTFILE, "w"))) {
      perror(OUTFILE);
      return;
   }

   /* Move every point up offset meters so lowest point of ocean floor
    * becomes 0. If offset is 10577 then ocean level becomes 10577m and
    * greatest elevation (8579m above sea) becomes 10577 + 8579 = 19156m.
    * This is within the limits of a signed 16 bit integer, which can
    * hold values up to 2^15 = 32768m.
    *
    * Set offset to 0 if you want to keep sea level at 0m.
    */
   if (min < 0) {
      /* Set offset to lowest point found by find_extremes */
      offset = abs(min);
   } else {
      /* default, 10577 = lowest point in gebco_bathy.21601x10801.bin
       * use that as offset if find_extremes was skipped.
       *
       * Change this to 0 to keep sea level at 0m.
       */
      offset = 10577;
   }

   printf("Merging files to %s, using offset %d meters\n", OUTFILE, offset);

   sp = (int16_t *)map[0];
   bp = (int16_t *)map[1];

   county = 0;
   for (y = 0; y < S_MAPH; y++) {
      count = 0;
      b_elevation = ntohs(*bp);

      for (x = 0; x < S_MAPW; x++) {

         s_elevation = ntohs(*sp++);

#if 0
         /* Take the biggest (positive or negative) */
         if (abs(s_elevation) > abs(b_elevation)) {
            elevation = s_elevation;
         } else {
            elevation = b_elevation;
         }
#else
         /* This seems to work better. Use land data
          * if it is > 0, else use bathygraphy data.
          */
         if (s_elevation > 0) {
            elevation = s_elevation;
         } else {
            elevation = b_elevation;
         }
#endif

         /* Write elevation (2 bytes) to destination file */
         elevation = htons(elevation + offset);
         if (fwrite(&elevation, 2, 1, fp) != 1) {
            /* Error. Probably out of diskspace. */
            fprintf(stderr, "Error writing to %s, is disk full?\n", OUTFILE);
            fclose(fp);
            return;
         }

         /* Step up bp every fourth pixel (see dimensions) */
         if (count == 3) {
            count = 0;
            b_elevation = ntohs(*bp++);
         } else {
            count++;
         }

      }

      /* Step up bp every fourth line (see dimensions) */
      if (county == 3) {
         county = 0;
         /* Jump over extra pixel at end */
         bp++;
      } else {
         /* Back full line - 1 */
         county++;
         bp -= B_MAPW - 1;
      }

   }

   fclose(fp);
}


int main(void)
{
   /* Memory map datafiles */
   if (!map_map(SRTMFILE, 0)) {
      free_maps();
      return 1;
   }

   if (!map_map(BATHYFILE, 1)) {
      free_maps();
      return 1;
   }

   /* Change to #if 1 to search both bin-files for lowest point */
#if 0
   printf("%s:\n", SRTMFILE);
   find_extremes(map[0], S_MAPW, S_MAPH);
   /* datafiles/srtm_ramp2.world.86400x43200.bin:
    *  - min: -907
    *  - max: 8579
    */
   printf("\n%s:\n", BATHYFILE);
   find_extremes(map[1], B_MAPW, B_MAPH);
   /* datafiles/gebco_bathy.21601x10801.bin:
    *  - min: -10577
    *  - max: 8430
    */
#endif

   merge_topo_bathy();

   free_maps();

   puts("Done. To use the merged height data, use option -h to hm:\n");
   printf("./hm -h %s 74.0 3.0 19.0 27.0 scandinavia\n\n", OUTFILE);

   return 0;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
