/*
 * LMFS WorkStation - Module Loader - HEADER FILE
 *
 * LICENSE SEE $ROOT/LICENSE
 *
 */

#ifndef LMFSWS_ML_H
#define LMFSWS_ML_H

#include "config.h"

#include <dlfcn.h>

typedef struct ArgList{
    int argc;
    char** argv;
}ArgList;

typedef double (*_Function)();
typedef void* (*PtrFunction)();

typedef struct FuncList{
    char* name;
    int type;
    _Function Func;
    PtrFunction PtrFunc;
}FuncList;

struct Module{
    char* ModuleName;
    void* dlheader;
    _Function Init;
    FuncList* FL;
};

typedef struct Module Module;

extern Module* Modules;
extern long int ModuleNum;

extern int FindModule(const char*);

extern int InitModulePool();
extern int LoadModule(const char*);

extern int RemoveLoaded(const char*);

extern int CallModuleInit(const char*);
extern void* GetPtrFunction(const char*, const char*);

extern int CloseModules();

#endif
