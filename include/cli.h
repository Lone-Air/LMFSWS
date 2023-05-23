/*
 * LMFS WorkStation - Command line Interpreter - Arguments Parser - Header File
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#ifndef LMFSWS_CLI_H
#define LMFSWS_CLI_H

#include "module-loader.h"
#include "command-register.h"
#include "lmfstypes.h"
#include <stdbool.h>

extern LMFSWS_State* L_s;
extern resultpShell* rpS;

#define result_p rpS->resultp
#define DefaultArgPool ALAM->l

extern ArgumentsList* DefaultArgPoolHead;

extern int InitArgPool();

extern void RegisterArg(Argument);
extern Argument GetArgByFullFlag(char*);
extern Argument GetArgbyShortFlag(char);
extern void callArgs(int argc, char** argv);
extern double callArgFunc(Argument, ArgList);

extern void UseState(LMFSWS_State*);

extern double doLMFSWSCmd(const char*);

/*
 char* s=(char*)malloc(2, sizeof(char));
 strcpy(s, "s");
 setresult(s); // Don't free `s'
 */
extern void setresult(char*);

/*
 setresult_automalloc("abcdefg");
 */
extern void setresult_automalloc(const char*);

extern int runNormallyFile(char*);

extern void CloseArgPool();

extern int InitLMFSWS();

extern void* get_result();

extern void CleanMemory();
extern void ForceQuit(int);

#endif

