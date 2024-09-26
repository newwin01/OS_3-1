#include "lib/helper.h"
#include "lib/lib.h"
#include <sys/wait.h>
#include <time.h>
#include <sys/resource.h>
#include <fcntl.h>
#define RESULT_BUFFER_SIZE 4086
#define MILLI 1000000

typedef struct {
    int timeout_optinoT;
    char *testdir_optionI;
    char *solution;
    char *target;
    _tclist *tclist;
} cmd_input;

typedef struct {
    long long maximum_time;
    long long minimum_time;
    long long total_time;
} execution_time;

int compile_code ( char *source_code, char *binary_file) ;
cmd_input * cmd_option_parser (int argc, char *argv[]);
char * get_error_msg_using_pipe (char *binary_file, char *test_case, int time_limit);
void alrm_handler(int sig);
void free_cmd_input (cmd_input *input);