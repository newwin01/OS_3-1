#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <stdbool.h>
#include <pthread.h>
#include <semaphore.h>
#include <tgmath.h>
#include <dirent.h> 
#include <sys/time.h>

#define BUFFER_SIZE 64

typedef struct{ //file number
    int checked;
    char* file_name;
} _file_checker;

typedef struct{
    char* org_file;
    char* cmp_file;
    int cmp_index;
} _file_comparer;

typedef struct{
    char* org_file;
    char* cmp_file;
} _same_file;

typedef struct
{
    char** same_files;
    int same_files_num;
} _output;


_file_checker* file_checker; //global variable that stores every file name in the directory
_file_comparer* buffer[BUFFER_SIZE]; //buffer for producer and consumer

//for producer and consumer
int in = 0;
int out = 0;
pthread_mutex_t mutex;
pthread_cond_t full;
pthread_cond_t empty;
pthread_mutex_t comp_mutex;
pthread_mutex_t result_mutex;

//for file comparing
int cmp_file_index = 0;
int ori_file_index = 0;
int file_number = 0;

//for samefile
char *output_file_name;
int total_same_file = 0;
_same_file *same_file;

bool compare_files(_file_comparer* file_comparer) {

    FILE* fp1 = fopen(file_comparer->org_file, "r");
    FILE* fp2 = fopen(file_comparer->cmp_file, "r");
    
    if (fp1 == NULL || fp2 == NULL) {
        return false;
    }
    
    int ch1 = getc(fp1);
    int ch2 = getc(fp2);
    
    while (ch1 != EOF && ch2 != EOF) {
        if (ch1 != ch2) {
            fclose(fp1);
            fclose(fp2);
            return false;
        }
        
        ch1 = getc(fp1);
        ch2 = getc(fp2);
    }
    
    bool isSame = (ch1 == EOF && ch2 == EOF);
    
    fclose(fp1);
    fclose(fp2);
    
    return isSame;
}

void produce(_file_comparer* item){
    pthread_mutex_lock(&mutex);
    while ((in + 1) % BUFFER_SIZE == out) {
        // Buffer is full, wait for consumer to consume items
        pthread_cond_wait(&full, &mutex);
    }
    buffer[in] = item;
    in = (in + 1) % BUFFER_SIZE;
    pthread_cond_signal(&empty);  // Signal consumer that buffer is not empty
    pthread_mutex_unlock(&mutex);
}

_file_comparer *consume(){
    
    _file_comparer *item;
    pthread_mutex_lock(&mutex);
    while (in == out) {
        pthread_cond_wait(&empty, &mutex);
    }
    if(buffer[out] != NULL){
        item = buffer[out];
    }
    out = (out + 1) % BUFFER_SIZE;
    pthread_cond_signal(&full);  // Signal producer that buffer is not full
    pthread_mutex_unlock(&mutex);

    return item;
}

void * producer (void* arg){

    int* iter = (int*)arg;
    
    _file_comparer *item;

    while (ori_file_index + 1 < file_number || cmp_file_index + 1 < file_number) { // end condition
        
        item = (_file_comparer*) malloc(sizeof(_file_comparer));
        
        if (file_checker[cmp_file_index].checked == 1 || ori_file_index == cmp_file_index) cmp_file_index++;
        if (file_checker[ori_file_index].checked == 1) ori_file_index++;

        item->cmp_file = strdup(file_checker[cmp_file_index].file_name);
        item->org_file = strdup(file_checker[ori_file_index].file_name);

        produce(item);
        #ifdef _DEBUG
            fprintf(stderr, "%s %s\n", file_checker[ori_file_index].file_name, file_checker[cmp_file_index].file_name );
        #endif

        
        cmp_file_index++;
        if (cmp_file_index >= file_number){
            cmp_file_index = ori_file_index + 1;
            ori_file_index++;
        }
    }

    for (int i = 0 ; i < *iter ; i++){
        item = (_file_comparer*) malloc(sizeof(_file_comparer));
        item->org_file = NULL;
        item->cmp_file = NULL;
        produce(item);
    }
    
 
    return NULL;
}

