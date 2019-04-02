#include <fstream>
#include "monster_parser.h"

#define FILE_PATH "/.rlg327/"

void parse_monsters(const char *file_name, heap_t *mh){
  char *directory = getenv("HOME");

  char *path = (char *)malloc(strlen(directory) + strlen(FILE_PATH) +  strlen(file_name) + 1);
  strcpy(path, directory);
  strcat(path, FILE_PATH);
  strcat(path, file_name);

  ifstream f(path);

}