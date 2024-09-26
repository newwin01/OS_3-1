#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void free_double_char_array (char **input);
int count_file_size (FILE *source_file);
char ** get_execution_file_and_option(int argc, char *argv[]);
char * save_file_content (char *opened_file);
char * memory_concat (char *string1, char *string2);
void free_two_string (char *string1, char *string2);
void check_char_array_initialization (char *string);