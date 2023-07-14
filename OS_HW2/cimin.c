#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/wait.h>

#define ERROR_BUFFER_SIZE 4096

char* current_minimized;
char* new_file_path;
char* buffer;
pid_t pid;
long source_size; //need to control on the delta debugging

void handler(int sig){ //wait for 3 seconds

    if(sig == SIGINT){
        fprintf(stderr, "\nCTRL C pressed\n");
        kill(pid, SIGTERM);
        FILE *dest_file;
        dest_file = fopen(new_file_path, "wb");
        fwrite(current_minimized, 1, source_size, dest_file);
        fclose(dest_file);
        exit(1);
    } else if(sig == SIGALRM){
        kill(pid, SIGTERM);
        fprintf(stderr, "\nTime over\n");
        exit(1);
    }
}

char* error_return (char* binary_file, char** binary_options, char* input){

    int to_parent[2];
    int to_child[2];
    int status;
    char* error_buffer;

    struct itimerval t ;  
    t.it_value.tv_sec = 3 ;
    t.it_value.tv_usec = 0;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    t.it_interval = t.it_value;

    setitimer(ITIMER_REAL, &t, NULL);

    
    if (pipe(to_parent) != 0){
        perror("Pipe Error occured");
        exit(1);
    }
    if (pipe(to_child) != 0){
        perror("Pipe Error occured"); 
        exit(1);
    }


    

    #ifdef DEBUG
        printf("\nPID : %d\n\n", pid);
    #endif
    pid = fork();
    if(pid == -1){
        fprintf(stderr, "Error on creating child process");
    }
    else if(pid==0){
        
        #ifdef DEBUG
            printf("binary file: %s\n\n", binary_file);
        #endif

        close(to_parent[0]);
        dup2(to_parent[1], STDERR_FILENO);
        

        // Redirect stdin from the pipe
        close(to_child[1]);
        dup2(to_child[0], STDIN_FILENO);
        
        #ifdef DEBUG
            fprintf(stderr, "binary file: %s", binary_file);
        #endif
        if (execv(binary_file, binary_options) == -1) {
            perror("execv failed");
            exit(1);
        }
    } 
    else {
        error_buffer = malloc(sizeof(char) * ERROR_BUFFER_SIZE);
        //parent process
        signal(SIGALRM, handler) ;
        signal(SIGINT, handler) ;
        
        //parent process
        close(to_parent[1]); // close write to parent
        close(to_child[0]); // close read to child
        
        
        if(input == NULL) return NULL;
    
        write(to_child[1], input, strlen(input)); // write
        close(to_child[1]);
        
        
        waitpid(pid, &status, 0);

        #ifdef DEBUG
            printf("Child PID : %d\n\n", pid);
        #endif

        if(read(to_parent[0], error_buffer, ERROR_BUFFER_SIZE)>0){
            #ifdef DEBUG
            fprintf(stderr, "This Error: %s\n", error_buffer);
            #endif
        } else {
            #ifdef DEBUG
            fprintf(stderr, "%s", "Return is Empty\n");
            #endif
            free(error_buffer);
            return NULL;
        }
        
        close(to_parent[0]);
        
        
    }
    // printf("%s", error_buffer);
    return error_buffer;
}

char* deltadebugging (char* finding_error, char *test_input, char* binary_file, char** binary_options){
    
    char* t = malloc(sizeof(char)*strlen(test_input));  //assign the test_input to reduced input
    
    strcpy(t, test_input);
    long s = strlen(test_input) - 1 ;  
    char* head;
    char* tail;
    char* error_msg;
    char *mid;
    while (s > 0) {
        
        for (int i = 0; i <= strlen(t) - s; i++) {

            if(i==0){ // consider the case of empty string
                head=NULL;
            }
            if(i!=0){
                head = malloc((i + 1) * sizeof(char));
                strncpy(head, t, i);
                head[i-1] = '\0';
            }
 
            if(i + s > strlen(t) - 1){ //consider the case of empty string
                tail = NULL;
            }else{
                tail = malloc((strlen(t) + 1 ) * sizeof(char));
                strncpy(tail, t + i + s, strlen(t) - 1);
                tail[strlen(t)-1] = '\0';
            }

            if(head == NULL){ // consider the empty string
                error_msg = error_return(binary_file, binary_options, tail);
            } else if (tail == NULL){
                error_msg = error_return(binary_file, binary_options, head);
            } else {
                error_msg = error_return(binary_file, binary_options, strcat(head, tail));                  
            }
            
            if(error_msg != NULL){ // Error message is contained
                #ifdef DEBUG
                fprintf(stderr, "%s", error_msg);
                #endif
                if (strstr(error_msg, finding_error) != NULL) {
                    if(head == NULL){
                        strcpy(current_minimized,tail);
                        source_size = (strlen(tail));
                        return deltadebugging(finding_error, tail, binary_file, binary_options);
                    } else if(tail == NULL){
                        strcpy(current_minimized,head);
                        source_size = (strlen(head));
                        return deltadebugging(finding_error, head, binary_file, binary_options);
                    } else {
                        strcpy(current_minimized,strcat(head, tail));
                        source_size = (strlen(strcat(head, tail)));
                        return deltadebugging(finding_error, strcat(head, tail), binary_file, binary_options);
                    }
                }
            }


            if(head != NULL){
                #ifdef DEBUG
                fprintf(stderr, "This is head %s \n", head);
                #endif
                free(head); 
                head = NULL;
            }
            if(tail != NULL){
                #ifdef DEBUG
                fprintf(stderr, "This is tail %s \n", tail);
                #endif
                free(tail); 
                tail = NULL;
            }
            if(error_msg != NULL){
                free(error_msg);
                error_msg = NULL;
            }
            #ifdef DEBUG
            fprintf(stderr, "%s\n", "iteration");
            #endif
            
        }
        for (int i = 0; i <= strlen(t) - s; i++) {
            #ifdef DEBUG
            fprintf(stderr, "mid start");
            #endif
            
            if(i> i+s-1){ //not going to happen, but safety reason
                fprintf(stderr, "%s", "Mid Array error");
                exit(1);
            }

            mid = malloc((s+1)*sizeof(char));
            strncpy(mid, t+i, s-1);
            mid[s-1] = '\0';
            #ifdef DEBUG
                fprintf(stderr, mid);
            #endif
        
            error_msg = error_return(binary_file, binary_options, mid);

            if(error_msg != NULL){
                #ifdef DEBUG
                fprintf(stderr, "%s", error_msg);
                #endif
                if(strstr(error_msg, finding_error) != NULL){
                    strcpy(current_minimized,mid);
                    source_size = (strlen(mid));
                    return deltadebugging(finding_error, mid, binary_file, binary_options);
                }
            }
        if(mid!=NULL){
            free(mid);
            mid = NULL;
        }
        if(error_msg!=NULL){
            free(error_msg);
            error_msg = NULL;
        }
        }
        s--;
    }
  return t;
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
    source_size = ftell(source_file);
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

    fclose(source_file);
    return source_data;
}


