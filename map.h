#ifndef __MAP_H
#define __MAP_H

/* All Rights Reversed - No Rights Reserved */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Memory mapped data of heightmap/bathymetry */
extern char *map[2];

/* srtm_ramp2.world.86400x43200.bin */
#define S_MAPW 86400
#define S_MAPH 43200

/* gebco_bathy.21601x10801.bin */
#define B_MAPW 21601
#define B_MAPH 10801

extern bool map_map(const char *filename, int index);
extern void free_map(int index);
extern void free_maps(void);

#endif /* __MAP_H */

/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
