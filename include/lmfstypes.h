/*
 * LMFS WorkStation - Types
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#ifndef _LMFSWS_TYPES
#define _LMFSWS_TYPES
#ifndef bool
typedef enum bool{
    false=0, true=1
}bool;
#endif

typedef struct ArgList{
    int argc;
    char** argv;
}ArgList;

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

typedef struct path_shell{
    ArgList p;
}path_shell;

typedef struct FuncListArr{
    FuncList* _Func;
    unsigned long int _FuncNum;
}FuncListArr;

typedef struct Argument{
    char flagShort;
    char* flagFull;
    char* desc;
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

typedef enum var_type{
    VAR_STR, VAR_LIST
}var_type;

typedef union var_data{
    char* strData;
    ArgList listData;
}var_data;

typedef struct var_struct{
    char* name;
    var_type type;
    var_data data;
}var_struct;

typedef struct varpool{
    var_struct* var_pool;
    size_t varpool_len;
}varpool;

typedef struct LMFSWS_State{
    Ala* als;
    ArgumentsList* ale;
    resultpShell *rps;
    FuncListArr* fla;
    ModuleList* ml;
    path_shell* _mp;
    varpool* varp;
}LMFSWS_State;

#endif
