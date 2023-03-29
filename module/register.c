/*
 * LMFS WorkStation - Modules Register User Mode
 *
 * Builtin
 *
 * LICENSE SEE $ROOT/LICENSE
 *
 */

#include "command-register.h"
#include "module-loader.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mod_helper(){
    printf("LMFS WorkStation - Work ToolBox - Core Command\n");
    printf("    include <modules...>\n");
    printf("    regist <modules...>\n");
    printf("    import <modules...>\n");
    printf("    printloaded\n");
    printf("    execext <command> [args...]\n");
    printf("    exthelp <modules...>\n");
    printf("    list_functions\n");
    printf("LMFSWorkStation Built-in Command.\n");
    return 0;
}

int mod_close(){
    return 0;
}

double include(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "register")==0){
            fprintf(stderr, "\033[91;1mError\033[0m: You can't include 'register' itself!\n");
            return -1;
        }
        if(LoadModuleByName(argv[i])==-1) return -1;
    };
    return 0;
}

double _regist(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        if(regist(argv[i])==-1) return -1;
    }
    return 0;
}

double import(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "register")==0){
            fprintf(stderr, "\033[91;1mError\033[0m: You can't include 'register' itself!\n");
            return -1;
        }
        if(LoadModuleByName(argv[i])!=-1){
            regist(argv[i]);
        }else return -1;
    }
    return 0;
}

double printloaded(){
    printf("Available modules:\n");
    for(int i=0;i<ModuleNum;i++){
        printf("%s ", Modules[i].ModuleName);
    }
    printf("\n");
    return 0;
}

double listFuncs(){
    printf("Available functions(*: Pointer functions, !: Not user functions):\n");
    for(int i=0;i<FuncNum;i++){
        if(Func[i].type==2||Func[i].type==1)
          printf("%s\033[92m%s\033[0m ", Func[i].type==1?"":"*", Func[i].name);
        else
          printf("!%s\033[91m%s\033[0m ", Func[i].type==3?"":"*", Func[i].name);
    }
    printf("\n");
    return 0;
}

int mod_init(LMFSWS_State* L){
    UseState(L);
    return 0;
}

double NF(){ return -1; }

FuncList Regist[]={
      {"include", 1, include, NULL},
      {"regist", 1, _regist, NULL},
      {"printloaded", 1, printloaded, NULL},
      {"import", 1, import, NULL},
      {"list_functions", 1, listFuncs, NULL},
      {NULL, -1, NF, NULL}
};

