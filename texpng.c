/* All Rights Reversed - No Rights Reserved */

#include "map.h"

#include <stdint.h>
#include <string.h>
#include <png.h>

/* Edit the following lines to change image.
 * It should have the same latitude/longitude borders as
 * the heightmap image. See README.md.
 */
#define TEXFILE "world.topo.bathy.200410.3x21600x10800.png"

#define PNG_BYTES_TO_CHECK 8

/* Variables shared between read and write functions */
static png_size_t width;
static png_size_t height;
static png_byte bit_depth;
static png_byte color_type;
static int bpp;

/* Alloced in read, freed in cleanup (called at end of write) */
static png_structp png_rptr = NULL;
static png_infop info_rptr = NULL;
static png_bytepp row_pointers = NULL;


static void clean_up_read_mem(void)
{
   png_size_t y;

   if (row_pointers) {
      for (y = 0; y < height; y++) {
         free(row_pointers[y]);
      }
      free(row_pointers);
   }

   if (png_rptr) {
      png_destroy_read_struct(&png_rptr, &info_rptr, NULL);
   }
}

static bool read_texture_png(const char *filename)
{
   bool ret = false;
   FILE *fp = NULL;
   char buf[PNG_BYTES_TO_CHECK];
   png_size_t rowbytes;
   png_size_t y;

   if (!(fp = fopen(filename, "rb"))) {
      perror(filename);
      goto out2;
   }

   if (fread(buf, 1, PNG_BYTES_TO_CHECK, fp) != PNG_BYTES_TO_CHECK) {
      printf("%s is too short", filename);
      goto out2;
   }

   if (png_sig_cmp((png_bytep)buf, (png_size_t)0, PNG_BYTES_TO_CHECK)) {
      printf("%s is not a png file", filename);
      goto out2;
   }

   png_rptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_rptr == NULL) {
      goto out2;
   }

   info_rptr = png_create_info_struct(png_rptr);
   if (info_rptr == NULL) {
      png_destroy_read_struct(&png_rptr, NULL, NULL);
      goto out2;
   }

   if (setjmp(png_jmpbuf(png_rptr))) {
      goto out;
   }

   /* Get image info */
   png_init_io(png_rptr, fp);
   png_set_sig_bytes(png_rptr, PNG_BYTES_TO_CHECK);
   png_read_info(png_rptr, info_rptr);

   /* Save relevant info */
   bit_depth = png_get_bit_depth(png_rptr, info_rptr);
   bpp = (bit_depth >> 3) * png_get_channels(png_rptr, info_rptr);
   color_type = png_get_color_type(png_rptr, info_rptr);
   width = png_get_image_width(png_rptr, info_rptr);
   height = png_get_image_height(png_rptr, info_rptr);

   printf("bit_depth = %d\n", bit_depth);
   printf("w x h = %ld x %ld\n", width, height);
   printf("bytes per pixel = %d\n", bpp);

   /* Is this necessary? */
   png_set_interlace_handling(png_rptr);
   png_read_update_info(png_rptr, info_rptr);

   /* Allocate memory for pixel data */
   row_pointers = (png_bytep*) malloc(sizeof(png_bytep) * height);
   rowbytes = png_get_rowbytes(png_rptr, info_rptr);
   for (y = 0; y < height; y++) {
      row_pointers[y] = (png_byte*)malloc(rowbytes);
   }

   /* Read image into memory (2GB...) */
   png_read_image(png_rptr, row_pointers);

   ret = true;

out:

   /* Cleanup on fail (on success cleanup in save) */
   if (!ret) {
      clean_up_read_mem();
   }

out2:

   if (fp) {
      fclose(fp);
   }

   return ret;
}


bool save_texture_png(long double lat, long double lon, long double span_h, long double span_w, const char *outfile)
{
   int x1, x2, y1, y2;
   bool ret = false;
   FILE *fp = NULL;
   png_structp png_ptr = NULL;
   png_infop info_ptr = NULL;
   png_size_t dest_w, dest_h, row_offset, y;
   png_bytep drow, srow;

   printf("%s: %Lf° %c, %Lf° %c, span %Lf°, %Lf° -> %s\n", __FUNCTION__,
          lat < 0 ? -lat : lat, lat < 0 ? 'S' : 'N',
          lon < 0 ? -lon : lon, lon < 0 ? 'W' : 'E',
          span_h, span_w, outfile);

   if (!read_texture_png(TEXFILE)) {
      goto out2;
   }

   if (!(fp = fopen(outfile, "w"))) {
      perror(outfile);
      goto out;
   }

   /* libpng magic */
   png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
   if (png_ptr == NULL) {
      goto out;
   }

   info_ptr = png_create_info_struct(png_ptr);
   if (info_ptr == NULL) {
      goto out;
   }

   if (setjmp(png_jmpbuf(png_ptr))) {
      goto out;
   }

   png_init_io(png_ptr, fp);

   if (setjmp(png_jmpbuf(png_ptr))) {
      goto out;
   }

   /* Map longitude/latidude to pixel coordinates in source image */
   x1 = ((lon + 180.0) / (long double)360) * width;
   x2 = (((lon + span_w) + 180.0) / (long double)360) * width;
   y1 = ((90 - lat) / (long double)180) * height;
   y2 = ((90 - (lat - span_h)) / (long double)180) * height;
   dest_w = x2 - x1 + 1;
   dest_h = y2 - y1 + 1;
   printf("%dx%d pixels\n", (int)dest_w, (int)dest_h);

   /* write header */
   png_set_IHDR(png_ptr, info_ptr, dest_w, dest_h,
                bit_depth, color_type, PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
   png_write_info(png_ptr, info_ptr);

   /* copy smaller dest area to beginning of already alloced row_pointers */
   row_offset = x1 * bpp;
   dest_w = dest_w * bpp;
   for (y = 0; y < dest_h; y++) {
      drow = row_pointers[y];
      srow = row_pointers[y + y1];
      memmove(drow, &srow[row_offset], dest_w);
   }

   png_write_image(png_ptr, row_pointers);

   /* end write */
   png_write_end(png_ptr, NULL);

   ret = true;
   printf("Success\n");

out:

   /* Free mem alloced in read_texture_png */
   printf("Cleaning up\n");
   clean_up_read_mem();

   /* Free write structs (row_pointers already freed) */
   if (png_ptr) {
      png_destroy_write_struct(&png_ptr, &info_ptr);
   }

   if (fp) {
      fclose(fp);
   }

out2:

   return ret;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