int
main(int argc, char *argv[])
{

    char *finding_error = NULL;
    new_file_path = NULL; 
    char *crashing_input_file = NULL;

    char *binary_file = NULL; //binary file and the options;
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
            finding_error = malloc(sizeof(char)*strlen(optarg));
            strcpy(finding_error,optarg); //store the error message that we received
            #ifdef DEBUG
                printf("%s\n", finding_error);
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

//getting options --------------------------------------------------
    int count = 0;
    if(optind < argc){
        binary_file = (char*)malloc(sizeof(char)*strlen(argv[optind]));
        strcpy(binary_file, argv[optind++]);
        if(binary_file == NULL){
            fprintf(stderr, "Not enough memory");
            exit(1);
        }
        
        if(optind < argc) {
            binary_options = (char**)malloc(sizeof(char*)*(argc-optind+2));
            binary_options[count] = (char*)malloc(sizeof(char)*strlen(argv[optind-1]));
            #ifdef DEBUG
            fprintf(stderr, argv[optind-1]);
            #endif
            strcpy(binary_options[count++], argv[optind-1]);
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
            #ifdef DEBUG
                printf("binary option is not empty\n");

            #endif
            binary_options[count] = NULL;
        }
        #ifdef DEBUG
            printf("%s\n", binary_file);
            printf("%d\n", count);
            for(int i=0;i<count;i++){
                printf("%s\n", binary_options[i]);
            }
        #endif
    } else {
        fprintf(stderr, "binary file is not provided\n");
        exit(1);
    }

//==========================================================Options======================================================================== //
    
    //pipe
    char *output;
    
    buffer = fileopen(crashing_input_file);
    current_minimized = malloc(sizeof(char)*strlen(buffer));
    strcpy(current_minimized, buffer);
        
    #ifdef DEBUG
    for (int i = 0; i < strlen(buffer); i++) {
        printf("%c", buffer[i]);
    }
    #endif

    #ifdef PNG
        for (int i = 0; i < strlen(buffer); i++) {
        printf("%02x ", (unsigned char)buffer[i]);
    }
    #endif
    
    output = deltadebugging(finding_error, buffer, binary_file, binary_options);

    // sleep(3);
    printf("\nOUTPUT: \n%s\n", output);
    printf("Minimized Input: %s\n", current_minimized);

    
    FILE *dest_file;
    dest_file = fopen(new_file_path, "wb");
    fwrite(output, 1, strlen(output) , dest_file);
    fclose(dest_file);


    #ifdef _FILETEST
    FILE *temp;
    printf("%ld", temp_buffer_length);
    temp = fopen("test_file", "w");
    fwrite(error_buffer, 1,temp_buffer_length , temp);
    fclose(temp);
    #endif
    #ifdef FILEDEBUG
    FILE *dest_file;
    dest_file = fopen(new_file_path, "wb");
    fwrite(buffer, 1, source_size , dest_file);
    fclose(dest_file);
    #endif
    
    //End of the execution
    //Need to free the char array that has been created
    free(finding_error);
    free(new_file_path);
    free(crashing_input_file);
    if(count > 1 ){
        for(int i=0;i<count;i++){
            free(binary_options[i]);
        }
        free(binary_options);
    }
    
    free(binary_file);
    // free(buffer); handling global variable
    // free(current_minimized);
    
    return 0;
}