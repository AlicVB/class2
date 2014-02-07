
#ifndef EXIV_H
#define EXIV_H

#include <time.h>

int xmp_read_infos(char* f, char* i1, char* i2, char* i3, time_t *tt);
int xmp_save_infos(char* f, char* i1, char* i2, char* i3, time_t *tt);
int exif_read_date(char* f, time_t *tt);

#endif

