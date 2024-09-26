#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

#define BUF_SIZE 2000

int fork_testing(int pid){
    return 0;
}

void handler(int sig){ //wait for 3 seconds

    if(sig == SIGINT){
        fprintf(stdout, "\nCTRL C pressed\n");
    }
    if(sig == SIGALRM){
        fprintf(stdout, "Time over\n");
    }
    exit(1);
}


char* fileopen(char* inputfile_name){

    FILE *source_file;


    source_file = fopen(inputfile_name, "rb");
    if(source_file == NULL){
        fprintf(stderr, "File Pointer Error");
        exit(1);
    }


    // Determine the size of the source file
    fseek(source_file, 0, SEEK_END);
    long source_size = ftell(source_file);
    fseek(source_file, 0, SEEK_SET);

    char *source_data = (char *) malloc(source_size);
    if (source_data == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    // Read the source image data into the allocated memory
    size_t source_bytes_read = fread(source_data, 1, source_size, source_file);
    if (source_bytes_read != source_size) {
        perror("fread");
        exit(EXIT_FAILURE);
    }


    return source_data;
}


int
main(int argc, char *argv[])
{

    char *input_error;
    char *new_file_path;
    char *crashing_input_file;

    char *binary_file; //binary file and the options;
    char **binary_options = NULL;  

    int i_flag = 0;
    int m_flag = 0;
    int o_flag = 0;

    if(argc < 7) {
        fprintf(stderr, "Error on option\n");
        exit(1);
    }
    //Get option using getopt
    int opt;
    while((opt = getopt(7, argv, "i:m:o:")) != -1){

        switch (opt)
        {
        // -i option is followed by a file path of the crashing input
        case 'i':
            i_flag = 1;
            crashing_input_file = malloc(sizeof(char)*strlen(optarg));
            strcpy(crashing_input_file, optarg);
            #ifdef DEBUG
                printf("%s\n", crashing_input_file);
            #endif
            break;

        //-m option is followed by a string whose appearance in standard error determines whether the expected crash occurs or not.
        case 'm': 
            m_flag = 1;
            input_error = malloc(sizeof(char)*strlen(optarg));
            strcpy(input_error,optarg); //store the error message that we received
            #ifdef DEBUG
                printf("%s\n", input_error);
            #endif
            break;

        //-o option is followed by a new file path to store the reduced crashing input.
        case 'o':
            o_flag = 1;
            new_file_path = malloc(sizeof(char)*strlen(optarg));
            strcpy(new_file_path,optarg); //store the file path to store new one
            #ifdef DEBUG
                printf("%s\n", new_file_path);
            #endif
            break;
        default:
            fprintf(stderr, "Error on getting path info");
            exit(1);
        }
    }

    if(i_flag == 0 || m_flag == 0 || o_flag == 0){
        fprintf(stderr, "Option exception");
        exit(1);
    }
    
    // get bianry file name and option
    int count = 0;
    if(optind < argc){
        binary_file = (char*)malloc(sizeof(char)*strlen(argv[optind]));
        strcpy(binary_file, argv[optind++]);
        if(binary_file == NULL){
            fprintf(stderr, "Not enough memory");
            exit(1);
        }
        if(optind < argc) {
            binary_options = (char**)malloc(sizeof(char*)*(argc-optind+1));
        }
        while(optind < argc){
            binary_options[count] = (char*)malloc(sizeof(char)*strlen(argv[optind]));
            if(binary_options[count] == NULL) {
                printf("Not enough memory");
                exit(1);
            }
            strcpy(binary_options[count++], argv[optind++]);
        }
        if(binary_options!=NULL){
            argv[optind+1] = NULL;
        }
        #ifdef DEBUG
            printf("%s\n", binary_file);
            for(int i=0;i<count;i++){
                printf("%s\n", binary_options[i]);
            }
        #endif
    } else {
        fprintf(stderr, "binary file is not provided\n");
        exit(1);
    }

    // ==========================================================Pre Processing======================================================================== //
    
    //pipe
    //0 -> stdin 1 -> stderr
    int to_parent[2];
    int to_child[2];
    

    #ifdef FILEDEBUG
        int j = 0;
        while(buffer[j] != NULL){ 
            printf("%s", buffer[j]);
            j++;
        }
    #endif



    struct itimerval t ;
    signal(SIGALRM, handler) ;
    signal(SIGINT, handler) ;

    t.it_value.tv_sec = 3 ;
    t.it_value.tv_usec = 0;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    t.it_interval = t.it_value;
    
    
    if (pipe(to_parent) != 0){
        perror("Pipe Error occured");
        exit(1);
    }
    if (pipe(to_child) != 0){
        perror("Pipe Error occured"); 
        exit(1);
    }

    
    int status;
    // char* buffer = fileopen(crashing_input_file);
    char temp_buffer[BUF_SIZE];
        
    // #ifdef DEBUG
    // for (int i = 0; i < strlen(buffer); i++) {
    //     printf("%c", buffer[i]);
    // }
    // #endif


    FILE *source_file;

    source_file = fopen(crashing_input_file, "rb");
    if(source_file == NULL){
        fprintf(stderr, "File Pointer Error");
        exit(1);
    }

    // Determine the size of the source file
    fseek(source_file, 0, SEEK_END);
    long source_size = ftell(source_file);
    fseek(source_file, 0, SEEK_SET);

    char *source_data = (char *) malloc(source_size);
    if (source_data == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    // Read the source image data into the allocated memory
    size_t source_bytes_read = fread(source_data, 1, source_size, source_file);
    if (source_bytes_read != source_size) {
        perror("fread");
        exit(EXIT_FAILURE);
    }

    // FILE *dest_file = fopen("dest.png", "wb");
    // if (dest_file == NULL) {
    //     perror("fopen");
    //     exit(EXIT_FAILURE);
    // }

    // // Write the source image data to the destination file
    // size_t dest_bytes_written = fwrite(source_data, 1, source_size, dest_file);
    // if (dest_bytes_written != source_size) {
    //     perror("fwrite");
    //     exit(EXIT_FAILURE);
    // }

    // Close the destination file
    // fclose(dest_file);

    int pid;
    pid = fork();
    if(pid == -1){
        fprintf(stderr, "Error on creating child process");
    }
    else if(pid==0){
        #ifdef DEBUG
            printf("%s %s", binary_file, crashing_input_file) ;
        #endif

        // to use parent stderr
        close(to_parent[0]); 
        close(to_child[1]);

        dup2(to_parent[1], STDOUT_FILENO);
        dup2(to_parent[1], STDERR_FILENO);
        //to send child stdin
        
        dup2(to_child[0], STDIN_FILENO);

        if(execv(binary_file, binary_options) == -1){
            fprintf(stderr, "Execution error");
        }
    } 
    else {
        //parent process
        close(to_parent[1]); //close write to parent
        close(to_child[0]); //close read to child
        
        
      

        // write(to_child[1], buffer, strlen(buffer)); 
        ssize_t bytes_written = write(to_child[1], source_data, source_size);
        if (bytes_written != source_size) {
            perror("write");
            printf("Error Not occur");
            free(source_data);          
            return EXIT_FAILURE;
        }

        //close to not wait
        close(to_child[1]);

        waitpid(pid, NULL, 0);
        
        ssize_t num_read;
        while ((num_read = read(to_parent[0], temp_buffer, sizeof(temp_buffer))) > 0) {
            fwrite(temp_buffer, 1, num_read, stderr);
        }
        close(to_parent[0]);   
        #ifdef PNG
            for (int i = 0; i < source_size+12; i++) {
                printf("%02x ", (unsigned char)source_data[i]);
            }
        #endif
        
    }

    
    
    // End of the execution
    // Need to free the char array that has been created
    free(input_error);
    free(new_file_path);
    free(crashing_input_file);
    if(count > 1 ){
        for(int i=0;i<count;i++){
            free(binary_options[i]);
        }
        free(binary_options);
    }
    
    // free(buffer);
    free(binary_file);
        
    return 0;
}