void * consumer (void * arg){

    _file_comparer *item;
    
    while( (item = consume())->org_file != NULL ){
        
        if (compare_files(item)) {
            pthread_mutex_lock(&result_mutex);
            file_checker[item->cmp_index].checked = 1;
            // fprintf(stderr, "\n%s \t %s\n", item->org_file, item->cmp_file);

            total_same_file++;
            same_file = realloc(same_file, sizeof(_file_comparer) * total_same_file);

            same_file[total_same_file -1].org_file = strdup(item->org_file);
            same_file[total_same_file -1].cmp_file = strdup(item->cmp_file);

            // memcpy(same_file[total_same_file - 1].org_file, item->org_file, strlen(same_file[total_same_file - 1].org_file));
            // memcpy(same_file[total_same_file - 1].cmp_file, item->cmp_file, strlen(same_file[total_same_file - 1].cmp_file));

            pthread_mutex_unlock(&result_mutex);
            }
        else {
            #ifdef _DEBUG
                fprintf(stderr, "%s\n", "not same");
            #endif
            free(item);
        }
        

    }    

    return item;
}

long calculateFileSize(char* file_path) {

    long size;
    FILE* file = fopen(file_path, "r");

    fseek(file, 0, SEEK_END);    
    size = ftell(file);          
    
    fclose(file);

    return size;
}

/*References from 
    https://github.com/hongshin/LearningC/blob/master/fileio/primitives/list_r.c
*/
void list_dir (char * dirpath, int min_size)
{
	DIR * dir = opendir(dirpath) ;
    long file_size;

	if (dir == 0x0)
		return ;

	for (struct dirent * i = readdir(dir) ; i != NULL ; i = readdir(dir)) {
		if (i->d_type != DT_DIR && i->d_type != DT_REG)
			continue ;

		char * filepath = (char *) malloc(strlen(dirpath) + 1 + strlen(i->d_name) + 1) ;
		strcpy(filepath, dirpath) ;
		strcpy(filepath + strlen(dirpath), "/") ;
		strcpy(filepath + strlen(dirpath) + 1, i->d_name) ;

		if (i->d_type == DT_DIR) {

			if (strcmp(i->d_name, ".") != 0 && strcmp(i->d_name, "..") != 0) 
				list_dir(filepath, min_size) ; //recursive function
		}
		else if (i->d_type == DT_REG) {

            file_size = calculateFileSize(filepath);
            #ifdef _DEBUG
                fprintf(stderr, "%ld\n", file_size);
            #endif

            if(file_size < min_size) continue; // Ignore the file less than limited size
    
            file_number++;
            file_checker = realloc(file_checker, sizeof(_file_checker) * file_number);
            file_checker[file_number - 1].checked = 0;
            file_checker[file_number - 1].file_name = strdup(filepath);
		}
        
		free(filepath) ;
	}

	closedir(dir) ;
}

_output* MakeOutput(int *file_num) {

    _output *output = NULL;
    int file_number = 0;
    int k ;
    int flag = 0;
    
    for (int i = 0 ; i < total_same_file ; i++){
        flag = 0;
        for (k = 0 ; k < file_number; k++){
            if(flag == 1) continue;
            for (int j = 0 ; j < output[k].same_files_num; j++){
                if(flag == 1) continue;
                if ( strcmp(output[k].same_files[j], same_file[i].org_file) == 0) {
                    for (int z = j ; z < output[k].same_files_num; z++) {
                        if ( strcmp(output[k].same_files[z], same_file[i].cmp_file) == 0) {
                            flag = 1; //don't need to find anymore 
                            break;
                        }
                    }
                    if (flag == 0) {
                        output[k].same_files_num++;
                        output[k].same_files = realloc(output[k].same_files, sizeof(_output) * output[k].same_files_num);
                        output[k].same_files[output[k].same_files_num - 1] = strdup(same_file[i].cmp_file);
                        flag = 1;
                        break;
                    }

                } else if ( strcmp(output[k].same_files[j], same_file[i].cmp_file) == 0) {
                    for (int z = j ; z < output[k].same_files_num; z++) {
                        if ( strcmp(output[k].same_files[z], same_file[i].org_file) == 0) {
                            flag = 1; //don't need to find anymore 
                            break;
                        }
                    }
                    if (flag == 0) {
                        output[k].same_files_num++;
                        output[k].same_files = realloc(output[k].same_files, sizeof(_output) * output[k].same_files_num);
                        output[k].same_files[output[k].same_files_num - 1] = strdup(same_file[i].org_file);
                        flag = 1;
                        break;
                    }
                }
            }
        }

        if (flag == 0) {
            output = realloc(output, sizeof(_output) * ++file_number);
            output[file_number - 1].same_files_num = 2;
            output[file_number - 1].same_files = malloc(sizeof(char*) * 2);
            output[file_number - 1].same_files[0] = strdup(same_file[i].org_file);
            output[file_number - 1].same_files[1] = strdup(same_file[i].cmp_file);
        }
    }

    *file_num = file_number;

    return output;
}

