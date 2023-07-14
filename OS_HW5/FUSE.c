#define FUSE_USE_VERSION 26
#define MAX_FILE_NAME 64

#include <stdio.h>
#include <fuse.h>
#include <errno.h>
#include "helper.c"

static int getattr_callback(const char *path, struct stat *stbuf) { //stat update
  memset(stbuf, 0, sizeof(struct stat));

  int in_reg;

  if (strcmp(path, "/") == 0) {
    stbuf->st_mode = S_IFDIR | 0755;
    stbuf->st_nlink = 2;
    return 0;
  }

  for (int i = 0 ; i < path_from_root_number ; i++) {
    if ( path_from_root[i].type == 0) continue;

    if (strcmp(path, path_from_root[i].path) == 0) {
      stbuf->st_mode = S_IFDIR | 0755;
      stbuf->st_nlink = 2;
      return 0;
    }
  }

    for (int i = 0 ; i < path_from_root_number ; i++) {
      if ( path_from_root[i].type == 1) continue;

      if (strcmp(path, path_from_root[i].path) == 0) {
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        in_reg = find_in_reg(path_from_root[i].inode);
        stbuf->st_size = strlen(reg[in_reg].data);
  
        return 0;
      }
  }

  
  return -ENOENT;
}

static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
    off_t offset, struct fuse_file_info *fi) { //read dir

    int seq;

    if (strcmp(path, "/") == 0) {  //root directory 
      filler(buf, ".", NULL, 0);
      filler(buf, "..", NULL, 0);
      for (int i = 0 ; i < dir[0].number_of_entries ; i++) {
        filler(buf, dir[0].entries[i].name, NULL, 0);
      }
      return 0;
    } else {
      for (int i = 0 ; i < path_from_root_number ; i++) {
        if ( path_from_root[i].type == 0) continue;

        if (strcmp(path, path_from_root[i].path) == 0) {
          filler(buf, ".", NULL, 0);
          filler(buf, "..", NULL, 0);
          seq = find_in_dir(path_from_root[i].inode);
          for (int i = 0 ; i < dir[seq].number_of_entries ; i++) {
            filler(buf, dir[seq].entries[i].name, NULL, 0);
          }
        }
      }
      return 0;
    }

  return -ENOENT ;
}

static int open_callback(const char *path, struct fuse_file_info *fi) {
  return 0;
}

static int read_callback(const char *path, char *buf, size_t size, off_t offset,
    struct fuse_file_info *fi) { //read

  int in_reg;

  for (int i = 0 ; i < path_from_root_number ; i++) {
    
    if (path_from_root[i].type == 1) continue;

      if (strcmp(path, path_from_root[i].path) == 0) {
      
      in_reg = find_in_reg(path_from_root[i].inode);

      size_t len = strlen(reg[in_reg].data);
      if (offset >= len) {
        return 0;
      } 

      if (offset + size > len) {
        memcpy(buf, reg[in_reg].data + offset, len - offset);
        return len - offset;
      }

      memcpy(buf, reg[in_reg].data + offset, size);
      return size;
    }

  } 

  return -ENOENT;
}

static int write_callback(const char *path, const char *buffer, size_t size, off_t offset,
        struct fuse_file_info *fi)
{

  int in_reg;

  for (int i = 0 ; i < path_from_root_number ; i++) {
    
    if (path_from_root[i].type == 1) continue;

      if (strcmp(path, path_from_root[i].path) == 0) {

        in_reg = find_in_reg(path_from_root[i].inode);

        reg[in_reg].data = realloc(reg[in_reg].data, offset + size + 1);
        memcpy(reg[in_reg].data + offset, buffer, size);
        reg[in_reg].data[strlen(reg[in_reg].data)] = '\0';

      }
  } 

  return -ENOENT;
}

static int mkdir_callback(const char *path, mode_t mode)
{
  //split
  char *dir_from_root = malloc(4096 * sizeof(char));
  char *new_dir = malloc(255 * sizeof(char));
  int seq;
  int entry_size;
  int inode;

  inode = ++table_num;
  inode_type_table = realloc(inode_type_table, sizeof(_inode_type_table)*inode); //increase inode
  inode_type_table[inode - 1].inode = inode - 1;
  inode_type_table[inode - 1].type = strdup("dir");

  split_at_last_stroke(path, dir_from_root, new_dir);

  //add new inode
  dir = realloc(dir, sizeof(_dir) * ++dir_num);

  for (int i = 0 ; i < path_from_root_number ; i++) {
      if ( path_from_root[i].type == 0) continue;

      if (strcmp(dir_from_root, path_from_root[i].path) == 0) {
        seq = find_in_dir(path_from_root[i].inode);
        entry_size = ++dir[seq].number_of_entries;
        dir[seq].entries = realloc(dir[seq].entries, sizeof(_entries) * entry_size);
        dir[seq].entries[entry_size-1].inode = inode - 1;
        dir[seq].entries[entry_size-1].name = strdup(new_dir);
      }
    }

  dir[dir_num-1].entries = NULL;
  dir[dir_num-1].number_of_entries = 0;
  dir[dir_num-1].inode = inode - 1;

  return 0;
}

static int chmod_callback(const char* path, mode_t mode){

  int good;

  good = chmod(path, mode);

  if(good == - 1)
    return -ENOENT;

  return 0;

}

static int rename_callback(const char* src, const char* dest){

  int good;

  good = rename(src, dest);

  if(good == - 1)
    return -ENOENT;

  return 0;

}

static int create_callback(const char *path, mode_t mode, struct fuse_file_info *fi)
{
    return open_callback(path, fi);
}



static struct fuse_operations fuse_example_operations = {
  .getattr = getattr_callback,
  .open = open_callback,
  .read = read_callback,
  .readdir = readdir_callback,
  .create = create_callback,
  .mkdir = mkdir_callback,
  .write = write_callback,
  .chmod = chmod_callback,
  .rename = rename_callback,
};

int main(int argc, char *argv[])
{

  char **args = malloc(sizeof(char*)*2);
 
  json_object* json_obj = json_object_from_file(argv[1]);

  args[0] = strdup(argv[0]);
  args[1] = strdup(argv[2]);

  if (json_obj == NULL) {
    printf("Error parsing JSON!\n");
    return 1;
  }

  get_json(json_obj);

  json_object_put(json_obj); // free json object

  make_path_from_root();

  //to check
  saved_as_json(); 

  return fuse_main(argc-1, args, &fuse_example_operations, NULL);
}
