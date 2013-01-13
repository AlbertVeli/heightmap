/*
 * Same as merge_topo_bathy.c, but uses geotiff
 * image for bathygraphy.
 *
 * All Rights Reversed - No Rights Reserved
 */

#include "map.h"

#include <stdint.h>
#include <png.h>
#include <arpa/inet.h> /* ntohs */

/* Install libtiff to get this one */
#include <tiffio.h>

/* Datafiles. Dimensions are in map.h */
#define SRTMFILE "datafiles/srtm_ramp2.world.86400x43200.bin"
#define BATHYFILE "datafiles/gebco_bathy.43200x21600.0-8000_geo.tif"
#define OUTFILE "datafiles/merged_topo_bathy.86400x43200.bin"

/* Reserve this many bytes for tif scanline */
#define MAXSCANLINE 43200
char scanline[MAXSCANLINE];
TIFF *tif = NULL;
uint32_t b_width, b_height;

int16_t min = 0;
int16_t max = 0;


int open_bathy_tiff(void)
{
   uint32_t bps, spp;
   bool ret = false;

   /* This will raise a couple of warnings because
    * libtiff doesnt recognize the GeoTIFF tags.
    * No problem, we already know that the image
    * starts at 90° N, 180° W at 30 arcseconds per pixel.
    *
    * Use the utility listgeo (from libgeotiff) if you
    * want to list the geotags yourself.
    */
   printf("Ignore the three warnings below\n\n");
   tif = TIFFOpen(BATHYFILE, "r");
   if (!tif) {
      return false;
   }
   printf("\n");

   /* Read some metadata about the image. */
   TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &b_width);
   TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &b_height);
   if (b_width != 43200 || b_height != 21600) {
      printf("%dx%d\n", b_width, b_height);
      printf("expected 43200x21600, probably wrong file\n");
      goto out;
   }

   TIFFGetField(tif, TIFFTAG_BITSPERSAMPLE, &bps);
   TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
   if (bps * spp != 8) {
      printf("%d bits per sample, %d samples per pixel\n", bps, spp);
      printf("expected 8 bits per sample and 1 sample per pixel\n");
      goto out;
   }

   if (TIFFScanlineSize(tif) > MAXSCANLINE) {
      printf("scanline too long\n");
      goto out;
   }

   /* Everything ok */
   ret = true;

 out:

   return ret;
}


void close_bathy_tiff(void)
{
   /* close the TIFF file descriptor */
   TIFFClose(tif);
}


void find_extremes(char *mp, int w, int h)
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

bool merge_topo_bathy(void)
{
   FILE *fp;
   int x, y;
   int16_t *sp;
   int16_t s_elevation, b_elevation, elevation;
   int count;
   int16_t offset;
   uint32_t b_y, county;
   uint8_t *color;

   if (!(fp = fopen(OUTFILE, "w"))) {
      perror(OUTFILE);
      return false;
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

   b_y = 0;
   TIFFReadScanline(tif, scanline, b_y++, 0);
   county = 0;

   for (y = 0; y < S_MAPH; y++) {
      count = 0;

      color = (uint8_t *)scanline;
      /* Map 0 - 255 to -8000 - 0:
       * (color - 0) / 255 = (b_elevation - -8000) / 8000
       */
      b_elevation = (int16_t)((*color) * (8000 / (double)255.0)) - 8000;

      for (x = 0; x < S_MAPW; x++) {

         s_elevation = ntohs(*sp++);

         /* Bathygraphy file seems to have 255 at land areas */
         if (*color != 255) {
            elevation = b_elevation;
         } else {
            elevation = htons(s_elevation);
         }

         /* Write elevation (2 bytes) to destination file */
         elevation += offset;
         if (fwrite(&elevation, 2, 1, fp) != 1) {
            /* Error. Probably out of diskspace. */
            fprintf(stderr, "Error writing to %s, is disk full?\n", OUTFILE);
            fclose(fp);
            return false;
         }

         /* Step up bathy every other pixel (exactly half dimension) */
         if (count == 1) {
            count = 0;
            b_elevation = (int16_t)((*color++) * (8000 / (double)255.0)) - 8000;
         } else {
            count++;
         }

      }

      /* Step up bathy scanline every other line */
      if (county == 1) {
         county = 0;
         TIFFReadScanline(tif, scanline, b_y++, 0);
      } else {
         county++;
      }

   }

   fclose(fp);

   return true;
}


int main(void)
{
   /* Memory map srtm elevation file */
   if (!map_map(SRTMFILE, 0)) {
      free_maps();
      return 1;
   }

   /* Open bathygraphy tiff file */
   if (!open_bathy_tiff()) {
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

   if (merge_topo_bathy()) {
      puts("Done. To use the merged height data, use option -h to hm:\n");
      printf("./hm -h %s 74.0 3.0 19.0 27.0 scandinavia\n\n", OUTFILE);
   }

   close_bathy_tiff();
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
