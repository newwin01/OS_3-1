#include "parse.h"

_dir * dir;
int dir_num = 0;

_reg * reg;
int reg_num = 0;

_inode_type_table * inode_type_table;
int table_num = 0;

void parse_json_obj_ent(json_object * json_obj, _dir * dir, int length)
{

	int type;

	dir->entries = malloc(sizeof(_entries)*length);

	for (int i = 0; i < length; i++) {
        json_object* element_obj = json_object_array_get_idx(json_obj, i);
        // enum json_type type = json_object_get_type(element_obj);
		json_object_object_foreach(element_obj, key_name, value_obj)
		{
			type = json_object_get_type(value_obj);

			if (type == json_type_int) dir->entries[i].inode = json_object_get_int(value_obj);
			else dir->entries[i].name = strdup(json_object_get_string(value_obj));

		}
		#ifdef _DEBUG
			fprintf(stderr, "%d\n", dir->entries[i].inode);
			fprintf(stderr, "%s\n", dir->entries[i].name);
		#endif
	}
}

void save_dir_json(int type, json_object *value_obj, int size){

	int entries_length;

	switch(type){ 
		case json_type_null:
			printf("Type is null\n");
			break;
		case json_type_int:
			dir[size].inode = json_object_get_int(value_obj);
			#ifdef _DEBUG
				fprintf(stderr, "%d %d\n", size, dir[size].inode);
			#endif
			break;
		case json_type_array:
			entries_length = json_object_array_length(value_obj);
			dir[size].number_of_entries = entries_length;
			parse_json_obj_ent(value_obj, &dir[size], entries_length);
	}
}

void save_reg_json(int type, json_object *value_obj, int size){

	switch(type){ 
		case json_type_null:
			printf("Type is null\n");
			break;
		case json_type_int:
			reg[size].inode = json_object_get_int(value_obj);
			break;
		case json_type_string:
			reg[size].data = strdup(json_object_get_string(value_obj));
	}
}



void parse_dir(json_object * json_obj)
{
	int type = 0;

	int size = ++dir_num ;
	dir = realloc(dir, sizeof(_dir) * size);

	json_object_object_foreach(json_obj, key_name, value_obj)
	{
		type = json_object_get_type(value_obj);
		save_dir_json(type, value_obj, size-1);
	}
}

void parse_reg(json_object * json_obj)
{
	int type = 0;

	int size = ++reg_num ;
	reg = realloc(reg, sizeof(_reg) * size);

	json_object_object_foreach(json_obj, key_name, value_obj)
	{
		if ( strcmp(key_name, "type") == 0) continue;

		type = json_object_get_type(value_obj);
		save_reg_json(type, value_obj, size-1);
	}
}


int get_json_type(json_object * json_obj){

	int type;

	json_object_object_foreach(json_obj, key_name, value_obj)
	{
		if ( strcmp(key_name, "inode") == 0) {
			type = json_object_get_int(value_obj);
		}

		if( strcmp(key_name, "type") == 0 ) {
			
			inode_type_table = realloc(inode_type_table, sizeof(_inode_type_table) * ++table_num);
			inode_type_table[table_num-1].inode = type;
			inode_type_table[table_num-1].type = strdup(json_object_get_string(value_obj));

			if ( strcmp(json_object_get_string(value_obj), "reg") == 0 ) { //regular file
				
				return 0 ;
			}
			else {

				return 1 ;
			}
		}
	}

	return -1;
}

void get_json(json_object * json_obj){

	int reg_or_dir;

    enum json_type type = json_object_get_type(json_obj);
    if (type == json_type_array) {
		
        int array_length = json_object_array_length(json_obj);
		
        for (int i = 0; i < array_length; i++) {

            json_object* element_obj = json_object_array_get_idx(json_obj, i);

			reg_or_dir = get_json_type(element_obj);

			if (reg_or_dir == 0 ) {//if it is regular file
				parse_reg(element_obj);
			}
			else {
				parse_dir(element_obj);
			}

        }
    }

}

void print() {

	for (int i = 0 ; i < dir_num; i++) { 
		
		fprintf(stderr, "%d\n", dir[i].inode);

		for (int j = 0 ; j < dir[i].number_of_entries ; j ++ ){
			fprintf(stderr, "%d\n", dir[i].entries[j].inode);
			fprintf(stderr, "%s\n", dir[i].entries[j].name);
		}

	}

	for (int i = 0 ; i < reg_num ; i++){

		fprintf(stderr, "%d %s\n", reg[i].inode, reg[i].data);

	}

	for (int i = 0 ; i < table_num ; i++){

		fprintf(stderr, "%d %s\n", inode_type_table[i].inode, inode_type_table[i].type);

	}

}