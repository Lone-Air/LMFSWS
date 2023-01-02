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
    printf("LMFS WorkStation - Work ToolBox - Core Module\n");
    printf("    echo <text...>\n");
    printf("    system <command...>\n");
    printf("    exit [code]\n");
    printf("    version\n");
    printf("    clear\n");
    printf("LMFSWorkStation Built-in command.\n");
}

double echo(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        printf("%s ", argv[i]);
    }
    printf("\n");
    return 0;
}

double _system(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        system(argv[i]);
    }
    return 0;
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

double _clear(){
    printf("\033[H\033[J\033[3J");
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
      {"echo", 1, echo, NULL},
      {"system", 1, _system, NULL},
      {"exit", 1, _exit, NULL},
      {"version", 1, version, NULL},
      {"clear", 1, _clear, NULL},
      {NULL, -1, NF, NULL}
};
