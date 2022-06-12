/*
 * LMFS WorkStation - Version Information
 *
 * Builtin
 *
 * LICENSE SEE $ROOT/LICENSE
 *
 */

#include "command-register.h"
#include "module-loader.h"

#include <stdio.h>
#include <stdlib.h>

int mod_init(){ return 0; }
int mod_helper(){
    printf("LMFS WorkStation 2022 - Work ToolBox - Core\n");
    printf("    version\n");
    printf("LMFSWorkStation Builtin.\n");
    return 0;
}

double version(){
    printf(
#include "VERSION"
           "\n"
          );
    return 0;
}

double NF(){ return -1; }

FuncList Regist[]={
      {"version", 1, version, NULL},
      {NULL, -1, NF, NULL}
};

