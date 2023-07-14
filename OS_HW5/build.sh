set -x
gcc FUSE.c $(pkg-config fuse json-c --cflags --libs) -o fuse_example