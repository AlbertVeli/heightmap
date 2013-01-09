#ifndef __HMPNG_H
#define __HMPNG_H

/* All Rights Reversed - No Rights Reserved */

extern void save_heightmap_png(int x1, int x2, int y1, int y2, const char *outfile);
extern void save_texture_png(long double lat, long double lon, long double span_h, long double span_w, const char *outfile);

#endif /* __HMPNG_H */

/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
