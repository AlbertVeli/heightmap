#include "map.h"

/* All Rights Reversed - No Rights Reserved */

#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


/* Memory mapped data */
char *map[2] = { NULL, NULL };
static struct stat mapstat[2];

void free_map(int index)
{
   if (index != 0 && index != 1) {
      puts("index must be 0 or 1");
   } else {
      if (map[index]) {
         munmap(map[index], mapstat[index].st_size);
         map[index] = NULL;
      }
   }
}

void free_maps(void)
{
   free_map(0);
   free_map(1);
}

/* map[index] = mmap(filename)
 * index must be 0 or 1
 */
bool map_map(const char *filename, int index)
{
   bool ret = false;
   int fd = open(filename, O_RDONLY);
   
   if (fd < 0) {
      perror(filename);
      goto out;
   }

   if (fstat(fd, &mapstat[index]) < 0) {
      perror(filename);
      goto out2;
   }

   if (index != 0 && index != 1) {
      puts("index must be 0 or 1");
      goto out2;
   }

   map[index] = mmap(NULL, mapstat[index].st_size, PROT_READ, MAP_SHARED, fd, 0);
   if (map[index] == MAP_FAILED) {
      perror(filename);
      goto out2;
   }

   /* Free map automatically when program exits */
   atexit(free_maps);
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
