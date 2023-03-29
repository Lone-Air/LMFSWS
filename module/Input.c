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

char* first_x_data(char* _s, int x){
	char* result=(char*)calloc(x*sizeof(char)+1);
	for(int i=0;i<x;i++){
	    result[i]=_s[i];
	    if(i==x-1)
	      result[i+1]='\0';
	}
	return result;
}

char* Generator(const char* part, int state){
    char* ptr;
    char* result;
    char* temp;
    int ok=0;
    for(int i=0;i<FuncNum;i++){
        ptr=Func[i].name;
        temp=first_x_data(ptr, strlen(part));
        if(strcmp(temp, part)==0 || strcmp(part, "")==0){
            if(state>ok)
              ok++;
            else{
                result=(char*)calloc(sizeof(char)*strlen(ptr)+1);
                strcpy(result, ptr);
                return result;
            }
        }
        free(temp);
    }
    return NULL;
}

int mod_init(){
    rl_completion_entry_function=Generator;
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
    char* prompt=(char*)calloc(1, sizeof(char));
    size_t len=1;
    for(int i=1;i<argc;i++){
        len=len+strlen(argv[i])+1;
        prompt=(char*)realloc(prompt, len*sizeof(char));
        if(i!=1) strcat(prompt, " ");
        strcat(prompt, argv[i]);
    }
    char* result=input(prompt);
    free(prompt);
    return result;
}

FuncList Regist[]={
      {"input", 4, NULLFUNC, (void*)input},
      {"prompt", 2, NULLFUNC, (void*)userinput},
      {NULL, -1, NULLFUNC, NULL}
};

