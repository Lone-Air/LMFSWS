/*
 * LMFS WorkStation - MODULE - EXIT
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
    printf("LMFS WorkStation - Work ToolBox\n");
    printf("    exit [code]\n");
    printf("LMFSWorkStation Built-in command.\n");
}

int sToi(char* s){
    int ch;
    int result=0;
    while(ch=*s++!=0){
        if(ch>='0'&&ch<='9'){
            result=result*10+ch-48;
        }
    }
    return result;
}

double _exit(int argc, char* argv[]){
    if(argc==1)
      exit(0);
    else if(argc==2)
      exit(sToi(argv[1]));
    else
      fprintf(stderr,"\033[91;1mError\033[0m: required an argument, but %d was given.\n", argc);
    return 0;
}

double NF(){ return -1; }

FuncList Regist[]={
      {"exit", 1, _exit, NULL},
      {NULL, -1, NF, NULL}
};
