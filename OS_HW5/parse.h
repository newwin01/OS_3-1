#include <stdio.h>
#include <json.h>
#include <string.h>


// void parse_json_obj_ent(json_object * json_obj, _dir dir, int length);
// void parse_json_array(json_object* json_obj);

typedef struct {
    int inode;
    char *name;
} _entries;

typedef struct {
    int inode;
    char *data;
} _reg;

typedef struct 
{
    int inode;
    int number_of_entries;
    _entries *entries;
} _dir;

typedef struct
{
    int inode;
    char* type;
} _inode_type_table;
