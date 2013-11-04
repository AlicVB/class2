#include <stdlib.h>
//#include <gtk/gtk.h>
#include <string.h>
#include <stdio.h>
//#include <ctype.h>

unsigned char isFile =0x8;
unsigned char isFolder =0x4;
DIR Dir;
struct dirent *DirEntry;
Dir = opendir("c:/test/")

while(Dir=readdir(Dir))
{
   if ( DirEntry->d_type == isFile)
   {
	cout <<"Found a File : " << DirEntry->d_name << endl;
   }
}





char* substring(const char* str, size_t begin, size_t len)
{
  if (str == 0 || strlen(str) == 0 || strlen(str) < begin || strlen(str) < (begin+len))
    return 0;

  return strndup(str + begin, len);
} 

bool fileexists(char* f)
{
  FILE * file = fopen(f, "r");
  if (!file) return false;
  fclose(file);
  return true; 
}
bool direxists(char* d)
{
  DIR* dir = opendir(d);
  if (dir)
  {
      closedir(dir);
      return TRUE;
  }
  return FALSE;
}

static void mkdir2(const char *path)
{
  char opath[256];
  char *p;
  size_t len;

  strncpy(opath, path, sizeof(opath));
  len = strlen(opath);
  if(opath[len - 1] == '/')
          opath[len - 1] = '\0';
  for(p = opath; *p; p++)
          if(*p == '/') {
                  *p = '\0';
                  if(access(opath, F_OK))
                          mkdir(opath, S_IRWXU);
                  *p = '/';
          }
  if(access(opath, F_OK))         /* if path is not terminated with / */
          mkdir(opath, S_IRWXU);
}
