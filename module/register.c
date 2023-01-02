/*
 * LMFS WorkStation - Modules Register User Mode
 *
 * Builtin
 *
 * LICENSE SEE $ROOT/LICENSE
 *
 */

#include "command-register.h"
#include "module-loader.h"



#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int mod_helper(){
    printf("LMFS WorkStation - Work ToolBox - Core Command\n");
    printf("    include <modules...>\n");
    printf("    regist <modules...>\n");
    printf("    import <modules...>\n");
    printf("    printloaded\n");
    printf("    execext <command> [args...]\n");
    printf("    exthelp <modules...>\n");
    printf("    list_functions\n");
    printf("LMFSWorkStation Built-in Command.\n");
}

int mod_close(){
    CloseModulesPart();
    CloseFuncPool();
    return 0;
}

double exthelp(int argc, char* argv[]){
    if(argc<2){
        printf("LMFS WorkStation - Work ToolBox - Core Command\n");
        printf("    exthelp <modules...>\n");
        printf("LMFSWorkStation Built-in Command.\n");
    }else{
        for(int i=1;i<argc;i++){
            if(strcmp(argv[i], "help")==0){
                printf("LMFS WorkStation - Work ToolBox - Core Command\n");
                printf("    exthelp <modules...>\n");
                printf("LMFSWorkStation Builtin.\n");
                continue;
            }
            int index=FindModule(argv[i]);
            if(index==-1){
                fprintf(stderr, "\033[91;1mFatal Error\033[0m: No module named `%s'. Please use 'printloaded' and 'builtin_printloaded' to see what loaded\n", argv[i]);
                continue;
            }
            _Function helper=dlsym(Modules[index].dlheader, "mod_helper");
            char* err=dlerror();
            if(err!=NULL){
                fprintf(stderr, "\033[91;1mFatal Error\033[0m: Cannot load symbol `mod_helper', Reason:\n%s\nPlease check module status\n", err);
                continue;
            }
            helper();
        }
    }
    return 0;
}

double execext(int argc, char* argv[]){
    ArgList new;
    int count=0;
    char** arg=(char**)calloc(argc+1,sizeof(char*));
    for(int i=0;i<argc;i++){
        arg[count]=(char*)calloc(strlen(argv[i])+1, sizeof(char));
        strcpy(arg[count++], argv[i]);
    }
    new.argc=argc-1;
    new.argv=arg;
    int type;
    double result;
    type=getType(arg[0]);
    if(type!=-1){
        int isusermode=CheckUserMode(arg[0]);
        if(isusermode==1){
            fprintf(stderr, "\033[91;1mOperation denied\033[0m: This function may just for <Program internal running mode>\n");
            return -1;
        }
        if(isusermode==-1){
            fprintf(stderr, "\033[91;1mMode Checker Failed\033[0m: This function was registered with unknow execution type\n");
            return -1;
        }
        if(type==1){
            result=GetFunc(arg[0]).Func(new);
            if((int)result!=0){
                printf("Result=%lf\n", result);
            }
        }else if(type==2){
            GetFunc(arg[0]).PtrFunc(new);
        }
    }
    return 0;
}

double include(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "register")==0){
            fprintf(stderr, "\033[91;1mError\033[0m: You can't include 'register' itself!\n");
            continue;
        }
        LoadModule(argv[i]);
    };
    return 0;
}

double _regist(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        regist(argv[i]);
    }
    return 0;
}

double import(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        if(strcmp(argv[i], "register")==0){
            fprintf(stderr, "\033[91;1mError\033[0m: You can't include 'register' itself!\n");
            continue;
        }
        if(LoadModule(argv[i])!=-1){
            regist(argv[i]);
        }
    }
    return 0;
}

double printloaded(){
    printf("Available modules:\n");
    for(int i=0;i<ModuleNum;i++){
        printf("%s ", Modules[i].ModuleName);
    }
    printf("\n");
    return 0;
}

double listFuncs(){
    printf("Available functions(*: Pointer functions, !: Not user functions):\n");
    for(int i=0;i<FuncNum;i++){
        if(Func[i].type==2||Func[i].type==1)
          printf("%s\033[92m%s\033[0m ", Func[i].type==1?"":"*", Func[i].name);
        else
          printf("!%s\033[91m%s\033[0m ", Func[i].type==3?"":"*", Func[i].name);
    }
    printf("\n");
    return 0;
}

double _sync(FuncList* _Func, int _FuncNum){
    CloseFuncPool();
    InitFuncPool();
    for(int i=0; i<_FuncNum; i++){
        RegisterManual(_Func[i].name, _Func[i].type, _Func[i].Func, _Func[i].PtrFunc);
    }
    FuncNum=_FuncNum;
    return 0;
}

double _sync_mod(Module* _mds, int _ModN){
    CloseModulesPart();
    InitModulePool();
    for(int i=0; i<_ModN; i++){
        AddModuleInfo(_mds[i]);
    }
    return 0;
}

FuncListArr* _sync_main(){
    FuncListArr* result=(FuncListArr*)calloc(1, sizeof(FuncListArr));
    result->_Func=Func;
    result->_FuncNum=FuncNum;
    return result;
}

ModuleList* _sync_mod_main(){
    ModuleList* result=(ModuleList*)calloc(1, sizeof(ModuleList));
    result->_m=Modules;
    result->_mn=ModuleNum;
    return result;
}

int mod_init(){
    if(InitModulePool()!=1) return -1;
    if(InitFuncPool()!=0){
        CloseModules();
        return 1;
    }
    return 0;
}

double NF(){ return -1; }

FuncList Regist[]={
      {"include", 1, include, NULL},
      {"regist", 1, _regist, NULL},
      {"printloaded", 1, printloaded, NULL},
      {"execext", 1, execext, NULL},
      {"exthelp", 1, exthelp, NULL},
      {"import", 1, import, NULL},
      {"list_functions", 1, listFuncs, NULL},
      {"sync", 4, _sync, NULL},
      {"sync_main", 4, NF, (void*)_sync_main},
      {"sync_mod", 4, _sync_mod, NULL},
      {"sync_mod_main", 4, NF, (void*)_sync_mod_main},
      {NULL, -1, NF, NULL}
};

