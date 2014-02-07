#ifndef UTILS_H
#define UTILS_H

char* substring(const char* str, size_t begin, size_t len);
bool fileexists(char* f);
bool direxists(char* d);
void mkdir2(const char *path);

int getallfiles(char* dir, char **files, char **plus, int plus_nb);
int getalldirs(char* dir, char **dirs, char **plus, int plus_nb);
int file_copy(char* src, char* dest);
char* get_dir(char* filename);
#endif
