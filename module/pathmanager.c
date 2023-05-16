/*
 * LMFS WorkStation - MODULE - MODULE LOADER PATH MANAGER
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

int mod_init(LMFSWS_State* L){
    UseState(L);
    return 0;
}

int mod_helper(){
    printf("LMFS WorkStation - Work ToolBox - Core Module\n");
    printf(
           "    add_path <paths...>\n"
           "    list_paths\n"
           "    del_paths <counts...>\n"
          );
    printf("LMFSWorkStation Built-in command.\n");
    return 0;
}

double add_p(int argc, char* argv[]){
    for(int i=1;i<argc;i++){
        add_path(argv[i]);
    }
    return 0;
}

double list_p(int argc, char* argv[]){
    for(int i=0;i<pathL;i++)
      printf("%d|\t%s\n", i, pathA[i]);
    return 0;
}

static int sToi(char*);

double del_p(int argc, char* argv[]){
    char** new_p=(char**)calloc(1, sizeof(char*));
    int length_p=0;
    bool callcontinue=false;
    for(int j=0;j<pathL;j++){
        for(int i=1;i<argc;i++)
          if(j==sToi(argv[i])){
              callcontinue=true;
              break;
          }
        if(callcontinue==true){
            callcontinue=false;
            continue;
        }
        new_p=(char**)realloc(new_p, sizeof(char*)*(length_p+1));
        new_p[length_p]=(char*)calloc(strlen(pathA[j]), sizeof(char));
        strcpy(new_p[length_p++], pathA[j]);
    }
    for(int i=0;i<pathL;i++){
        free(pathA[i]);
    }
    free(pathA);
    pathA=new_p;
    pathL=length_p;
    return 0;
}

static int sToi(char* s){
    int ch;
    int result=0;
    while((ch=*(s++))!=0){
        if(ch>='0'&&ch<='9'){
            result=result*10+ch-48;
        }
        else{
            fprintf(stderr, "\033[91;1mError\033[0m: Not an legal integer number!\n");
            return -1;
        }
    }
    return result;
}

FuncList Regist[]={
      {"add_path", 1, add_p, NULL},
      {"list_paths", 1, list_p, NULL},
      {"del_paths", 1, del_p, NULL},
      {NULL, -1, NULL, NULL}
};
