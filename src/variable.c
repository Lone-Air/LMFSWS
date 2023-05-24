/*
 * LMFS WorkStation - Variable System - SOURCE FILE
 *
 * LICENSE: $ROOT/LICENSE
 */

#include "cli.h"
#include "variable.h"
#include <string.h>
#include <stdlib.h>

struct varpool* default_varpool;
ArgList emptylist;

int InitVarPool(){
    emptylist.argc=0;
    emptylist.argv=NULL;
    default_varpool=(varpool*)malloc(sizeof(varpool));
    vlen=0;
    vpool=(var_struct*)malloc(sizeof(var_struct)*1);
    if(vpool==NULL) return -1;
    char* version_lmfsws=(char*)malloc(sizeof(char)*(strlen(VERSION)));
    strcpy(version_lmfsws, VERSION);
    newVar("VERSION", VAR_STR, (var_data){
           version_lmfsws
           });
    return 0;
}

void newVar(char* name, var_type type, var_data data){
    vpool=(var_struct*)realloc(vpool, sizeof(var_struct)*(vlen+1));
    vpool[vlen].name=(char*)malloc(sizeof(char)*(1+strlen(name)));
    strcpy(vpool[vlen].name, name);
    vpool[vlen].type=type;
    vpool[vlen++].data=data;
}

int search_var(char* name){
    for(int i=0; i<vlen; i++){
        if(strcmp(name, vpool[i].name)==0)
          return i;
    }
    return -1;
}

int update_var(char* name, var_type type, var_data data){
    int id=search_var(name);
    if(id==-1) return -1;
    switch(vpool[id].type){
      case VAR_STR:
        free(vpool[id].data.strData);
        break;
      case VAR_LIST:
        for(int j=0;j<vpool[id].data.listData.argc;j++)
          free(vpool[id].data.listData.argv[j]);
        free(vpool[id].data.listData.argv);
        break;
    }
    vpool[id].data=data;
    return -1;
}

int delete_var(char* name){
    int id=search_var(name);
    if(id==-1) return -1;
    varpool* new_vpool=(varpool*)malloc(sizeof(varpool));
    new_vpool->var_pool=(var_struct*)malloc(sizeof(var_struct));
    int new_vlen=0;
    for(int i=0; i<vlen; i++){
        if(i==id){
            destroy_var(i);
            continue;
        }
        new_vpool->var_pool=(var_struct*)realloc(new_vpool->var_pool, sizeof(var_struct)*(new_vlen+1));
        new_vpool->var_pool[new_vlen++]=vpool[i];
    }
    vpool=new_vpool->var_pool;
    vlen=new_vlen;
    return 0;
}

void destroy_var(int id){
    free(vpool[id].name);
    switch(vpool[id].type){
      case VAR_STR:
        free(vpool[id].data.strData);
        break;
      case VAR_LIST:
        for(int j=0;j<vpool[id].data.listData.argc;j++)
          free(vpool[id].data.listData.argv[j]);
        free(vpool[id].data.listData.argv);
        break;
    }
}

int CleanVariables(){
    for(int i=0;i<vlen;i++){
        destroy_var(i);
    }
    free(vpool);
    free(default_varpool);
    return 0;
}
