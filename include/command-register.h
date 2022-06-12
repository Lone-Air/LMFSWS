/*
 * LMFS WorkStation - Command Register - HEADER FILE
 *
 * LICENSE SEE $ROOT/LICENSE
 * 
 */

#ifndef LMFSWS_CMDREG_H
#define LMFSWS_CMDREG_H

#include "module-loader.h"

extern FuncList *Func;
extern long int FuncNum;

extern int FindFunc(const char*);

extern int InitFuncPool();

extern int regist(const char*);
extern int getType(const char*);
extern FuncList GetFunc(const char*);

extern int CloseFuncPool();

#endif

