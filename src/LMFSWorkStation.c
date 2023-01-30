/*
 * LMFS WorkStation
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#include "module-loader.h"
#include "command-register.h"
#include "cmdline.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

static double _run_command(int argc, char** argv){
    if(argc>=1)
      doLMFSWSCmd(argv[0]);
    return 0;
}

static double _get_version(int argc, char** argv){
    fprintf(stdout, "LMFSWorkStation - LMFS Org. - "
#include "VERSION"
            "\n");
    exit(0);
    return 0;
}

static double run(int argc, char** argv){
    char* _fn;
    char* _fn1;
    bool used_fn1;
    used_fn1=false;
    char* _home;
    for(int i=0;i<argc;i++){
        _fn=(char*)calloc(strlen(argv[i])+1, sizeof(char));
        strcpy(_fn, argv[i]);
        if(_fn[0]=='~'){
            _fn++;
            _home=getenv("HOME");
            if(_home==NULL) _home="/home";
            _fn1=(char*)calloc(strlen(_fn)+strlen(_home)+1, sizeof(char));
            strcpy(_fn1, _home);
            strcat(_fn1, _fn);
            _fn--;
            free(_fn);
            _fn=_fn1;
            used_fn1=true;
        }
        runNormallyFile(_fn);
        if(used_fn1){
            free(_fn1);
            used_fn1=false;
        }
    }
    return 0;
}

int main(int argc, char* argv[]){
    InitArgPool();
    RegisterArg((Argument){'\0', "run", "Run file --run=file1,file2", 1, run});
    RegisterArg((Argument){'\0', "runfiles", "Run file --runfiles file1 file2", 2, run});
    RegisterArg((Argument){'v', "version", "Get version of LMFSWorkStation", 3, _get_version});
    RegisterArg((Argument){'c', "command", "Execute a command in  LMFSWorkStation without command line", 4, _run_command});
    if(argc==1){
        fprintf(stdout,
#include "HELLOTEXT"
                "\n"
                );
    }
    if(InitModulePool()!=1) return 1;
    if(InitFuncPool()!=0){
        CloseModules();
        return 1;
    }
    if(InitLMFSWS()==-1) return 1;
    PtrFunction input=GetFunc("input").PtrFunc;
    if(argc==1){
        while(1){
            char* data=input("LMFSWorkStation> ");
            if(data==NULL) break;
            doLMFSWSCmd(data);
            free(data);
        }
    }
    else{
        callArgs(argc, argv);
    }
    CloseArgPool();
    CloseModules();
    CloseFuncPool();
    return 0;
}

