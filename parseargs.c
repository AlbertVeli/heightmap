/* All Rights Reversed - No Rights Reserved */

#include "parseargs.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define PROGRAM_NAME "hm"
#define PROGRAM_VERSION "0.1"
#define PROGRAM_NAME_VERSION PROGRAM_NAME "-" PROGRAM_VERSION

/* Filled in by parse_args */
struct opts opt;

/* Program documentation. */
static char *prog_doc =
     "\n" PROGRAM_NAME_VERSION
     "\nProgram to extract heightmap and texture data of an\n"
     "area specified by latitude/longitude.\n\n"
     "USAGE\n\n" PROGRAM_NAME
     " [OPTIONS] "
     "<latitude> <longitude> <latitude range> <longitude range> <basename>\n\n"
     "Use decimal fractions for latitude/longitude/spans.\n"
     "latitude (North/South) in range 90 (° N) to -90 (° S).\n"
     "longitude (West/East) in range -180 (° W) to 180 (° E).\n\n"
     "MANDATORY ARGUMENTS\n\n"
     "latitude/longitude - upper left corner of the wanted area.\n"
     "latitude/longitude span - height and width of wanted area in degrees.\n"
     "basename - common beginning of the filenames to save output to.\n\n"
     "OPTIONS\n\n"
     "-h <FILE> - Datafile for heightmap instead of default\n"
     "-t <FILE> - Datafile for texture instead of default\n"
     "-v        - Verbose output\n\n"
     "EXAMPLE\n\n"
     "To generate images scandinavia_heightmap.png and scandinavia_texture.png\n"
     "for the Scandinavia area (74° N, 3° E, span 19°, 27°), run:\n\n "
     "./" PROGRAM_NAME " 74 3 19 27 scandinavia\n\n";

void print_usage(void)
{
   printf("%s", prog_doc);
}

bool parse_args(int argc, char **argv)
{
   int c;

   /* Default values. */
   opt.heightmap = "datafiles/srtm_ramp2.world.86400x43200.bin";
   opt.texture = "datafiles/world.topo.bathy.200410.3x21600x10800.png";
   opt.verbose = false;

   /* Parse arguments */
   opterr = 0;

   while ((c = getopt (argc, argv, "vh:t:")) != -1) {

      switch (c) {

      case 'v':
         opt.verbose = true;
         break;

      case 'h':
         opt.heightmap = optarg;
         break;

      case 't':
         opt.texture = optarg;
         break;

      case '?':
         fprintf(stderr, "Error: unknown option -%c\n\n", optopt);
         return false;

      default:
         return false;
      }
   }

   if (argc != optind + 5) {
      puts("Error: wrong number of mandatory arguments");
      return false;
   }

   /* Convert degrees to floats */
   opt.latitude = strtold(argv[optind++], NULL);
   if (opt.latitude < -90 || opt.latitude > 90) {
      puts("Error: latitude out of range");
      return false;
   }

   opt.longitude = strtold(argv[optind++], NULL);
   if (opt.longitude < -180 || opt.longitude > 180) {
      puts("Error: longitude out of range");
      return false;
   }

   opt.span_h = strtold(argv[optind++], NULL);
   if (opt.span_h < 0 || opt.span_h > 180) {
      puts("Error: latitude span out of range");
      return false;
   }

   opt.span_w = strtold(argv[optind++], NULL);
   if (opt.span_w < 0 || opt.span_w > 360) {
      puts("Error: longitude span out of range");
      return false;
   }

   if (opt.latitude - opt.span_h < -180) {
      puts("Error: latitude - latitude span below bottom edge");
      return false;
   }

   if (opt.longitude + opt.span_w > 360) {
      puts("Error: longitude + longitude span outside right edge");
      return false;
   }

   opt.basename = argv[optind];

   if (opt.verbose) {
      printf("%Lf° %c, %Lf° %c, span %Lf°, %Lf°\n",
             opt.latitude < 0 ? -opt.latitude : opt.latitude, opt.latitude < 0 ? 'S' : 'N',
             opt.longitude < 0 ? -opt.longitude : opt.longitude, opt.longitude < 0 ? 'W' : 'E',
             opt.span_h, opt.span_w);

      printf ("basename: %s\n"
              "datafiles:\n - %s\n - %s\n\n",
              opt.basename,
              opt.heightmap,
              opt.texture);
   }

   return true;
}


/**
 * Local Variables:
 * mode: c
 * indent-tabs-mode: nil
 * c-basic-offset: 3
 * End:
 */
