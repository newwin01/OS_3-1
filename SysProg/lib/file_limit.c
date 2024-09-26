#include <errno.h> 
#include <fcntl.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <stdlib.h>
#include <sys/resource.h>

void __attribute__((constructor)) before_main( void )
{
    struct rlimit limit;
    limit.rlim_cur = 3;
    limit.rlim_max = 3;
    if (setrlimit(RLIMIT_NOFILE, &limit) != 0) {
        fprintf(stderr, "%s\n", "setrlimit");
        exit(1);
    }
}