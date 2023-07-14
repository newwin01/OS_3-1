#include "parse.c"
#define MAX_LENGTH 500

typedef struct {
    int inode;
    char* path;
    int type; // 0 for reg, 1 for dir
} _path_from_root;

_path_from_root *path_from_root;
int path_from_root_number = 0;

char * parser = "/"; 
char * temp_root;

char* strcat_2src(const char* source1, const char* source2) {

    size_t len1 = strlen(source1);
    size_t len2 = strlen(source2);

    // Allocate memory for the destination string
    char* destination = (char*)malloc((len1 + len2 + 1) * sizeof(char));

    strcpy(destination, source1);

    strcat(destination, source2);

    return destination;
}

int parent(int inode){

    if (inode == 0) return -1;

    for ( int  i = 0 ; i < path_from_root_number ; i++){
        
        if (path_from_root[i].inode == inode) {
            return i;
        }

    }

    return -1;

}


int find_in_dir(int inode) {
    for (int i = 0 ; i < dir_num ; i++) {
        if (dir[i].inode == inode) {
            return i;
        }
    }

    fprintf(stderr, "%s", "Dir_not_found");
    return -1;
}

int find_in_reg(int inode) {
    for (int i = 0 ; i < reg_num ; i++) {
        if (reg[i].inode == inode) {
            return i;
        }
    }

    fprintf(stderr, "%s", "Reg_not_found");
    return -1;
}

/*
return inode type, if the return type is 0, regular file
*/
int get_inode_type(int inode) {

    for (int i = 0 ; i < table_num ; i++){

        if (inode_type_table[i].inode == inode) {
            if ( strcmp(inode_type_table[i].type, "reg") == 0 ){
                return 0;
            }
            if ( strcmp(inode_type_table[i].type, "dir") == 0 ){
                return 1;
            }
        }
    }

    return -1;

}

void make_path_from_root() {

    int temp_inode;
    int parent_inode;
    char * parent_path;
    int inode_type;
    

    for (int i = 0 ; i < dir_num ; i++) {

        for (int j = 0 ; j < dir[i].number_of_entries ; j++){

            temp_inode = dir[i].entries[j].inode;

            inode_type = get_inode_type(temp_inode);

            path_from_root = realloc(path_from_root, ++path_from_root_number * sizeof(_path_from_root));
            parent_inode = parent(dir[i].inode);
            path_from_root[path_from_root_number-1].inode = temp_inode;

            if (parent_inode == -1){
                                    
                parent_path = strdup("/");
                
                path_from_root[path_from_root_number-1].path = strdup(
                    strcat_2src(parent_path, dir[i].entries[j].name)); 
                                    
            } else {

                parent_path = strdup(path_from_root[parent_inode].path);
                path_from_root[path_from_root_number-1].path = strdup(
                    strcat_2src(parent_path, 
                    strcat_2src(parser, dir[i].entries[j].name) ));     
            }

            path_from_root[path_from_root_number-1].type = inode_type;
            
        }
        
    }
}

void tree_print() {

    for (int i = 0 ; i < path_from_root_number ; i++) {

        fprintf(stderr, "%d %s\n", path_from_root[i].inode, path_from_root[i].path);

    }

}

void split_at_last_stroke(const char* str, char* part1, char* part2) {
    
    const char* last_stroke = strrchr(str, '/');

    if (last_stroke != NULL) {
        
        strncpy(part1, str, last_stroke - str);
        part1[last_stroke - str] = '\0';

        
        strcpy(part2, last_stroke + 1);
    } else {
        
        strcpy(part1, str);
        strcpy(part2, "");
    }
}

void saved_as_json()
{

    FILE *file = fopen("data.json", "w");

    fputs("[\n", file);

    for (int i = 0 ; i < table_num ; i++){

        fprintf(file, "%s", "\t{"); 
        for (int j = 0 ; j < dir_num ; j++) { //dir

            if (dir[j].inode == i) {

                fprintf(file,"\n\t\t\"inode\": %d,\n\t\t\"type\": \"dir\",\n\t\t\"entries\":\n\t\t\t[\n"
                ,dir[j].inode);

                for (int k = 0 ; k < dir[j].number_of_entries ; k++) {
                    if ( k != dir[j].number_of_entries - 1 ){
                        fprintf(file,"\t\t\t{\"name\": \"%s\", \"inode\": %d},\n", dir[j].entries[k].name, dir[j].entries[k].inode);
                    } else {
                        fprintf(file,"\t\t\t{\"name\": \"%s\", \"inode\": %d}\n", dir[j].entries[k].name, dir[j].entries[k].inode);
                    }
                }
                fprintf(file, "%s", "\t\t\t]\n");
                break;
            }
        } 

        for (int j = 0 ; j < reg_num ; j++) { //dir

            if (reg[j].inode == i) {

                fprintf(file,"\n\t\t\"inode\": %d,\n\t\t\"type\": \"reg\",\n\t\t\"data\": \"%s\" \n"
                ,reg[j].inode, reg[j].data);

                break;
            }
        } 

        if (i != table_num-1){
            fprintf(file, "%s", "\t},\n");
        } else {
            fprintf(file, "%s", "\t}");
        }

    }

    fputs("\n]", file);

    fclose(file);

}