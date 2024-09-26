#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <getopt.h>

typedef struct {
    int file_num;
    char **list_of_testcase;
} _tclist;

void list_dir (char * dirpath, _tclist *tclist);