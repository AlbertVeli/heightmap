#ifndef __PARSEARGS_H
#define __PARSEARGS_H

/* All Rights Reversed - No Rights Reserved */

#include <stdbool.h>

struct opts
{
   /* Mandatory */
   long double latitude, longitude;
   long double span_w, span_h;
   char *basename;

   /* Optional */
   char *heightmap;
   char *texture;
   bool verbose;
};

/* Parsed options will be here */
extern struct opts opt;

extern bool parse_args(int argc, char **argv);
extern void print_usage(void);

#endif /* __PARSEARGS_H */

/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
