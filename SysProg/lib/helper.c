#include "helper.h"
#include "lib.h"

void list_dir (char* dirpath, _tclist *tclist)
{
	DIR * dir = opendir(dirpath) ;
    long file_size;

	if (dir == 0x0)
		return ;

	for (struct dirent * i = readdir(dir) ; i != NULL ; i = readdir(dir)) {

		if (i->d_type != DT_DIR && i->d_type != DT_REG)
			continue ;

		char * filepath = (char *) malloc(strlen(dirpath) + 1 + strlen(i->d_name) + 1);
		strcpy(filepath, dirpath);
		strcpy(filepath + strlen(dirpath), "/");
		strcpy(filepath + strlen(dirpath) + 1, i->d_name);

		if (i->d_type == DT_DIR) {
			if (strcmp(i->d_name, ".") != 0 && strcmp(i->d_name, "..") != 0) 
				list_dir(filepath, tclist); //recursive function
		}

		else if (i->d_type == DT_REG) {
			
            tclist->file_num++;
            tclist->list_of_testcase = realloc (tclist->list_of_testcase , sizeof(char*) * tclist->file_num);
            tclist->list_of_testcase[tclist->file_num - 1] = strdup(filepath);
		}
		free(filepath) ;
	}

	closedir(dir) ;
}