/*
 * LMFS WorkStation - MODULE - SYSTEM COMMAND EXECUTE
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
    printf("    system <command...>\n");
    printf("LMFSWorkStation Built-in command.\n");
}

double _system(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        system(argv[i]);
    }
    return 0;
}

double NF(){ return -1; }

FuncList Regist[]={
      {"system", 1, _system, NULL},
      {NULL, -1, NF, NULL}
};
