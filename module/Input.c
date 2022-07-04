/*
 * LMFS WorkStation - Work ToolBox - Input
 *
 * Builtin. LICENSE SEE $ROOT/LICENSE
 */

#include "config.h"

#include "module-loader.h"
#include "command-register.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif

double NULLFUNC(){ return -1; }

int mod_init(){
    return 0;
}

int mod_helper(){
    printf("LMFS WorkStation 2022 - Work ToolBox\n");
    printf("    input <prompt>\n");
    printf("LMFSWorkStation Built-in Module. Don't Use it in a script!\n");
    return 0;
}

char* input(const char* prompt){
    char* res;
#ifndef HAVE_READLINE
    printf("%s", prompt);
    res=(char*)calloc(0, 2);
    char ch;
    while(1){
        ch=getchar();
        if(ch=='\n'||ch=='\0') break;
        res=(char*)realloc(res, strlen(res)+1);
        res[strlen(res)]=ch;
    }
#else
    res=readline(prompt);
    if(res&&*res) add_history(res);
#endif
    return res;
}

int mod_start(int argc, char* argv[]){
    return 0;
}

FuncList Regist[]={
      {"input", 4, NULLFUNC, (void*)input},
      {NULL, -1, NULLFUNC, NULL}
};

