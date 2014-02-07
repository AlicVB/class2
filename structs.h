#ifndef STRUCTS_H
#define STRUCTS_H

#include <time.h>
  
typedef struct cl_file_t
{
  char jpg[1024];
  char raw[1024];
  char xmp[1024];
  
  char i1[1024];
  char i2[1024];
  char i3[1024];
  char i4[1024];
  
  char djpg[1024];
  char draw[1024];
  char dxmp[1024];
  
  time_t tt;
  int time_ok;
}
cl_file_t;

typedef struct cl_base_t
{
  cl_file_t* files[2048];
  int files_nb;
  
  char** preselect;
  int preselect_nb;
  
  GtkWidget* TBinfo1;
  GtkWidget* TBinfo2;
  GtkWidget* TBinfo3;
}
cl_base_t;

extern cl_base_t base;
#endif
