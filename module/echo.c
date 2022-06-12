/*
 * LMFS WorkStation - MODULE - TEXT PRINTER
 *
 * Builtin
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#include "command-register.h"

#include <stdio.h>
#include <stdlib.h>

int mod_init(){ return 0; }
int mod_helper(){
    printf("LMFS WorkStation 2022 - Work ToolBox\n");
    printf("    echo <text...>\n");
    printf("LMFSWorkStation Built-in command.\n");
}

double echo(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}

double NF(){ return -1; }

FuncList Regist[]={
      {"echo", 1, echo, NULL},
      {NULL, -1, NF, NULL}
};
