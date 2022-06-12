/*
 * LMFS WorkStation - Command Register
 *
 * LICENSE SEE $ROOT/LICENSE
 *
 */

#include "command-register.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FuncList *Func;
long int FuncNum;

extern int FindFunc(const char* name){
    for(int i=0;i<FuncNum;i++){
        if(strcmp(name, Func[i].name)==0) return i;
    }
    return -1;
}

extern int InitFuncPool(){
    Func=(FuncList*)malloc(sizeof(FuncList)*1);
    if(Func==NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: Cannot allocate enough memories for Function List Pool!\n");
        return -1;
    }
    FuncNum=0;
    return 0;
}

extern int regist(const char* name){
    int Find=FindModule(name);
    if(Find==-1){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: No module named `%s'\n", name);
        return -1;
    }
    if(CallModuleInit(name)==-1) return -1;
    FuncList Item;
    int count=0;
    while(1){
        Item=Modules[Find].FL[count++];
        if(Item.name==NULL) break;
        Func=(FuncList*)realloc(Func, sizeof(FuncList)*(FuncNum+1));
        Func[FuncNum++]=Item;
    }
    return 0;
}

extern int getType(const char* name){
    int Find=FindFunc(name);
    if(Find==-1){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: No function named `%s'\n", name);
        return -1;
    }
    return Func[Find].type;
}

double NULLFUNC(){return -1;}

extern FuncList GetFunc(const char* name){
    int Find=FindFunc(name);
    if(Find==-1){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: No function named `%s'\n", name);
        _Function NF=NULLFUNC;
        return (FuncList){NULL, -1, NF, NULL};
    }
    return Func[Find];
}

extern int CloseFuncPool(){
    free(Func);
    FuncNum=0;
    return 0;
}

