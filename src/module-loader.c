/*
 * LMFS WorkStation - Module Loader
 *
 * LICENSE SEE $ROOT/LICENSE
 *
 */

#include "module-loader.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <unistd.h>

Module* Modules;
long int ModuleNum;

double NF_C(){ return 0; }

static char* load(FILE* F){
    fseek(F,0L,SEEK_END);
    uint64_t size=ftell(F);
    char* fs=(char*)malloc((size+1)*(sizeof(char)));
    rewind(F);
    size=fread(fs,1,size,F);
    fs[size]='\0';
    fclose(F);
    return fs;
}

extern int FindModule(const char* name){
    for(int i=0;i<ModuleNum;i++){
        if(strcmp(Modules[i].ModuleName, name)==0){
            return i;
        }
    }
    return -1;
}

extern int InitModulePool(){
    ModuleNum=0;
    Modules=(Module*)malloc((ModuleNum+1)*sizeof(Module));
    if(Modules==NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: Failed to allocate enough memories for the modules pool!\n");
        return -1;
    }
    return 1;
}

extern int LoadModule(const char* mod_name){
    if(FindModule(mod_name)!=-1) return 0;
    void* dlh;
    FuncList* list;
    _Function Init;
    _Function closeit;

    char* ER;
    int err=0;

    char* name=(char*)malloc(strlen(mod_name)+1+strlen(".mode"));
    strcpy(name, mod_name);
    strcat(name, ".mode");

    char* name_so=(char*)malloc(strlen(mod_name)+1+strlen(".so"));
    strcpy(name_so, mod_name);
    strcat(name_so, ".so");

    char* SYSPATH=(char*)malloc(strlen(name_so)+strlen(MODULE)+1);
    strcpy(SYSPATH, MODULE);
    strcat(SYSPATH, name_so);

    char* SYSMOD=(char*)malloc(strlen(name)+strlen(MODULE)+1);
    strcpy(SYSMOD, MODULE);
    strcat(SYSMOD, name);

    char cwd[2048];
    getcwd(cwd, 2048);

    char* UNDERMOD=(char*)malloc(strlen(cwd)+1+strlen("/")+strlen(mod_name)+strlen(".mode"));
    strcpy(UNDERMOD, cwd);
    strcat(UNDERMOD, "/");
    strcat(UNDERMOD, mod_name);
    strcat(UNDERMOD, ".mode");

    char* UNDERNAME=(char*)malloc(strlen(cwd)+1+strlen("/")+strlen(mod_name)+strlen(".so"));
    strcpy(UNDERNAME, cwd);
    strcat(UNDERNAME, "/");
    strcat(UNDERNAME, mod_name);
    strcat(UNDERNAME, ".so");

    char* cache;

    FILE* cfg=fopen(SYSPATH, "r");
    if(cfg==NULL){
        cfg=fopen(UNDERMOD, "r");
        if(cfg==NULL){
            fprintf(stderr, "\033[91;1mFatal Error\033[0m: No module named `%s'\n", mod_name);
            err=1;
            goto finish;
        }
        cache=load(cfg);
        dlh=dlopen(UNDERNAME, strcmp(cache, "LAZY")==0?RTLD_LAZY:RTLD_NOW);
    }else{
        cache=load(cfg);
        dlh=dlopen(SYSPATH, strcmp(cache, "LAZY")==0?RTLD_LAZY:RTLD_NOW);
    }

    if(!dlh){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: %s\n", dlerror());
        err=1;
        goto finish;
    }

    Init=dlsym(dlh, "mod_init");
    ER=dlerror();
    if(ER!=NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: CANNOT LOAD initalize function, Reason:\n%s\n", ER);
        int err=1;
        goto finish;
    }

    list=dlsym(dlh, "Regist");
    ER=dlerror();
    if(ER!=NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: CANNOT LOAD regist list, Reason:\n%s\n", ER);
        int err=1;
        goto finish;
    }

    closeit=dlsym(dlh, "mod_close");
    if(dlerror()!=NULL){
        closeit=NF_C;
    }

    Modules=(Module*)realloc(Modules, (ModuleNum+1)*sizeof(Module));
    Modules[ModuleNum]=(Module){(char*)malloc(strlen(mod_name)+1), dlh, Init, closeit, list};
    strcpy(Modules[ModuleNum++].ModuleName, mod_name);

finish:
    free(name);
    free(name_so);
    free(SYSPATH);
    free(SYSMOD);
    if(!err) free(cache);
    free(UNDERMOD);
    free(UNDERNAME);

    if(err) return -1;

    return 0;
}

extern int RemoveLoaded(const char* name){
    int Find=FindModule(name);
    if(Find==-1){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: No module named `%s'\n", name);
        return -1;
    }
    dlclose(Modules[Find].dlheader);
    Modules[Find].Close();
    free(Modules[Find].ModuleName);
    Module* new=(Module*)malloc(sizeof(Module)*(ModuleNum-1));
    if(new==NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: Cannot allocate enough memories for RemoveLoaded()\n");
        return -1;
    }
    int counter=0;
    for(int i=0;i<ModuleNum;i++){
        if(i==Find){
            continue;
        }
        new[counter++]=Modules[i];
    }
    free(Modules);
    Modules=new;
    ModuleNum=counter;
    return 0;
}

extern int CallModuleInit(const char* name){
    int Find=FindModule(name);
    if(Find==-1){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: No module named `%s'\n", name);
        return -1;
    }
    return Modules[Find].Init();
}

extern void* GetPtrFunction(const char* name, const char* func){
    PtrFunction Func=dlsym(Modules[FindModule(name)].dlheader, func);
    char* err=dlerror();
    if(err!=NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: Failed to load symbol `%s' from Module `%s', Reason:\n%s", func, name, err);
        return NULL;
    }
    return Func;
}

extern int CloseModules(){
    for(int i=0;i<ModuleNum;i++){
        Modules[i].Close();
        dlclose(Modules[i].dlheader);
        free(Modules[i].ModuleName);
    }
    ModuleNum=0;
    free(Modules);
    return 0;
}

