#include "structs.h"

void free_cmd_input ( _cmd_input *cmd_input ) {
    
    free (cmd_input->error_file_name_optionI);
    free (cmd_input->error_founding_optionM);
    free (cmd_input->output_file_optionO);
    free (cmd_input);

}

_cmd_input *cmd_option_parser (char *argv[]){

    int cmd_options;
    _cmd_input *cmd_input = malloc( sizeof( _cmd_input ) );

    while ( (cmd_options = getopt( 7, argv, "i:m:o:" ) ) != -1 ) {
        switch (cmd_options) {
            case 'i' : //error file name=
                cmd_input->error_file_name_optionI = strdup (optarg);
                break;
            case 'm' : // founding error
                cmd_input->error_founding_optionM = strdup (optarg);
                break;
            case 'o' : // new file path to store minimum error
                cmd_input->output_file_optionO = strdup (optarg);
                break;
            default:
                fprintf( stderr, "%s\n", "Error on getting options" );
                exit(1);
        }
    }

    return cmd_input;
}
