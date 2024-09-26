#include "lib/cimin_try.h"
#include "lib/structs.h"
#include "lib/helper.h"

#define ERROR_BUFFER_SIZE 4096

char * get_delta_debugged_head (int i, char *debugging_string, long remaining_length) {

    char *head;

    if (i == 0) { // consider the case of empty string
        return NULL;
    } else {
        head = malloc ( sizeof(char) * i );
        memcpy( head, debugging_string, i - 1 );
    }

    return head;
}

char * get_delta_debugged_tail (int i, char *debugging_string, long remaining_length) {

    char *tail;

    if ( i + remaining_length > sizeof(debugging_string) - 1) {
        return NULL;
    } else {
        tail = malloc ( sizeof(char) * (sizeof(debugging_string) ) );
        memcpy(tail, debugging_string + i + remaining_length, (sizeof(debugging_string) - 2) );
    }

    return tail;
}


char * get_error_msg_using_pipe (char *finding_error_msg, char **binary_options, char *debugging_string) {
    
    int pipe_to_parent[2];
    int pipe_to_child[2];
    char *error_msg;

    if (pipe(pipe_to_parent) != 0){
        fprintf ( stderr, "%s", "Parent pipe creation error" );
        exit(1);
    }
    if (pipe(pipe_to_child) != 0){
        fprintf ( stderr, "%s", "Child pipe creation error" );
        exit(1);
    }

    pid_t pid = fork();
    if (pid == -1) {
        fprintf (stderr, "%s", "Error on creating child process");
        exit(1);
    } 

    if (pid == 0) { //child process
    
        close (pipe_to_parent[0]); //close stdout from parent
        dup2 (pipe_to_parent[1], STDERR_FILENO); //standard error

        close (pipe_to_child[1]); //close stdin from parent
        dup2 (pipe_to_child[0], STDIN_FILENO);

        if ( execv(binary_options[0], binary_options) == -1 ) {
            fprintf (stderr, "%s", "Error on executing binary file on child process");
            exit(1);
        }
    }
    else { //parent process
        error_msg = malloc ( sizeof(char) * ERROR_BUFFER_SIZE);

        close(pipe_to_parent[1]); // close write to parent
        close(pipe_to_child[0]); // close read to child
    
        write(pipe_to_child[1], debugging_string, sizeof(debugging_string)); // write
        close(pipe_to_child[1]);

        if( read( pipe_to_parent[0], error_msg, ERROR_BUFFER_SIZE) > 0){
            fprintf(stderr, "This Error: %s\n", error_msg);
            close(pipe_to_parent[0]);
            return error_msg;
        } else {
            fprintf(stderr, "%s", "Return is Empty\n");
            close(pipe_to_parent[0]);
            free(error_msg);
            return NULL;
        }        
    }
    
    return NULL; //will not executed
}

char * execute_delta_debugging (char *file_contents, _cmd_input *cmd_input, char **binary_options) {

    char *debugging_string = malloc(sizeof(char) * strlen(file_contents));
    memcpy (debugging_string, file_contents, strlen(file_contents) ); //copy the contents

    fprintf(stderr, "%s", file_contents);
    fprintf(stderr, "%s", debugging_string);

    long remaining_length = sizeof(file_contents) - 1;

    char *head;
    char *tail;
    char *mid;
    char *error_msg;
    char *temp;

    while (remaining_length > 0) {
        
        for (int i = 0 ; i <= sizeof(debugging_string) - remaining_length; i++) {

            // check_char_array_initialization(head);
            // check_char_array_initialization(tail);

            head = get_delta_debugged_head(i, debugging_string, remaining_length);
            tail = get_delta_debugged_tail(i, debugging_string, remaining_length);

            // check_char_array_initialization(temp);
            temp = memory_concat(head, tail);

            // check_char_array_initialization(error_msg);
            error_msg = get_error_msg_using_pipe(cmd_input->error_file_name_optionI, binary_options, temp);

            if ( error_msg != NULL && ( strstr(error_msg, cmd_input->error_founding_optionM) != NULL ) ) {
                current_minimized_string = malloc ( sizeof(char) * sizeof(temp) );
                fprintf(stderr, "%s\n", current_minimized_string);
                memcpy (current_minimized_string, temp, sizeof(temp) * sizeof(temp[0]) );
                return execute_delta_debugging(temp, cmd_input, binary_options);
            }            
        } 
        for (int i = 0 ; i <= sizeof(debugging_string) - remaining_length ; i++ ) {
            
            // check_char_array_initialization(mid);
            mid = malloc ( (remaining_length) * sizeof(char) );
            memcpy(mid, debugging_string + i, remaining_length - 1);
            
            // check_char_array_initialization(error_msg);
            error_msg = get_error_msg_using_pipe(cmd_input->error_founding_optionM, binary_options, mid );

            if ( error_msg != NULL && ( strstr(error_msg, cmd_input->error_founding_optionM) != NULL ) ) {
                // free(current_minimized_string);
                current_minimized_string = malloc ( sizeof(char) * sizeof(mid) );
                memcpy (current_minimized_string, mid, sizeof(mid) * sizeof(mid[0]) );
                return execute_delta_debugging(mid, cmd_input, binary_options);
            }
        }
        remaining_length--;
    }
    return debugging_string;
}


int main(int argc, char *argv[])
{

    //if there is no necessary number of outputs
    if ( argc < 6 ) { 
        fprintf( stderr, "%s\n", "Error on getting options" );
        exit(1);
    }

    //getting options
    _cmd_input * cmd_input = cmd_option_parser(argv);
    char **execution_file_and_option = get_execution_file_and_option(argc, argv);

   //save file_content
    char *file_content = save_file_content(cmd_input->error_file_name_optionI);

    char *delta_debugged_min = execute_delta_debugging(file_content, cmd_input, execution_file_and_option);

    fprintf(stderr, "%s\n", delta_debugged_min);

    //freeing heap memories
    free_cmd_input (cmd_input);
    free_double_char_array (execution_file_and_option);
    free(file_content);

}