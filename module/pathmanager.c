/*
 * LMFS WorkStation - MODULE - MODULE LOADER PATH MANAGER
 *
 * Builtin
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#include "command-register.h"
#include "module-loader.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>

int mod_init(LMFSWS_State* L){
    UseState(L);
    return 0;
}

int mod_helper(){
    printf("LMFS WorkStation - Work ToolBox - Core Module\n");
    printf(
           "    add_path <paths...>\n"
           "    list_paths\n"
          )
    printf("LMFSWorkStation Built-in command.\n");
    return 0;
}

double add_p(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        add_path(argv[i];)
    }
    return 0;
}

double list_p(int argc, char* argv[]){
    for(int i=0;i<pathL;i++){
        printf("%d|\t%s\n", i, argv[i]);
    }
    return 0;
}

FuncList Regist[]={
      {"add_path", 1, add_p, NULL},
      {"list_paths", 1, list_p, NULL},
      {NULL, -1, NULL, NULL}
};
