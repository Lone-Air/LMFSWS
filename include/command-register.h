/*
 * LMFS WorkStation - Command Register - HEADER FILE
 *
 * LICENSE SEE $ROOT/LICENSE
 * 
 */

#ifndef LMFSWS_CMDREG_H
#define LMFSWS_CMDREG_H

#include "module-loader.h"

typedef struct FuncListArr{
    FuncList* _Func;
    unsigned long int _FuncNum;
}FuncListArr;

extern FuncListArr* FuncL;

#define Func FuncL -> _Func
#define FuncNum FuncL -> _FuncNum

extern int FindFunc(const char*);

extern int InitFuncPool();

extern int regist(const char*);
extern int RegisterManual(char* name, int type, _Function func, PtrFunction ptrfunc);
extern int getType(const char*);
extern FuncList GetFunc(const char*);

extern int CheckUserMode(const char*);

extern int CloseFuncPool();

#endif

