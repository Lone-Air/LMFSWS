/*
 * LMFS WorkStation - MODULE - COMMAND LINE PARSE
 *
 * Builtin
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#include "cmdline.h"

#include "module-loader.h"
#include "command-register.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SAVETO(a, counter, c) a=(char*)realloc(a, strlen(a)+1); \
                              a[counter++]=c;\

extern Array* parse(const char* s){
    size_t len=0;
    char** data=(char**)malloc(sizeof(char*)*1);
    int count=0;
    int ch;
    int str;
    int c;
    char* buf=(char*)calloc(0, 2);
    data[len]=(char*)calloc(0, 2);
    while((ch=*(s++))!='\0'){
        if(ch==' '||ch=='\t'){
            if(strcmp(buf, "")!=0){
                data[len]=(char*)calloc(0, count+1);
                strncpy(data[len], buf, count);
                data=(char**)realloc(data, (len+2)*sizeof(char*));
                data[++len]=(char*)calloc(0, 2);
                buf=(char*)calloc(0, 2);
                count=0;
            }
            continue;
        }
        switch(ch){
          case '"': case '\'':
            str=ch;
            while(ch!='\0'){
                ch=*(s++);
                switch(ch){
                  case '"': case '\'':
                    if(ch==str) break;
                    SAVETO(buf, count, ch);
                    break;
                  case '\\':
                    ch=*(s++);
                    if(ch=='\0'){
                        SAVETO(buf, count, ch);
                        break;
                    }
                    switch(ch){
                      case 'n': 
                        SAVETO(buf, count, '\n');
                        break;
                      case 't':
                        SAVETO(buf, count, '\t');
                        break;
                      case 'r':
                        SAVETO(buf, count, '\r');
                        break;
                      case 'e':
                        SAVETO(buf, count, '\033');
                        break;
                      case '0': case '1': case '2': case '3':\
                      case '4': case '5': case '6': case '7':\
                      case '8': case '9':
                        c=ch-48;
                        ch=*(s++);
                        if(ch=='\0'){
                            SAVETO(buf, count, c);
                            break;
                        }
                        switch(ch){
                          case '0': case '1': case '2': case '3':\
                          case '4': case '5': case '6': case '7':\
                          case '8': case '9':
                            c=c*10+ch-48;
                            ch=*(s++);
                            if(ch=='\0'){
                                SAVETO(buf, count, c);
                                break;
                            }
                            switch(ch){
                              case '0': case '1': case '2': case '3':\
                              case '4': case '5': case '6': case '7':\
                              case '8': case '9':
                                c=c*10+ch-48;
                                SAVETO(buf, count, c);
                                break;
                            };
                            break;
                        };
                        break;
                    }
                    break;
                  default:
                    SAVETO(buf, count, ch);
                    break;
                }
            }
            break;
          default:
            SAVETO(buf, count, ch);
            break;
        }
    }
    if(strcmp(buf, "")!=0){
        data[len]=(char*)calloc(0, count+1);
        strncpy(data[len], buf, count);
    }
    Array result[1];
    char** new=(char**)malloc(sizeof(char*)*1);
    long int newlen=0;
    for(int i=0;i<=len;i++){
        if(strcmp(data[i], "")!=0||i==0){
            new=(char**)realloc(new, sizeof(char*)*(newlen+1));
            new[newlen]=(char*)calloc(0, strlen(data[i])+1);
            strcpy(new[newlen++], data[i]);
        }
    }
    result[0]=(Array){new, newlen+1};
    Array* res=result;
    return res;
}

int mod_init(){ return 0; }
int mod_helper(){
    printf("LMFS WorkStation 2022 - Work ToolBox\n");
    printf("    parse <string>");
    printf("LMFSWorkStation Built-in. Don't Use it in a script!\n");
}

double NF(){ return -1; }

FuncList Regist[]={
      {"parse", 2, NF, (void*)parse},
      {NULL, -1, NF, NULL}
};
