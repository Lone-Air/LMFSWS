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
    printf("LMFS WorkStation - Work ToolBox\n");
    printf("    prompt <prompt...>\n");
    printf("LMFSWorkStation Built-in Module.\n");
    return 0;
}

char* input(const char* prompt){
    char* res;
#ifndef HAVE_READLINE
    int _c=0;
    int ch=0;
    printf("%s", prompt);
    res=(char*)calloc(2, sizeof(char*));
    while(1){
        ch=getchar();
        if(ch=='\n'||ch=='\0') break;
        if(ch==EOF){
            putchar(10);
            return NULL;
        }
        res=(char*)realloc(res, ++_c);
        res[_c-1]=ch;
    }
    res=(char*)realloc(res, ++_c);
    res[_c-1]='\0';
#else
    res=readline(prompt);
    if(res&&*res) add_history(res);
#endif
    return res;
}

void* userinput(int argc, char* argv[]){
    for(int i=1;i<argc;i++)
      printf("%s%c", argv[i], i+1>=argc?'\0':' ');
    return input("");
}

int mod_start(){
    return 0;
}

FuncList Regist[]={
      {"input", 4, NULLFUNC, (void*)input},
      {"prompt", 2, NULLFUNC, (void*)userinput},
      {NULL, -1, NULLFUNC, NULL}
};

