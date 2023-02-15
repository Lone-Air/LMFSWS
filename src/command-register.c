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

FuncListArr* FuncL;

#undef Modules
#undef ModuleNum
#define Modules ModL->_m
#define ModuleNum ModL->_mn

extern int FindFunc(const char* name){
    for(int i=0;i<FuncNum;i++){
        if(strcmp(name, Func[i].name)==0) return i;
    }
    return -1;
}

extern int InitFuncPool(){
    FuncL=(FuncListArr*)calloc(1, sizeof(FuncListArr));
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
        if(FindFunc(Item.name)!=-1){
            int toend=0;
            char* newname;
            while(1){
                newname=(char*)malloc(8+(strlen(Item.name))+2);
                memset(newname, 0, 8+strlen(Item.name)+2);
                sprintf(newname, "%s%d", Item.name, toend++);
                if(FindFunc(newname)==-1){
                    printf("\033[95;1mWarning\033[0m: `%s' from module `%s' has the same name as another function so register renamed it to `%s'\n", Item.name, name, newname);
                    Item.name=(char*)calloc(strlen(newname)+2, sizeof(char*));
                    strcpy(Item.name, newname);
                    free(newname);
                    break;
                }
                free(newname);
            }
        }
        Func[FuncNum++]=Item;
    }
    return 0;
}
extern int RegisterManual(char* name, int type, _Function func, PtrFunction ptrfunc){
    FuncList Item={name, type, func, ptrfunc};
    if(Item.name==NULL) return -1;
    Func=(FuncList*)realloc(Func, sizeof(FuncList)*(FuncNum+1));
    if(FindFunc(Item.name)!=-1){
        int toend=0;
        char* newname;
        while(1){
            newname=(char*)calloc(8+(strlen(Item.name))+2, sizeof(char));
            sprintf(newname, "%s%d", Item.name, toend++);
            if(FindFunc(newname)==-1){
                printf("\033[95;1mWarning\033[0m: `%s' from module `%s' has the same name as another function so register renamed it to `%s'\n", Item.name, name, newname);
                Item.name=(char*)calloc(strlen(newname)+2, sizeof(char*));
                strcpy(Item.name, newname);
                free(newname);
                break;
            }
            free(newname);
        }
    }
    Func[FuncNum++]=Item;
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

extern int CheckUserMode(const char* name){
    int Find=FindFunc(name);
    if(Find==-1){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: No function named `%s'\n", name);
        return -1;
    }
    int type=-1;
    switch(Func[Find].type){
      case 1: case 2:
        type=0;
        break;
      case 3: case 4:
        type=1;
    }
    return type;
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
    free(FuncL);
    return 0;
}

