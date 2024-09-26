#include "helper.h"

void free_double_char_array (char **input) {

    for (int i = 0 ; i < sizeof(input) / sizeof(char*) ; i++) {
        free(input[i]);
    }

    free(input);
}

int count_file_size (FILE *source_file) {

    long file_size;

    fseek(source_file, 0, SEEK_END);
    file_size = ftell(source_file);
    fseek(source_file, 0, SEEK_SET);

    return file_size;
}

char ** get_execution_file_and_option(int argc, char *argv[]) {

    int number_file_and_option = argc - 7;
    char **execution_file_and_option = malloc(sizeof(char*) * number_file_and_option);
    
    for ( int i = 0 ; i < number_file_and_option ; i++ ) {
        execution_file_and_option[i] = strdup (argv[i + 7] );
    }


    return execution_file_and_option;
}

char * save_file_content (char *opened_file) {

    FILE *source_file;
    char *file_content;
    int file_size;

    source_file = fopen (opened_file, "rb");
    if (source_file == NULL) {
        fprintf(stderr, "%s", "File Pointer Error");
        exit(1);
    }

    file_size = count_file_size (source_file);
    file_content = malloc( sizeof(char) * file_size);

    size_t source_bytes_read = fread(file_content, 1, file_size, source_file);

    fclose (source_file);

    return file_content;
}

char * memory_concat (char *string1, char *string2) {

    if (string1 == NULL) {
        free(string1);
        return string2;
    }
    if (string2 == NULL) {
        free(string2);
        return string1;
    }

    char *result_string = malloc ( sizeof(char) * (sizeof(string1) + sizeof(string2)) );

    memcpy (result_string, string1, sizeof(string1)*sizeof(string1[0]) );
    memcpy (result_string+sizeof(string1), string2, sizeof(string2)*sizeof(string2[0]) );

    free_two_string(string1, string2);

    return result_string;
}

void free_two_string (char *string1, char *string2) {

    free (string1);
    free (string2);
}

void check_char_array_initialization (char *string) {

    if (string == NULL) return;
    
    free(string);

}