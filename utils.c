#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <gio/gio.h>

int getallfiles(char* dir, char **files, char **plus, int plus_nb)
{
  unsigned char isFile =0x8;
  unsigned char isFolder =0x4;
  DIR *Dir = NULL;
  struct dirent *DirEntry;
  Dir = opendir(dir);
  int nb = 0;
  
  if (Dir)
  {
    while(DirEntry=readdir(Dir))
    {
       if ( DirEntry->d_type == isFile)
       {
         strncpy(files[nb],DirEntry->d_name,512);
         nb++;
       }
    }
  }
  
  //now we look for files inside the "plus"
  if (!plus) return nb;
  char dir2[2048]="";
  snprintf(dir2,2048,"%s",dir);

  for (int i=0; i<plus_nb; i++)
  {
    char *pre = strstr(plus[i],dir2);
    if (pre == plus[i])
    {
      char suf[2048]="";
      strncpy(suf,plus[i]+strlen(dir2),2048);
      //we just want files
      if (!strstr(suf,"/"))
      {
        strncpy(files[nb],suf,512);
        nb++;
      }
    }
  }
  
  return nb;
}

int getalldirs(char* dir, char **dirs, char **plus, int plus_nb)
{
  unsigned char isFile =0x8;
  unsigned char isFolder =0x4;
  DIR *Dir = NULL;
  struct dirent *DirEntry;
  Dir = opendir(dir);
  int nb = 0;
  
  if (Dir)
  {
    while(DirEntry=readdir(Dir))
    {
       if ( DirEntry->d_type == isFolder)
       {
         strncpy(dirs[nb],DirEntry->d_name,512);
         nb++;
       }
    }
  }

  //now we look for files inside the "plus"
  if (!plus) return nb;
  char dir2[2048]="";
  snprintf(dir2,2048,"%s",dir);
  for (int i=0; i<plus_nb; i++)
  {
    char *pre = strstr(plus[i],dir2);
    if (pre == plus[i])
    {
      char suf[2048]="";
      strncpy(suf,plus[i]+strlen(dir2),2048);
      //we don't want files
      char *dd = strstr(suf,"/");
      if (dd)
      {
        strncpy(dirs[nb],suf,strlen(suf)-strlen(dd));
        nb++;
      }
    }
  }
  
  return nb;
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
      return true;
  }
  return false;
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
                  if(access(opath, F_OK)) mkdir(opath, S_IRWXU);
                  *p = '/';
          }
  if(access(opath, R_OK))         /* if path is not terminated with / */
          mkdir(opath, S_IRWXU);
}

static char* get_dir(char* filename)
{
  char* f = strrchr(filename,'/');
  if (!f) return '\0';
  return strndup(filename,strlen(filename)-strlen(f));
}

/*void copy_progress(goffset current_num_bytes, goffset total_num_bytes, gpointer user_data)
{
  if (current_num_bytes >= total_num_bytes)
  {
    
  }
}
*/

int file_copy(char* src, char* dest)
{
  //we check if the src and dest exists
  if (!fileexists(src)) return 10;
  if (fileexists(dest)) return 11;
  
  //if the dest directory doen't exist, we create it
  char* d = get_dir(dest);
  if (!direxists(d)) mkdir2(d);
  if (!direxists(d)) return 15;
  
  bool rep = g_file_copy (g_file_new_for_path (src), g_file_new_for_path (dest), G_FILE_COPY_ALL_METADATA, NULL, NULL, NULL, NULL);
  
  //char cmd[2048]="";
  //sprintf( cmd, "/bin/cp -p \'%s\' \'%s\'", src,dest);
  //system( cmd);
  
  //we test that the copy has been done
  if (!rep) return 22;
  if (!fileexists(dest)) return 20;
  
  //and that the file sizes are sames
  struct stat st;
  stat(src, &st);
  int size1 = st.st_size;
  stat(dest, &st);
  int size2 = st.st_size;
  
  if (size1 != size2) return 21;
  
  return 1;
}
