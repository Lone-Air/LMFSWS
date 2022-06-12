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



#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mod_init(){
    if(InitModulePool()!=1) return -1;
    if(InitFuncPool()!=0){
        CloseModules();
        return 1;
    }
    return 0;
}

int mod_helper(){
    printf("LMFS WorkStation 2022 - Work ToolBox - Core Command\n");
    printf("    include <modules...>\n");
    printf("    regist <modules...>\n");
    printf("LMFSWorkStation Built-in Command.\n");
}

double execext(int argc, char* argv[]){
    ArgList new;
    int count=0;
    char** arg=(char**)malloc(((argc+1)*sizeof(char*)));
    for(int i=1;i<argc;i++){
        arg[count]=(char*)calloc(0, strlen(argv[i])+1);
        strcpy(arg[count++], argv[i]);
    }
    new.argc=argc-1;
    new.argv=arg;
    int type,result;
    type=getType(arg[0]);
    if(type!=-1){
        if(type==1){
            result=GetFunc(arg[0]).Func(new);
            if((int)result!=0){
                printf("Result=%lf\n", result);
            }
        }else if(type==2){
            GetFunc(arg[0]).PtrFunc(new);
        }
    }
    return 0;
}

double include(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        LoadModule(argv[i]);
    };
    return 0;
}

double _regist(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        regist(argv[i]);
    }
    return 0;
}

double printloaded(){
    for(int i=0;i<ModuleNum;i++){
        printf("%s\n", Modules[i].ModuleName);
    }
    return 0;
}

double NF(){ return -1; }

FuncList Regist[]={
      {"include", 1, include, NULL},
      {"regist", 1, _regist, NULL},
      {"printloaded", 1, printloaded, NULL},
      {"execext", 1, execext, NULL},
      {NULL, -1, NF, NULL}
};

