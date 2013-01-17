/* All Rights Reversed - No Rights Reserved */

#include "map.h"
#include "hmpng.h"

#include <stdint.h>
#include <png.h>
#include <arpa/inet.h> /* ntohs */

bool save_heightmap_png(long double lat, long double lon, long double span_h, long double span_w, const char *outfile)
{
   int x1, x2, y1, y2;
   bool ret = false;
   FILE *fp = NULL;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   size_t x, y, w, h, skip;
   png_bytepp row_pointers;
   png_bytep row;
   int16_t elevation, height;

   /* Pointer to humongous (7GB) S_MAPW x S_MAPH 16-bit map data (see map.c) */
   int16_t *mapp = (int16_t *)map[0];

   /* Map longitude/latidude to pixel coordinates in source image */
   x1 = ((lon + 180.0) / (long double)360) * S_MAPW;
   x2 = (((lon + span_w) + 180.0) / (long double)360) * S_MAPW - 1;
   y1 = ((90 - lat) / (long double)180) * S_MAPH;
   y2 = ((90 - (lat - span_h)) / (long double)180) * S_MAPH - 1;

   w = x2 - x1 + 1;
   h = y2 - y1 + 1;

   printf("%s: %dx%d ... ", outfile, (int)w, (int)h);
   fflush(stdout);

   /* Advance mapp to first pixel */
   mapp += (uint64_t)x1 + (uint64_t)S_MAPW * (uint64_t)y1;

   /* Skip this many int16_t:s to get to next line */
   skip = S_MAPW - w;

   /* libpng stuff below */

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      goto out;
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      png_destroy_write_struct(&png_ptr, NULL);
      goto out;
   }

   if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      goto out;
   }

   /* Specify 16-bit grayscale in png header */
   png_set_IHDR(png_ptr, info_ptr,
                w, h,
                16,
                PNG_COLOR_TYPE_GRAY,
                PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);


   /* Alloc mem for png image data */
   row_pointers = (png_bytepp)png_malloc(png_ptr, h * png_sizeof(png_bytep));
   for (y = 0; y < h; y++) {
      row_pointers[y] = png_malloc(png_ptr, w * 2);
   }

   /* Copy image data from mapp to row_pointers */
   for (y = 0; y < h; y++) {
      row = row_pointers[y];
      for (x = 0; x < w; x++) {

         /* Read 2 big endian source bytes */
         elevation = *mapp++;

         /* Clamp negative heights to 0 */
         height = ntohs(elevation); /* swap bytes if little endian */
         if (height < 0) {
            elevation = 0;
         }

         /* Write 2 bytes to destination */
         *row++ = (png_byte)(elevation & 0xff);
         *row++ = (png_byte)(elevation >> 8);
      }
      mapp += skip;
   }

   /* Write image data to fp */

   if (!(fp = fopen(outfile, "w"))) {
      perror(outfile);
      goto out2;
   }

   png_init_io(png_ptr, fp);
   png_set_rows(png_ptr, info_ptr, row_pointers);
   png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

   ret = true;

 out2:

   /* Cleanup. */
   for (y = 0; y < h; y++) {
      png_free(png_ptr, row_pointers[y]);
   }
   png_free(png_ptr, row_pointers);
   png_destroy_write_struct(&png_ptr, &info_ptr);

   if (fp) {
      fclose(fp);
   }

 out:

   if (ret) {
      puts("ok");
   } else {
      puts("fail");
   }

   return ret;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
