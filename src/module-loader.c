/*
 * LMFS WorkStation - Module Loader
 *
 * LICENSE SEE $ROOT/LICENSE
 *
 */

#include "module-loader.h"
#include "command-register.h"
#include "cli.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <dlfcn.h>

#ifdef __WIN32__
  #include <io.h>
  #define getcwd _getcwd
#else
  #include <unistd.h>
#endif

ModuleList* ModL;

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
    ModL=(ModuleList*)calloc(1, sizeof(ModuleList));
    ModuleNum=0;
    Modules=(Module*)calloc(ModuleNum+2, sizeof(Module));
    if(Modules==NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: Failed to allocate enough memories for the modules pool!\n");
        return -1;
    }
    return 1;
}

static bool check_config(char* text, const char* target){
    short count=0;
    short match=0;
    while(text[count]!='\0'){
        if(text[count]!=target[match] && text[count]!='\n')
          break;
        if(text[count]==target[match])
          match++;
        if(match>=strlen(target)-1) return true;
        count++;
    }
    return false;
}

extern int LoadModuleByName(const char* mod_name){
    if(FindModule(mod_name)!=-1){
        fprintf(stderr, "\033[95;1mWarning\033[0m: The module which named `%s' had been loaded\n", mod_name);
        return 0;
    };
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

    char* cwd=calloc(2048, sizeof(char));
    cwd=getcwd(cwd, 2048);

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

    char* cache={0};

    FILE* cfg=fopen(SYSPATH, "r");
    if(cfg==NULL){
        cfg=fopen(UNDERNAME, "r");
        if(cfg==NULL){
            fprintf(stderr, "\033[91;1mFatal Error\033[0m: No module named `%s'\n", mod_name);
            err=1;
            goto finish;
        }
        cfg=fopen(UNDERMOD, "r");
        const char* _p=UNDERNAME;
        if(cfg!=NULL){
            cache=load(cfg);
            if(check_config(cache, "LAZY"))
              dlh=dlopen(_p, RTLD_LAZY);
            else if(check_config(cache, "NOW"))
              dlh=dlopen(_p, RTLD_NOW);
            else if(check_config(cache, "GLOBAL"))
              dlh=dlopen(_p, RTLD_GLOBAL);
            else if(check_config(cache, "LOCAL"))
              dlh=dlopen(_p, RTLD_LOCAL);
            else{
                fprintf(stderr, "\033[91;1mFatal Error\033[0m: Invalid mode for module %s\n", mod_name);
                err=1;
                goto finish;
            }
        }else{
            dlh=dlopen(_p, RTLD_NOW);
        }
    }else{
        cfg=fopen(SYSMOD, "r");
        const char* _p=SYSPATH;
        if(cfg!=NULL){
            cache=load(cfg);
            if(check_config(cache, "LAZY"))
              dlh=dlopen(_p, RTLD_LAZY);
            else if(check_config(cache, "NOW"))
              dlh=dlopen(_p, RTLD_NOW);
            else if(check_config(cache, "GLOBAL"))
              dlh=dlopen(_p, RTLD_GLOBAL);
            else if(check_config(cache, "LOCAL"))
              dlh=dlopen(_p, RTLD_LOCAL);
            else{
                fprintf(stderr, "\033[91;1mFatal Error\033[0m: Invalid mode for module %s\n", mod_name);
                err=1;
                goto finish;
            }
        }
        else{
            dlh=dlopen(_p, RTLD_NOW);
        }
    }

    if(cfg!=NULL) free(cache);

    if(!dlh){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: %s\n", dlerror());
        err=1;
        goto finish;
    }

    Init=dlsym(dlh, "mod_init");
    ER=dlerror();
    if(ER!=NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: CANNOT LOAD initalize function, Reason:\n%s\n", ER);
        goto finish;
    }

    list=dlsym(dlh, "Regist");
    ER=dlerror();
    if(ER!=NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: CANNOT LOAD regist list, Reason:\n%s\n", ER);
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
    free(UNDERMOD);
    free(UNDERNAME);
    free(cwd);

    if(err) return -1;

    return 0;
}

extern void AddModuleInfo(Module _mod){
    Modules=(Module*)realloc(Modules, (ModuleNum+1)*sizeof(Module));
    Modules[ModuleNum]=(Module){(char*)malloc(strlen(_mod.ModuleName)+1), _mod.dlheader, _mod.Init, _mod.Close, _mod.FL};
    strcpy(Modules[ModuleNum++].ModuleName, _mod.ModuleName);
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
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: Cannot allocate enough ram for RemoveLoaded()\n");
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
    return Modules[Find].Init(L_s);
}

extern void* GetPtrFunction(const char* name, const char* func){
    PtrFunction _Func=dlsym(Modules[FindModule(name)].dlheader, func);
    char* err=dlerror();
    if(err!=NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: Failed to load symbol `%s' from Module `%s', Reason:\n%s", func, name, err);
        return NULL;
    }
    return _Func;
}

extern int CloseModules(){
    for(int i=0;i<ModuleNum;i++){
        Modules[i].Close();
        dlclose(Modules[i].dlheader);
        free(Modules[i].ModuleName);
    }
    ModuleNum=0;
    free(Modules);
    free(ModL);
    return 0;
}

