#include "main.h"

pid_t kill_pid;

cmd_input * cmd_option_parser (int argc, char *argv[])
{

    if ( argc != 7 ) {
        fprintf (stderr, "%s\n", "Invalid Input");
    }

    int cmd_options; //value used in getopt function
    cmd_input *input = (cmd_input*)malloc( sizeof(cmd_input) );
    input->tclist = (_tclist*)malloc( sizeof(_tclist) );
    input->tclist->file_num = 0;

    int temp_timeout;

    while ( (cmd_options = getopt( 5, argv, "i:t:" ) ) != -1 ) {
        switch (cmd_options) {
            case 'i' : //test directory root
                if ( opendir ( optarg ) == NULL) {
                    fprintf( stderr, "%s\n", "Open Dir Error" );
                    exit(1);
                }
                input->testdir_optionI = strdup(optarg);

                list_dir (optarg, input->tclist);

                break;

            case 't' : // timeout limit

                if ( isdigit_char_array (optarg) && (temp_timeout = atoi( optarg )) > 0 && temp_timeout <= 10 ) {
                    input->timeout_optinoT = temp_timeout;
                } else {
                    fprintf( stderr, "%s\n", "Invalid Timeout input" );
                    exit(1);
                }
                break;

            default:
                fprintf( stderr, "%s\n", "Error on getting options" );
                exit(1);
        }
    }

    if ( check_file_exist (argv[5]) == 0 || check_file_exist (argv[6]) == 0 ) {
        fprintf( stderr, "%s\n", "File not exists" );
        exit(1);
    }

    if ( check_c_file (argv[5]) == 0 || check_c_file (argv[6]) == 0 ) {
        fprintf( stderr, "%s\n", "Input is not c file" );
        exit(1);
    }

    input->solution = strdup( argv[5] );
    input->target = strdup( argv[6] );

    return input;
}

int compile_code ( char *source_code, char *binary_file) 
{  
    int status;
    pid_t child_pid;

    child_pid = vfork();

    if (child_pid == -1) {
        fprintf (stderr, "%s", "Error on creating child process");
        exit(1);
    }

    if (child_pid == 0) { //solution child process

        char *argv[] = {"gcc", "-o", binary_file, source_code, "lib/file_limit.c", NULL};

        remove_file_exists(binary_file);

        if ( execvp("gcc", argv) == -1 ) { //TODO: hard coding?
            fprintf (stderr, "%s\n", "Error on executing binary file on child process");
            exit(1);
        }

    } else {

        waitpid(child_pid, &status, 0);

        if ( status > 0 ) {
            fprintf(stderr, "%s\n", "Compile Error");
            return -1;
        }

    }

    return 1;
}

void alrm_handler(int sig)
{
    if(sig == SIGALRM)
    {
        kill(kill_pid,SIGKILL);
    }
}

char * get_error_msg_using_pipe (char *binary_file, char *test_case, int time_limit) { // 0 is solution 1 is target

    if ( check_file_exist(binary_file) == 0) return "Compile Error";
    
    int pipe_to_parent[2];
    int pipe_to_child[2];
    int status;

    struct rlimit limit;

    if (pipe(pipe_to_parent) != 0){
        fprintf ( stderr, "%s", "Parent pipe creation error" );
        exit(1);
    }
    if (pipe(pipe_to_child) != 0){
        fprintf ( stderr, "%s", "Child pipe creation error" );
        exit(1);
    }

    pid_t child_pid;

    child_pid = vfork();

    if (child_pid == -1) {
        fprintf (stderr, "%s", "Error on creating child process");
        exit(1);
    }

    if (child_pid == 0) { //solution child process

        close (pipe_to_parent[0]); //close stdout from parent
        close (STDOUT_FILENO);
        dup2 (pipe_to_parent[1], STDOUT_FILENO); //standard out

        close (pipe_to_child[1]); //close stdin from parent
        close (STDIN_FILENO);
        dup2 (pipe_to_child[0], STDIN_FILENO); //standard input

        limit.rlim_cur = 4; // Limit the number of file descriptors to 3
        limit.rlim_max = 4;

        if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
            fprintf(stderr, "%s\n", "setrlimit");
            exit(1);
        }

        if ( execl( binary_file, binary_file, NULL) == -1 ) { //TODO: hard coding?
            fprintf (stderr, "%s\n", "Error on executing binary file on child process");
            exit(1);
        }

    } else { //parent process

        char *result_message = NULL;
        char *filecontents = read_file_contents(test_case);
        struct timespec start_time, end_time;

        if (filecontents == NULL) {
            fprintf(stderr, "%s\n", "File Open Error");
            exit(1);
        }

        close(pipe_to_parent[1]); // close write to parent
        close(pipe_to_child[0]); // close read to child
    
        write(pipe_to_child[1], filecontents, strlen(filecontents)); // write to solution 
        close(pipe_to_child[1]);

        kill_pid = child_pid;
        signal(SIGALRM, alrm_handler);
        alarm(time_limit);
        
        waitpid(child_pid, &status, 0); 
        alarm(0);

        if (status > 0) { //can use status to identify what type of error it is if it is requried
            close(pipe_to_parent[0]);
            return "Testcase failed";
        }

        result_message = (char *)malloc ( sizeof(char) * RESULT_BUFFER_SIZE );
        memset(result_message, 0x0, RESULT_BUFFER_SIZE);

        if( read( pipe_to_parent[0], result_message, RESULT_BUFFER_SIZE) > 0 ){ 
            close(pipe_to_parent[0]);
        }

        fprintf(stderr, "%s", result_message);

        return result_message ;
    }

    return NULL; //will not be executed
}

