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

/* Map byte in tif file (0 - 255) to -8000 - 0 */
#define MAP_BYTE_MINUS8000_0(val) ((int)((val) * (8000 / (double)255.0)) - 8000)

/* Reserve this many bytes for tif scanline */
#define MAXSCANLINE 43200
char scanline[MAXSCANLINE];
TIFF *tif = NULL;
uint32_t b_width, b_height;


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

   /* We already know the lowest point is -8000 because
    * we do the mapping from byte to -8000 - 0.
    */
   offset = 8000;

   printf("Merging files to %s, using offset %d meters\n", OUTFILE, offset);

   sp = (int16_t *)map[0];

   b_y = 0;
   TIFFReadScanline(tif, scanline, b_y++, 0);
   county = 0;

   for (y = 0; y < S_MAPH; y++) {
      count = 0;

      color = (uint8_t *)scanline;
      b_elevation = MAP_BYTE_MINUS8000_0(*color);

      for (x = 0; x < S_MAPW; x++) {

         /* Convert from big endian to host endian */
         s_elevation = ntohs(*sp++);

         /* Bathygraphy file seems to have 255 at land areas
          * which is mapped to 0 in b_elevation. Use that
          * "land mask".
          */
         if (b_elevation < 0) {
            /* b_elevation is already in host endian */
            elevation = b_elevation;
         } else {
            /* Land */
            if (s_elevation < 0) {
               /* crop negative land areas to 0 */
               elevation = 0;
            } else {
               /* positive land area */
               elevation = s_elevation;
            }
         }

         /* Add offset and convert to big endian */
         elevation = htons(elevation + offset);

         /* Write elevation (2 bytes) to destination file */
         if (fwrite(&elevation, 2, 1, fp) != 1) {
            /* Error. Probably out of diskspace. */
            fprintf(stderr, "Error writing to %s, is disk full?\n", OUTFILE);
            fclose(fp);
            return false;
         }

         /* Step up bathy every other pixel (exactly half dimension) */
         if (count == 1) {
            count = 0;
            b_elevation = MAP_BYTE_MINUS8000_0(*color++);
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
