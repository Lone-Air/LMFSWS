/*
 * LMFS WorkStation - Module Loader - HEADER FILE
 *
 * LICENSE SEE $ROOT/LICENSE
 *
 */

#ifndef LMFSWS_ML_H
#define LMFSWS_ML_H

#include "config.h"

#define _GNU_SOURCE

#include <dlfcn.h>

#ifndef bool
typedef enum bool{
    false=0, true=1
}bool;
#endif

typedef struct ArgList{
    int argc;
    char** argv;
}ArgList;

typedef struct path_shell{
    ArgList p;
}path_shell;

extern path_shell* _main_p;

#define pathA _main_p->p.argv
#define pathL _main_p->p.argc

typedef double (*_Function)();
typedef void* (*PtrFunction)();

typedef struct FuncList{
    char* name;
    int type;
    _Function _Func;
    PtrFunction PtrFunc;
}FuncList;

typedef struct Module{
    char* ModuleName;
    void* dlheader;
    _Function Init;
    _Function Close;
    FuncList* FL;
}Module;

typedef struct ModuleList{
    Module* _m;
    unsigned int _mn;
}ModuleList;

extern ModuleList* ModL;

#define Modules ModL->_m
#define ModuleNum ModL->_mn

extern int initia_path();
extern int add_path(const char*);

extern int FindModule(const char*);

extern int InitModulePool();
extern int LoadModuleByName(const char*);

extern int RemoveLoaded(const char*);

extern void AddModuleInfo(Module);

extern int CallModuleInit(const char*);
extern void* GetPtrFunction(const char*, const char*);

extern int CloseModules();
extern int ClearPath();
//extern int CloseModulesPart();


#endif
