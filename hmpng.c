/* All Rights Reversed - No Rights Reserved */

#include "map.h"

#include <stdint.h>
#include <png.h>

void save_heightmap_png(int x1, int x2, int y1, int y2, const char *outfile)
{
   FILE *fp = NULL;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   size_t x, y, w, h, skip;
   png_bytepp row_pointers;
   png_bytep row;
   int16_t color;

   /* Pointer to humongous (7GB) MAPW x MAPH 16-bit map data (see map.c) */
   int16_t *mapp = (int16_t *)map;

   if (!(fp = fopen(outfile, "w"))) {
      perror(outfile);
      return;
   }

   w = x2 - x1 + 1;
   h = y2 - y1 + 1;

   /* Advance mapp to first pixel */
   mapp += (uint64_t)x1 + (uint64_t)MAPW * (uint64_t)y1;

   /* Skip this many int16_t:s to get to next line */
   skip = MAPW - w;

   /* libpng stuff below */

   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      return;
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      png_destroy_write_struct(&png_ptr, NULL);
      return;
   }

   if (setjmp(png_jmpbuf(png_ptr))) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
      return;
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
         color = *mapp++;
#if 1
         /* Clamp to negative heights to 0 */
         if (color < 0) {
            color = 0;
         }
#endif
         *row++ = (png_byte)(color & 0xff);
         *row++ = (png_byte)(color >> 8);
      }
      mapp += skip;
   }

   /* Write image data to fp */
   png_init_io(png_ptr, fp);
   png_set_rows(png_ptr, info_ptr, row_pointers);
   png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

   /* Cleanup. */
   for (y = 0; y < h; y++) {
      png_free(png_ptr, row_pointers[y]);
   }
   png_free(png_ptr, row_pointers);
   png_destroy_write_struct(&png_ptr, &info_ptr);

   fclose(fp);
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
