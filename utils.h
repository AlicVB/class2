#ifndef UTILS_H
#define UTILS_H

char* substring(const char* str, size_t begin, size_t len);
bool fileexists(char* f);
bool direxists(char* d);
void mkdir2(const char *path);

#endif
