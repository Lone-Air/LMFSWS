/*
 * LMFS WorkStation
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#include "module-loader.h"
#include "command-register.h"
#include "cmdline.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

int InitLMFSWS(){
    int status;
    status=LoadModule("Input");
    if(status==-1){
        fprintf(stderr, "LMFS WorkStation has crashed\n");
        return -1;
    }
    if(regist("Input")==-1) return -1;
    status=LoadModule("cmdline");
    if(status==-1){
        fprintf(stderr, "LMFS WorkStation has crashed\n");
        return -1;
    }
    if(regist("cmdline")==-1) return -1;
    status=LoadModule("version");
    if(status==-1){
        fprintf(stderr, "LMFS WorkStation has crashed\n");
        return -1;
    }
    if(regist("version")==-1) return -1;
    status=LoadModule("register");
    if(status==-1){
        fprintf(stderr, "LMFS WorkStation has crashed\n");
        return -1;
    }
    if(regist("register")==-1) return -1;
    return 0;
}

int main(){
    Array* Args;
    ArgList Arg;
    int type;
    if(InitModulePool()!=1) return 1;
    if(InitFuncPool()!=0){
        CloseModules();
        return 1;
    }
    if(InitLMFSWS()==-1) return 1;
    PtrFunction input=GetFunc("input").PtrFunc;
    PtrFunction parse=GetFunc("parse").PtrFunc;
    while(1){
        const char* data=input("LMFSWorkStation> ");
        if(data==NULL) break;
        Args=parse(data);
        if(Args[0].length<1||strcmp(Args[0].data[0], "")==0) continue;
        Arg.argc=Args[0].length-1;
        Arg.argv=Args[0].data;
        if(strcmp(Arg.argv[0], "help")==0){
            if(Arg.argc>1){
                for(int i=1;i<Arg.argc;i++){
                    if(strcmp(Arg.argv[i], "help")==0){
                        printf("LMFS WorkStation 2022 - Work ToolBox - BUILTIN\n");
                        printf("    help <modules...>\n");
                        printf("LMFSWorkStation Builtin.\n");
                        continue;
                    }
                    int index=FindModule(Arg.argv[i]);
                    if(i==-1) continue;
                    _Function helper=dlsym(Modules[index].dlheader, "mod_helper");
                    char* err=dlerror();
                    if(err!=NULL){
                        fprintf(stderr, "\033[91;1mFatal Error\033[0m: Cannot load symbol `mod_helper', Reason:\n%s\n", err);
                        continue;
                    }
                    helper();
                }
                continue;
            }
            else{
                printf("LMFS WorkStation 2022 - Work ToolBox - BUILTIN\n");
                printf("    help <modules...>\n");
                printf("LMFSWorkStation Builtin.\n");
                continue;
            }
        }
        int FuncIndex=FindFunc(Arg.argv[0]);
        if(FuncIndex!=-1){
            type=Func[FuncIndex].type;
            double result;
            if(type!=-1){
                if(type==1){
                  result=GetFunc(Arg.argv[0]).Func(Arg);
                  if((int)result!=0){
                      printf("Result=%lf\n", result);
                  }
                }
                else if(type==2){
                    GetFunc(Arg.argv[0]).PtrFunc(Arg);
                }
            }
        }else{
            char** newargv=(char**)malloc((Arg.argc+2)*sizeof(char*));
            int _count=1;
            newargv[0]="execext";
            for(int i=0;i<Arg.argc;i++){
                newargv[_count]=(char*)malloc(strlen(Arg.argv[i])+2);
                memset(newargv[_count], 0, strlen(Arg.argv[i])+2);
                strcpy(newargv[_count++], Arg.argv[i]);
            }
            ArgList newarg={_count, newargv};
            GetFunc("execext").Func(newarg);
            for(int i=1;i<_count;i++)
              free(newargv[i]);
        }
    }
    CloseModules();
    CloseFuncPool();
    return 0;
}

