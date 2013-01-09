#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <png.h>

void save_png(FILE* fp, long int size)
{
    png_structp png_ptr = NULL;
    png_infop info_ptr = NULL;
    size_t x, y;
    png_bytepp row_pointers;

    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (png_ptr == NULL) {
        return ;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_write_struct(&png_ptr, NULL);
        return ;
    }

     if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_write_struct(&png_ptr, &info_ptr);
        return ;
    }

    png_set_IHDR(png_ptr, info_ptr,
                 size, size, // width and height
                 16, // bit depth
                 PNG_COLOR_TYPE_GRAY, // color type
                 PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

    /* Initialize rows of PNG. */
    row_pointers = (png_bytepp)png_malloc(png_ptr,
        size*png_sizeof(png_bytep));

    for (int i=0; i<size; i++)
       row_pointers[i]=NULL;

    for (int i=0; i<size; i++)
       row_pointers[i]=png_malloc(png_ptr, size*2);

    //set row data
    for (y = 0; y < size; ++y) {
        png_bytep row = row_pointers[y];
        for (x = 0; x < size; ++x) {
                short color = x+y;
                *row++ = (png_byte)(color & 0xFF);
                *row++ = (png_byte)(color >> 8);
        }
    }

    /* Actually write the image data. */
    png_init_io(png_ptr, fp);
    png_set_rows(png_ptr, info_ptr, row_pointers);
    png_write_png(png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);
    //png_write_image(png_ptr, row_pointers);

    /* Cleanup. */
    for (y = 0; y < size; y++) {
        png_free(png_ptr, row_pointers[y]);
    }
    png_free(png_ptr, row_pointers);
    png_destroy_write_struct(&png_ptr, &info_ptr);
}

int main()
{
  FILE* f;
  if((f=fopen("test.png", "wb"))!=NULL)
  {
    save_png(f, 257);

    fclose(f);
  }
  return 0;
}
