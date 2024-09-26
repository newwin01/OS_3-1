#include <stdio.h>

int main(int argc, char *argv[])
{

    char line[10];

    scanf("%s", line);
    
    printf("Read from file: %s", line);


    FILE *file = fopen("dummy.c", "r");
    
    if (file == NULL) printf(" file open error\n");
    
    return 0;
}