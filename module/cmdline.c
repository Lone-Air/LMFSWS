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

#define SAVETO(a, counter, c) a=(char*)realloc(a, (counter+1)*sizeof(char)); \
                              a[counter++]=c;\

extern ArrayList* parse(const char* s){
    size_t len=0;
    char** data=(char**)calloc(2, sizeof(char*));
    int count=0;
    int ch;
    int str;
    int c;
    char* buf=(char*)calloc(1, sizeof(char));
    data[len]=(char*)calloc(1, sizeof(char));
    Array** list=(Array**)calloc(1, sizeof(Array*));
    unsigned int listlen=0;
    char** new;
    unsigned int newlen=0;
    while((ch=*(s++))!='\0'){
        if(ch==' '||ch=='\t'){
            if(strcmp(buf, "")!=0){
                data[len]=(char*)calloc(count+1, sizeof(char));
                strncpy(data[len], buf, count);
                data=(char**)realloc(data, (len+2)*sizeof(char*));
                data[++len]=(char*)calloc(2, sizeof(char));
                buf=(char*)calloc(2, sizeof(char));
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
                      case '"': case '\'':
                        SAVETO(buf, count, ch);
                        break;
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
          case ';':
            if(strcmp(buf, "")!=0){
                SAVETO(buf, count, '\0'); // Without this line may cause may bugs on windows!!!
                data[len]=(char*)calloc(strlen(buf)+2, sizeof(char));
                strcpy(data[len], buf);
                free(buf);
                buf=(char*)calloc(2, sizeof(char));
                count=0;
            }
            new=(char**)calloc(2, sizeof(char*));
            newlen=0;
            for(int i=0;i<=len;i++){
                if(strcmp(data[i], "")!=0||i==0){
                    new=(char**)realloc(new, sizeof(char*)*(newlen+2));
                    new[newlen]=(char*)calloc(strlen(data[i])+1, sizeof(char));
                    strcpy(new[newlen++], data[i]);
                }
                if(i!=len)
                  free(data[i]);
            }
            data=(char**)calloc(2, sizeof(char*));
            len=0;
            data[len]=(char*)calloc(1, sizeof(char));
            list=(Array**)realloc(list, sizeof(Array*)*(listlen+2));
            list[listlen]=(Array*)calloc(1, sizeof(Array));
            if(newlen==0) newlen=1;
            list[listlen]->length=newlen;
            list[listlen++]->data=new;
            break;
          default:
            SAVETO(buf, count, ch);
            break;
        }
    }
    if(strcmp(buf, "")!=0){
        SAVETO(buf, count, '\0'); // Without this line may cause may bugs on windows!!!
        data[len]=(char*)calloc(strlen(buf)+2, sizeof(char));
        strcpy(data[len], buf);
    }
    new=(char**)calloc(2, sizeof(char*));
    newlen=0;
    for(int i=0;i<=len;i++){
        if(strcmp(data[i], "")!=0||i==0){
            new=(char**)realloc(new, sizeof(char*)*(newlen+2));
            new[newlen]=(char*)calloc(strlen(data[i])+2, sizeof(char));
            strcpy(new[newlen++], data[i]);
        }
        if(i!=len)
          free(data[i]);
    }
    list=(Array**)realloc(list, sizeof(Array*)*(listlen+3));
    list[listlen]=(Array*)calloc(2, sizeof(Array));
    if(newlen==0) newlen=1;
    list[listlen]->length=newlen;
    list[listlen++]->data=new;
    ArrayList* result=(ArrayList*)calloc(2, sizeof(ArrayList));
    *result=(ArrayList){listlen, list};
    return result;
}

int mod_init(){ return 0; }
int mod_helper(){
    printf("LMFS WorkStation - Work ToolBox\n");
    printf("    parse <string>\n");
    printf("LMFSWorkStation Built-in. Don't Use it in a script!\n");
    return 0;
}

double NF(){ return -1; }

FuncList Regist[]={
      {"parse", 4, NF, (void*)parse},
      {NULL, -1, NF, NULL}
};