void print(_output *output, int file_number) {

  
    if (output_file_name == NULL) {
        fprintf(stderr, "[\n");
        for (int i = 0 ; i < file_number ; i++) {
            fprintf(stderr, "\t[\n");
            for (int j = 0 ; j < output[i].same_files_num ; j++){
                fprintf(stderr, "\t%s \n", output[i].same_files[j]);
            }
            fprintf(stderr, "\t]\n");
        }
        fprintf(stderr, "\n]\n");
    } else {
        
        FILE *dest_file;
        dest_file = fopen(output_file_name, "wb");
        
        fprintf(dest_file, "[\n");
        for (int i = 0 ; i < file_number ; i++) {
            fprintf(dest_file, "\t[\n");
            for (int j = 0 ; j < output[i].same_files_num ; j++){
                fprintf(dest_file, "\t%s \n", output[i].same_files[j]);
            }
            fprintf(dest_file, "\t]\n");
        }
        fprintf(dest_file, "\n]\n");

        fclose(dest_file);

    }
    
}

void handler(int sig){ //wait for 3 seconds

    if(sig == SIGINT){
        fprintf(stderr, "\nCTRL C pressed\n");
        _output *output;

        int file_number;
        output = MakeOutput(&file_number);

        print(output, file_number);
        
        for (int i = 0 ; i < total_same_file ; i++) {
            free(same_file[i].cmp_file);
            free(same_file[i].org_file);
        }

        free(same_file);

        for (int i = 0 ; i < file_number ; i++) {

            for (int j = 0 ; j < output[i].same_files_num ; j++) {
                free(output[i].same_files[j]);
            }

            free(output[i].same_files);
        }
        free(output);
        exit(1);
    } else if(sig == SIGALRM){
            fprintf(stderr, "The number of identical files: %d\n\n", total_same_file);
            fprintf(stderr, "Currently searching: %s %s\n\n", file_checker[ori_file_index].file_name, file_checker[cmp_file_index].file_name);

        if (same_file != NULL) {

            #ifdef _DEBUG
                for (int i = 0 ; i < total_same_file ; i++) {
                    fprintf(stderr, "Same file: %s %s\n", same_file[i].org_file, same_file[i].cmp_file);
                }
            #endif

        } else {
            fprintf(stderr, "No same file yet");
        }
    }
}

