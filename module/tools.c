/*
 * LMFS WorkStation - MODULE - TEXT PRINTER
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

//resultpShell* rpS;

int mod_init(ModuleList* ML_m, FuncListArr* FLA_m, resultpShell* rpS_m){
    ModL=ML_m;
    FuncL=FLA_m;
    rpS=rpS_m;
    return 0;
}

int mod_helper(){
    printf("LMFS WorkStation - Work ToolBox - Core Module\n");
    printf("    echo <text...>\n");
    printf("    system <command...>\n");
    printf("    exit [code]\n");
    printf("    print_result --- Attention: Used only to return the contents of a ptrfunction (must be char*)\n");
    printf("    getresult --- Attention: Used only to return the contents of a ptrfunction\n");
    printf("    version\n");
    printf("    clear\n");
    printf("LMFSWorkStation Built-in command.\n");
    return 0;
}

double printresult(){
    printf("%s", (char*)(rpS->resultp));
    return 0;
}

void* getresult(){
    return rpS->resultp;
}

double runrps(){
    return doLMFSWSCmd(getresult());
}

double execlmfswscmd(int argc, char* argv[]){
    int status=0;
    for(int i=1;i<argc;i++){
        status=doLMFSWSCmd(argv[i]);
        if(status==-1) return -1;
    }
    return status;
}

double echo(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        printf("%s%c", argv[i], i+1==argc?'\0':' ');
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
    while((ch=*(s++))!=0){
        if(ch>='0'&&ch<='9'){
            result=result*10+ch-48;
        }
        else{
            fprintf(stderr, "\033[91;1mError\033[0m: Not an legal integer number!\n");
            return -1;
        }
    }
    return result;
}

double _lmfsws_exit(int argc, char* argv[]){
    int exitcode=-1;
    if(argc==1)
      exit(0);
    else if(argc==2){
        exitcode=sToi(argv[1]);
        if(exitcode!=-1) exit(exitcode);
        else return -1;
    }
    else{
        fprintf(stderr,"\033[91;1mError\033[0m: required an argument, but %d %s given.\n", argc, argc<2?"was":"were");
        return -1;
    }
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
      {"exit", 1, _lmfsws_exit, NULL},
      {"version", 1, version, NULL},
      {"clear", 1, _clear, NULL},
      {"print_result", 1, printresult, NULL},
      {"getresult", 2, NULL, (void*)getresult},
      {"exec_lastresult", 1, runrps, NULL},
      {"exec", 1, execlmfswscmd, NULL},
      {NULL, -1, NF, NULL}
};
