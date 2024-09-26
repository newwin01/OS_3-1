#include "lib.h"

int isdigit_char_array (char *string) {

    int length = strlen(string);

    for (int i = 0 ; i < length ; i++) {
        if( isdigit(string[i]) == 0) {
            return 0;
        }
    }

    return 1; //true
}

int check_c_file(char *string) {
    int length = strlen(string);

    if (length >= 2 && string[length - 2] == '.' && string[length - 1] == 'c') {
        return 1;
    }

    return 0; 
}

int check_file_exist(char *filename) {
    struct stat fileInfo;

    if (stat(filename, &fileInfo) != 0) {
        return 0;
    }

    return 1;
}

char * two_string_concat (char *string1, char *string2) {

    if (string1 == NULL) {
        return string2;
    }
    if (string2 == NULL) {
        return string1;
    }

    char *result_string = malloc ( sizeof(char) * (strlen(string1) + strlen(string2)) );

    strcpy ( result_string, string1 );
    strcpy ( result_string + strlen(string1), string2 );

    return result_string;
}

char * read_file_contents (char *filepath) {

    FILE *file;

    file = fopen(filepath, "r"); // Open the file in read mode

    if (file == NULL) {
        printf("Unable to open the file.\n");
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    char *contents = (char *)malloc(file_size + 2); 

    size_t bytes_read = fread(contents, 1, file_size, file);
    contents[bytes_read] = '\n';

    fclose(file); // Close the file
    
    return contents;
}

void remove_file_exists (char *filepath)  {
    
    FILE *file;

    file = fopen(filepath, "r");

    if (!file) return;

    if (file) {
        fclose(file);
    }

    if (remove(filepath) == 0) {
    } else {
        fprintf(stderr, "%s\n", "Error deleting file");
    }

}

char * free_ptr (char *ptr) {
    if (ptr != NULL) {
        free(ptr);
        ptr = NULL; // Set the pointer to NULL after freeing
    } else {
        fprintf(stderr, "Pointer is already NULL.\n");
    }
    
}