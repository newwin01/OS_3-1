#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <string.h>

typedef struct command_line_input {
    char *error_file_name_optionI; 
    char *error_founding_optionM;
    char *output_file_optionO;
} _cmd_input;

void free_cmd_input (_cmd_input *cmd_input);
_cmd_input *cmd_option_parser (char *argv[]);