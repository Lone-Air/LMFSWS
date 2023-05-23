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

static double _v(int argc, char** argv){
    printf(
           "LMFSWorkStation - LMFS org. - "
#include "VERSION"
           "\n"
           "INSTALL_PREFIX=%s"
           "\n"
           "LIBRARY_PATH=%s"
           "\n"
           "BINARY_PATH=%s"
           "\n"
           "ETC_PATH=%s"
           "\n"
           "LMFSWS_MORULE_DIR=%s"
           "\n"
           "LMFSWS_MODULE_PATH=%s"
           "\n"
           "SHADOW_PATH=%s"
           "\n"
           "COMPILATION_FLAGS=%s"
           "\n",
           PREFIX, PREFIX LIBNAME, PREFIX BINNAME, ETCNAME, MODNAME, MODULE, SHADOWPATH, COMPILATION
           );
    return 0;
}

static double _run_command(int argc, char** argv){
    if(argc>=1)
      doLMFSWSCmd(argv[0]);
    return 0;
}

static double _get_version(int argc, char** argv){
    fprintf(stdout, "LMFSWorkStation - LMFS org. - "
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
    int status=0;
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
        status=runNormallyFile(_fn);
        if(status==-1){
            fprintf(stderr, "\033[91;1mError\033[0m: Error happened while running\n");
        }
        if(used_fn1){
            free(_fn1);
            used_fn1=false;
        }
    }
    return 0;
}

int main(int argc, char* argv[]){
    if(InitLMFSWS()==-1){
        fprintf(stderr, "LMFSWS failed to launch\n");
        return 1;
    }
    RegisterArg((Argument){'\0', "run", "Run file --run=file1,file2", 1, run});
    RegisterArg((Argument){'\0', "runfiles", "Run file --runfiles file1 file2", 2, run});
    RegisterArg((Argument){'\0', "version", "Get version of LMFSWorkStation", 3, _get_version});
    RegisterArg((Argument){'c', "command", "Execute a command in LMFSWorkStation without command line", 4, _run_command});
    RegisterArg((Argument){'v', NULL, "Output LMFSWS configuration information", 3, _v});
    if(argc==1){
        fprintf(stdout,
#include "HELLOTEXT"
                "\n"
                );
    }
    PtrFunction input=GetFunc("input").PtrFunc;
    char* data;
    if(argc==1){
        while(1){
            data=input("LMFSWorkStation> ");
            if(data==NULL) break;
            doLMFSWSCmd(data);
            free(data);
        }
    }
    else{
        callArgs(argc, argv);
    }
    CleanMemory();
    return 0;
}

