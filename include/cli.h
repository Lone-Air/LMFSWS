/*
 * LMFS WorkStation - Command line Interpreter - Arguments Parser - Header File
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#ifndef LMFSWS_CLI_H
#define LMFSWS_CLI_H

#include "module-loader.h"
#include "command-register.h"
#include <stdbool.h>

typedef struct Argument{
    char flagShort;
    char* flagFull;
    char* disc;
    short ArgType; // 1, 2, 3, 4, 5
    /*
     * 1: --xxx="a, b, c" -> xxx, {"a", "b", "c"}
     * 2: --xxx a b c d --yyy -> xxx, {"a", "b", "c", "d"}
     * 3: --xxx a b c --yyy -> xxx; a; b; c; yyy
     * 4: --xxx a b c -> xxx, a; b; c (b and c will be unrecognizable)
     * 5: Take all program parameters as arguments to this parameter
     */
    _Function callf; // double _f(int argc, char* argv);
}Argument;

typedef struct ArgumentsList{
    Argument arg;
    struct ArgumentsList* next;
    struct ArgumentsList* last;
}ArgumentsList;

typedef struct Ala{
    ArgumentsList* l;
}Ala;

extern Ala* ALAM;

typedef struct ArgWithAl{
    char* name;
    ArgList arg;
}ArgWithAl;

extern char** EMPTY_LIST;

typedef struct resultpShell{
    void* resultp;
}resultpShell;

typedef struct LMFSWS_State{
    Ala* als;
    ArgumentsList* ale;
    resultpShell *rps;
    FuncListArr* fla;
    ModuleList* ml;
}LMFSWS_State;

extern LMFSWS_State* L_s;
extern resultpShell* rpS;

#define result_p rpS->resultp
#define DefaultArgPool ALAM->l

extern ArgumentsList* DefaultArgPoolHead;

extern void InitArgPool();

extern void RegisterArg(Argument);
extern Argument GetArgByFullFlag(char*);
extern Argument GetArgbyShortFlag(char);
extern void callArgs(int argc, char** argv);
extern double callArgFunc(Argument, ArgList);

extern void UseState(LMFSWS_State*);

extern double doLMFSWSCmd(const char*);

extern int runNormallyFile(char*);

extern void CloseArgPool();

extern int InitLMFSWS();

extern void* get_result();

extern void CleanMemory();
extern void ForceQuit(int);

#endif

