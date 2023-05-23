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
#include "lmfstypes.h"

extern path_shell* _main_p;

#define pathA _main_p->p.argv
#define pathL _main_p->p.argc



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

#endif
