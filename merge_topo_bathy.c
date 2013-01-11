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

#if 0
void find_extremes(char *mp, int w, int h)
{
   int x, y;
   int16_t *p = (int16_t *)mp;
   int16_t elevation, min, max;

   min = 0;
   max = 0;

   for (y = 0; y < h; y++) {
      for (x = 0; x < w; x++) {
         elevation = ntohs(*p++);
         if (elevation < min) {
            min = elevation;
         }
         if (elevation > max) {
            max = elevation;
         }
      }
   }

   printf(" - min: %d\n", min);
   printf(" - max: %d\n", max);
}
#endif

void merge_topo_bathy(void)
{
   FILE *fp;
   int x, y;
   int16_t *sp, *bp;
   int16_t s_elevation, b_elevation, elevation;
   int count, county;

   if (!(fp = fopen(OUTFILE, "w"))) {
      perror(OUTFILE);
      return;
   }

   sp = (int16_t *)map[0];
   bp = (int16_t *)map[1];

   county = 0;
   for (y = 0; y < S_MAPH; y++) {
      count = 0;
      b_elevation = ntohs(*bp);

      for (x = 0; x < S_MAPW; x++) {

         s_elevation = ntohs(*sp++);

         /* Take the biggest (positive or negative) */
         if (abs(s_elevation) > abs(b_elevation)) {
            elevation = htons(s_elevation);
         } else {
            elevation = htons(b_elevation);
         }

         /* Write elevation (2 bytes) to destination file */
         fwrite(&elevation, 2, 1, fp);

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

   return 0;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
