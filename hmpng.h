#ifndef __HMPNG_H
#define __HMPNG_H

/* All Rights Reversed - No Rights Reserved */

extern bool save_heightmap_png(long double lat, long double lon, long double span_h, long double span_w, const char *outfile);
extern bool save_texture_png(long double lat, long double lon, long double span_h, long double span_w, const char *outfile);

#endif /* __HMPNG_H */

/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
