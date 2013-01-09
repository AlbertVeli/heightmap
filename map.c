#include "map.h"

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/* Memory mapped data */
char *map = NULL;
static struct stat mapstat;

void free_map(void)
{
   if (map) {
      munmap(map, mapstat.st_size);
   }
}

bool map_map(const char *filename)
{
   bool ret = false;
   int fd = open(filename, O_RDONLY);
   
   if (fd < 0) {
      perror(filename);
      goto out;
   }

   if (fstat(fd, &mapstat) < 0) {
      perror(filename);
      goto out2;
   }

   map = mmap(0, mapstat.st_size, PROT_READ, MAP_SHARED, fd, 0);
   if (map == MAP_FAILED) {
      perror(filename);
      goto out2;
   }

   /* Free map automatically when program exits */
   atexit(free_map);
   ret = true;

 out2:

   close(fd);

 out:

   return ret;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
