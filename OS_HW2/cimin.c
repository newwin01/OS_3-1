#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int fork_testing(int pid){

    

    return 0;
}


int
main(int argc, char *argv[])
{

    //Get option using get opt
    int opt;
    while((opt = getopt(argc, argv, "i:m:o:")) != -1){
        switch (opt)
        {
        // -i option is followed by a file path of the crashing input
        case 'i':

            break;

        //-m option is followed by a string whose appearance in standard error determines whether the expected crash occurs or not.
        case 'm': 

            break;

        //-o option is followed by a new file path to store the reduced crashing input.
        case 'o':

            break;

        default:
            fprintf(stderr,"Error getting on the path");
            exit(1);
        }


    }




    return 0;
}