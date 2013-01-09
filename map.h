#ifndef __MAP_H
#define __MAP_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/* Memory mapped data of heightmap */
extern char *map;

/* These defines are valid for the biggest heightmap here:
 * http://visibleearth.nasa.gov/view.php?id=73934
 * Change the values if a different heightmap is used.
 */
#define MAPW 86400
#define MAPH 43200
#define XDPP (360/(long double)MAPW)
/* YDPP should be same as XDPP if map is correctly mapped */
#define YDPP (180/(long double)MAPH)

extern bool map_map(const char *filename);
extern void free_map(void);

#endif /* __MAP_H */

/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
