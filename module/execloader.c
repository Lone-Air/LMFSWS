
/*
 * LMFS WorkStation - MODULE - EXECUTABLE LOADER
 *
 * Builtin
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#include "command-register.h"
#include "module-loader.h"
#include "cli.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void **dlheaders;
int dlh_len;

int mod_init(LMFSWS_State* L){
    UseState(L);
    dlheaders=(void**)malloc(1*sizeof(void*));
    dlh_len=0;
    return 0;
}

int mod_close(){
    for(int i=0;i<dlh_len;i++)
      dlclose(dlheaders[i]);
    free(dlheaders);
    return 0;
}

int mod_helper(){
    printf("LMFS WorkStation - Work ToolBox - Core Module\n");
    printf(
           "    load_exec <executable files...>\n"
          );
    printf("LMFSWorkStation Built-in command.\n");
    return 0;
}

static int add_dlheader(void* dlheader){
    dlheaders=(void**)realloc(dlheaders, sizeof(void*)*(dlh_len+1));
    dlheaders[dlh_len++]=dlheader;
    return 0;
}

double exec_loader(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        bool opensuccess=false;
        char* p;
        for(int j=0;j<pathL;j++){
            p=(char*)calloc(strlen(pathA[j])+strlen(argv[i])+2, sizeof(char));
            strcpy(p, pathA[0]);
            strcat(p, "/");
            strcat(p, argv[i]);
            FILE* _f=fopen(p, "r");
            if(_f!=NULL){
                free(p);
                fclose(_f);
                opensuccess=true;
                break;
            }
            free(p);
        }
        if(!opensuccess)
          fprintf(stderr, "\033[91;1mError\033[0m: Cannot find %s in LMFSWS module loader paths, you may use addpath_automatic to add paths in $PATH to module loader paths automatically\n", argv[i]);
        else{
            void* dlh=dlopen(p, RTLD_NOW);
            if(!dlh){
                fprintf(stderr, "\033[91;1mFailed to load\033[0m: error in %s was: \n%s\n", argv[i], dlerror());
            }
            _Function _func=dlsym(dlh, "main");
            RegisterManual(argv[i], 1, _func, NULL);
            add_dlheader(dlh);
        }
    }
    return 0;
}

FuncList Regist[]={
      {"loadexec", 1, exec_loader, NULL},
      {NULL, -1, NULL, NULL}
};
