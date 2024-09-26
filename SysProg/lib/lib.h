#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

char * two_string_concat (char *string1, char *string2);
int check_c_file(char *string);
int check_file_exist(char *filename);
char * read_file_contents (char *filepath);
int isdigit_char_array (char *string);
void remove_file_exists (char *filepath);
char * free_ptr (char *ptr);