void free_cmd_input (cmd_input *input) {

    for (int i = 0 ; i < input->tclist->file_num ; i++) {
        free(input->tclist->list_of_testcase[i]);
    }
    free(input->tclist->list_of_testcase);
    free(input->solution);
    free(input->target);
    free(input->testdir_optionI);
    free(input);

}

int main(int argc, char *argv[])
{

    cmd_input *input = cmd_option_parser (argc, argv);
    execution_time *ex_time = (execution_time*)malloc(sizeof(execution_time));
    ex_time->maximum_time = 0;
    ex_time->minimum_time = MILLI;
    ex_time->total_time = 0;

    char *result_solution;
    char *result_target;

    int result;
    int correct_num_executions = 0;

    struct timespec start_time, end_time;
    long long elapsed_ms;

    compile_code (input->solution, "bin/solution"); //assume there is no error on solution code
    result = compile_code(input->target, "bin/target");

    if (result == -1) {
        // fprintf (stderr, "%s", "Compile Error");
    } else {
        for (int i = 0 ; i < input->tclist->file_num ; i++ ) { //with assumption that solution will provide answer correctly always
            
            result_solution = get_error_msg_using_pipe("bin/solution",input->tclist->list_of_testcase[i], input->timeout_optinoT);
            clock_gettime(CLOCK_MONOTONIC, &start_time); 
            result_target =  get_error_msg_using_pipe("bin/target",input->tclist->list_of_testcase[i], input->timeout_optinoT);
            clock_gettime(CLOCK_MONOTONIC, &end_time);
            elapsed_ms = (end_time.tv_sec - start_time.tv_sec) + (end_time.tv_nsec - start_time.tv_nsec);
            elapsed_ms = elapsed_ms/MILLI;

            if ( strcmp(result_solution, result_target) == 0 ) {
                fprintf( stdout, "testcase %d: %s\n", i, "Testcase matched");
                if (ex_time->maximum_time < elapsed_ms) {
                        ex_time->maximum_time = elapsed_ms;
                    }
                    if (ex_time->minimum_time > elapsed_ms) {
                        ex_time->minimum_time = elapsed_ms;
                    }

                    ex_time->total_time += elapsed_ms;
                    correct_num_executions++;
            } else {
                fprintf( stdout, "%s", "testcase unmatched. \n");
                fprintf( stdout, "testcase %d result: %s\n", i, result_target);  
            }
            // free(result_solution);
        }
    }

    fprintf(stdout, "Correct Number of execution %d\nFailed Number of execution %d\n", correct_num_executions, input->tclist->file_num - correct_num_executions);
    fprintf(stdout, "Maximum Time: %lldms\nMinimum Time: %lldms\nTotal Time: %lldms\n", ex_time->maximum_time, ex_time->minimum_time, ex_time->total_time);

    free_cmd_input(input);
    free(ex_time);

    return 0;
}