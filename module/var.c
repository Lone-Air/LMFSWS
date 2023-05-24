/*
 * LMFS WorkStation - MODULE - VARIABLE CONTROLLER - USER
 *
 * Builtin
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#include "command-register.h"
#include "module-loader.h"
#include "cli.h"
#include "variable.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int mod_init(LMFSWS_State* L){
    UseState(L);
    return 0;
}

int mod_helper(){
    printf(
            "LMFS WorkStation - Core Module - Variable\n"
            "    printvar <variable names...>\n"
            "    createvar <str/list> <name>\n"
          );
    return 0;
}

static char* copystr(char* str){
    char* s=(char*)malloc(sizeof(char)*(strlen(str)+1));
    strcpy(s, str);
    return s;
}

double create(int argc, char* argv[]){
    if(argc<3){
        fprintf(stderr, "\033[91;1m%s: need 2 arguments\033[0m\n", argv[0]);
        return -1;
    }
    if(search_var(argv[2])!=-1)
      return 0;
    if(strcmp(argv[1], "str")==0)
      newVar(argv[2], VAR_STR, (var_data){copystr("")});
    else if(strcmp(argv[1], "list")==0){
        char** arg=(char**)calloc(1, sizeof(char*));
        var_data data;
        data.listData.argc=0;
        data.listData.argv=arg;
        data.strData=NULL;
        newVar(argv[2], VAR_LIST, data);
    }
    else{
        fprintf(stderr, "\033[91;1m:%s: invalid type\033[0m\n", argv[0]);
        return -1;
    }
    return 0;
}

double printvar(int argc, char* argv[]){
    for(int i=1; i<argc; i++){
        int id=search_var(argv[i]);
        if(id==-1){
            fprintf(stderr, "\033[91;1mNameError\033[0m: no variable named %s\n", argv[i]);
            return -1;
        }
        var_struct var=vpool[id];
        switch(var.type){
          case VAR_STR:
            printf("%s", var.data.strData);
            break;
          case VAR_LIST:
            for(int j=0; j<var.data.listData.argc; j++)
                printf("%s%c", var.data.listData.argv[i], j==argc-1?'\0':' ');
            break;
        }
    }
    return 0;
}

FuncList Regist[]={
    {"printvar", 1, printvar, NULL},
    {"createvar", 1, create, NULL},
    {NULL, -1, NULL, NULL}
};