int main(int argc, char *argv[])
{

    int t_flag = 0;
    int m_flag = 0;
    int o_flag = 0;
    int max_thread;
    int minimum_size;

    
    for (int i = 1 ; i < argc -1 ; i++){ //option parsing
        if (strcmp(argv[i], "-t") == 0){
            t_flag = 1;
            max_thread = atoi(argv[++i]);
        }
        if (strcmp(argv[i], "-m") == 0){
            m_flag = 1;
            minimum_size = atoi(argv[++i]);
        }
        if(argc > 6){
            if (strcmp(argv[i], "-o") == 0){ 
                o_flag = 1;
                output_file_name = malloc(strlen(argv[++i]));
                strcpy(output_file_name, argv[i]);
            }
        }
    }

    if(argc > 6){
        if(t_flag == 0 || m_flag == 0 || o_flag == 0){
            fprintf(stderr, "Required option exception");
            exit(1);
        }
    } else {
        if(t_flag == 0 || m_flag == 0){
            fprintf(stderr, "Required option exception");
            exit(1);
        }
    }

    char* dir;  

    if(max_thread <= 0){
        fprintf(stderr, "Not enough thread required");
        exit(1);
    } 
    if(max_thread == 1) {
        fprintf(stderr, "It is equal to sequential program");
        exit(1);
    }
    if (max_thread > 64){
        fprintf(stderr, "Thread is too big");
        exit(1);
    }


    dir = malloc(strlen(argv[argc-1]));
    strcpy(dir, argv[argc-1]);

    
    #ifdef _DEBUG
        fprintf(stderr, "%d %d %s\n", max_thread, minimum_size, dir);
    #endif   


    //read files on the folder
    list_dir(dir, minimum_size);


    #ifdef _DEBUG
        fprintf(stderr, "\n File: Number %d\n", file_number);
        for(int i = 0 ; i < file_number ; i++)  {
            fprintf(stderr, "\n%s %d\n", file_checker[i].file_name, file_checker[i].checked);
        }
    #endif

    //based on the heuristic

    int producer_thread;
    int consumer_thread;

    if (max_thread <= 4){ 
        producer_thread = 1;
        consumer_thread = max_thread - producer_thread;
    } else {
        
        producer_thread = roundf((float)max_thread*0.25);
        consumer_thread = roundf((float)max_thread*0.75);

        if (producer_thread + consumer_thread > max_thread) {
            producer_thread --;
        }

        //maximum producer thread is 5
        
        if (producer_thread > 5){
            consumer_thread = consumer_thread + producer_thread - 5;
            producer_thread = 5;
        }

    }

    clock_t start_time, end_time;
    double elapsed_time;

    pthread_t producer_t[producer_thread];
    pthread_t consumer_t[consumer_thread];

    in = 0;
    out = 0;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&full, NULL);
    pthread_cond_init(&empty, NULL);
    pthread_mutex_init(&comp_mutex, NULL);

    
    int* prod_arg = malloc(sizeof(int));
    *prod_arg = BUFFER_SIZE / producer_thread;

    int i;
    start_time = clock();

    for (i = 0 ; i < producer_thread ; i++) {
		pthread_create(&(producer_t[i]), NULL, producer, prod_arg) ;
	}

    for (i = 0 ; i < consumer_thread ; i++) {
        pthread_create(&(consumer_t[i]), NULL, consumer, NULL);
    }

    struct itimerval t ;  
    t.it_value.tv_sec = 5 ;
    t.it_value.tv_usec = 0;
    t.it_interval.tv_sec = 0;
    t.it_interval.tv_usec = 0;
    t.it_interval = t.it_value;

    setitimer(ITIMER_REAL, &t, NULL);

    signal(SIGINT, handler);
    signal(SIGALRM, handler);

    signal(SIGINT, handler) ;

    for (i = 0 ; i < producer_thread ; i++) {
        pthread_join(producer_t[i], NULL);
    }

    for (i = 0 ; i < consumer_thread ; i++) {
        pthread_join(consumer_t[i], NULL);
    }

    end_time = clock();
    _output *output;

    int file_number;
    output = MakeOutput(&file_number);

    print(output, file_number);

    elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Elapsed time: %.2f milliseconds\n", elapsed_time * 1000);
    
    
    for (int i = 0 ; i < total_same_file ; i++) {
        free(same_file[i].cmp_file);
        free(same_file[i].org_file);
    }

    free(same_file);

    for (int i = 0 ; i < file_number ; i++) {

        for (int j = 0 ; j < output[i].same_files_num ; j++) {
            free(output[i].same_files[j]);
        }

        free(output[i].same_files);
    }

    free(output);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&full);
    pthread_cond_destroy(&empty);
    pthread_mutex_destroy(&comp_mutex);

}