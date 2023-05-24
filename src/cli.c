/*
 * LMFS WorkStation - Command line Interpreter - Arguments Parser
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#include "cli.h"
#include "cmdline.h"
#include "command-register.h"
#include "variable.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

char** EMPTY_LIST={0};
void* res;

resultpShell* rpS;
LMFSWS_State *L_s;

Ala* ALAM;
ArgumentsList* DefaultArgPoolHead;

extern double help_LMFSWS(int argc, char** argv){
    ArgumentsList* copy=DefaultArgPoolHead;
    fprintf(stdout, "LMFSWorkStation - LMFS org. - "
#include "VERSION"
            "\nUsage: %s [Options] [Files]\nOptions:\n", argv[0]);
    while(copy!=NULL){
        if(strcmp(copy->arg.desc, "")!=0)
          if(copy->arg.flagShort!='\0'&&strcmp(copy->arg.flagFull, "")!=0)
            fprintf(stdout, "    -%c --%s\t%s\n", copy->arg.flagShort, copy->arg.flagFull, copy->arg.desc);
          else if(copy->arg.flagShort!='\0'&&strcmp(copy->arg.flagFull, "")==0)
            fprintf(stdout, "    -%c\t%s\n", copy->arg.flagShort, copy->arg.desc);
          else
            fprintf(stdout, "    --%s\t%s\n", copy->arg.flagFull, copy->arg.desc);
        else
          if(copy->arg.flagShort!='\0'&&strcmp(copy->arg.flagFull, "")!=0)
            fprintf(stdout, "    -%c --%s\t<no description>\n", copy->arg.flagShort, copy->arg.flagFull);
          else if(copy->arg.flagShort!='\0'&&strcmp(copy->arg.flagFull, "")==0)
            fprintf(stdout, "    -%c\t<no description>\n", copy->arg.flagShort);
          else
            fprintf(stdout, "    --%s\t<no description>\n", copy->arg.flagFull);
        copy=copy->next;
    }
    return 0;
}

extern int InitArgPool(){
    ALAM=(Ala*)calloc(1, sizeof(Ala));
    if(ALAM==NULL) return -1;
    DefaultArgPool=(ArgumentsList*)calloc(1, sizeof(ArgumentsList));
    if(DefaultArgPool==NULL) return -1;
    DefaultArgPoolHead=DefaultArgPool;
    DefaultArgPool->last=NULL;
    DefaultArgPool->next=(ArgumentsList*)calloc(1, sizeof(ArgumentsList));
    if(DefaultArgPool->next==NULL) return -1;
    DefaultArgPool->arg.flagShort='h';
    DefaultArgPool->arg.flagFull="help";
    DefaultArgPool->arg.desc="Show this help";
    DefaultArgPool->arg.ArgType=5;
    DefaultArgPool->arg.callf=(_Function)help_LMFSWS;
    return 0;
}

extern void RegisterArg(Argument Argu){
    DefaultArgPool->last=DefaultArgPool;
    DefaultArgPool->next=(ArgumentsList*)calloc(1, sizeof(ArgumentsList));
    DefaultArgPool=DefaultArgPool->next;
    DefaultArgPool->next=NULL;
    DefaultArgPool->arg.flagShort=Argu.flagShort;
    if(Argu.flagFull!=NULL){
        DefaultArgPool->arg.flagFull=(char*)calloc(strlen(Argu.flagFull)+1, sizeof(char));
        strcpy(DefaultArgPool->arg.flagFull, Argu.flagFull);
    }
    else{
        DefaultArgPool->arg.flagFull=(char*)calloc(1, sizeof(char));
    }
    if(Argu.desc!=NULL){
        DefaultArgPool->arg.desc=(char*)calloc(strlen(Argu.desc)+1, sizeof(char));
        strcpy(DefaultArgPool->arg.desc, Argu.desc);
    }
    else{
        DefaultArgPool->arg.desc=(char*)calloc(1, sizeof(char));
    }
    DefaultArgPool->arg.ArgType=Argu.ArgType;
    DefaultArgPool->arg.callf=Argu.callf;
}

static double NF(){ return -1; }

extern Argument GetArgByFullFlag(char* fullflag){
    ArgumentsList* copy=DefaultArgPoolHead;
    while(copy!=NULL){
        if(strcmp(copy->arg.flagFull, fullflag)==0) return copy->arg;
        copy=copy->next;
    }
    return (Argument){0, NULL, NULL, -1, NF};
}

extern Argument GetArgByShortFlag(char shortflag){
    ArgumentsList* copy=DefaultArgPoolHead;
    while(copy!=NULL){
        if(copy->arg.flagShort==shortflag) return copy->arg;
        copy=copy->next;
    }
    return (Argument){0, NULL, NULL, -1, NF};
}

static bool check_including(char* s1, char c){
    int ch;
    while((ch=*s1)!='\0'){
        if(ch==c) return true;
        s1++;
        if(*s1=='\0') break;
    }
    return false;
}

static ArgWithAl decompose(char* s){
    char* buf=(char*)calloc(1, sizeof(char));
    unsigned int bufLen=0;
    char* name={0};
    bool nameSaved=false;
    char** argl=(char**)calloc(1, sizeof(char*));
    unsigned int arglLen=0;
    int ch;
    while((ch=*s++)!='\0'){
        switch(ch){
          case '=':
            if(!nameSaved){
                buf=(char*)realloc(buf, sizeof(char)*(bufLen+1));
                buf[bufLen++]='\0';
                name=(char*)calloc(strlen(buf)+1, sizeof(char));
                strcpy(name, buf);
                nameSaved=true;
                free(buf);
                buf=(char*)calloc(1, sizeof(char));
                bufLen=0;
            }else{
                buf=(char*)realloc(buf, sizeof(char)*(bufLen+1));
                buf[bufLen++]=ch;
            }
            break;
          case ',':
            if(nameSaved){
                argl=(char**)realloc(argl, sizeof(char*)*(arglLen+1));
                buf=(char*)realloc(buf, sizeof(char)*(bufLen+1));
                buf[bufLen++]='\0';
                buf=(char*)realloc(buf, sizeof(char)*(bufLen+1));
                buf[bufLen++]=ch;
                argl[arglLen]=(char*)calloc(strlen(buf)+1, sizeof(char));
                strcpy(argl[arglLen++], buf);
                free(buf);
                buf=(char*)calloc(1, sizeof(char));
                bufLen=0;
            }
            else{
                buf=(char*)realloc(buf, sizeof(char)*(bufLen+1));
                buf[bufLen++]=ch;
            }
            break;
          default:
            buf=(char*)realloc(buf, sizeof(char)*(bufLen+1));
            buf[bufLen++]=ch;
            break;
        }
    }
    if(strlen(buf)>0){
        if(!nameSaved){
            buf=(char*)realloc(buf, sizeof(char)*(bufLen+1));
            buf[bufLen++]=ch;
            name=(char*)calloc(strlen(buf)+1, sizeof(char));
            nameSaved=true;
            free(buf);
            buf=(char*)calloc(1, sizeof(char));
            bufLen=0;
        }
        else{
            buf=(char*)realloc(buf, sizeof(char)*(bufLen+1));
            buf[bufLen++]=ch;
            argl=(char**)realloc(argl, sizeof(char*)*(arglLen+2));
            argl[arglLen]=(char*)calloc(strlen(buf)+1, sizeof(char));
            strcpy(argl[arglLen++], buf);
            free(buf);
            buf=(char*)calloc(1, sizeof(char));
            bufLen=0;
        }
    }
    return (ArgWithAl){name, (ArgList){arglLen, argl}};
}

extern void callArgs(int argc, char** argv){
    char* s; int _c=1;
    char** argL=(char**)calloc(1, sizeof(char*));
    int argLlen=0;
    char* default_s;
    Argument temp;
    char** argvHead=argv;
    while(_c<argc){
        argv++;
        s=*argv;
        default_s=s;
        if(*s!='-'){
            runNormallyFile(s);
            _c++;
            continue;
        }
        if(*s=='-') s++;
        if(*s=='-') s++;
        if(check_including(s, '=')){
            ArgWithAl awitha=decompose(s);
            if(strlen(awitha.name)==1){
                temp=GetArgByShortFlag(awitha.name[0]);
                if(temp.flagFull==NULL){
                    fprintf(stderr, "\033[91;1mFatal Error\033[0m: '%s' is unrecognized\n", awitha.name);
                    break;
                }
                if(temp.ArgType==1){
                    callArgFunc(temp, awitha.arg);
                }
                else{
                    fprintf(stderr, "\033[91;1mFatal Error\033[0m: Do you mean to call argument '%s'? If that's what you mean, Maybe '%s' was written in wrong format. In register it's not '1'!\n", awitha.name, default_s);
                    break;
                }
            }
            else{
                temp=GetArgByFullFlag(awitha.name);
                if(temp.flagFull==NULL){
                    fprintf(stderr, "\033[91;1mFatal Error\033[0m: '%s' is unrecognized\n", awitha.name);
                    break;
                }
                if(temp.ArgType==1){
                    callArgFunc(temp, awitha.arg);
                }
                else{
                    fprintf(stderr, "\033[91;1mFatal Error\033[0m: Do you mean to call argument '%s'? If that's what you mean, Maybe '%s' was written in wrong format. In register it's not '1'!\n", awitha.name, default_s);
                    break;
                }
            }
            for(int i=0; i<awitha.arg.argc; i++) free(awitha.arg.argv[i]);
            free(awitha.arg.argv);
        }
        else{
            if(strlen(s)==1){
                temp=GetArgByShortFlag(s[0]);
                if(temp.flagFull==NULL){
                    fprintf(stderr, "\033[91;1mFatal Error\033[0m: '%s' is unrecognized\n", s);
                    break;
                }
            }
            else{
                temp=GetArgByFullFlag(s);
                if(temp.flagFull==NULL){
                    fprintf(stderr, "\033[91;1mFatal Error\033[0m: '%s' is unrecognized\n", s);
                    break;
                }
            }
            switch(temp.ArgType){
              case 1:
                callArgFunc(temp, (ArgList){0, EMPTY_LIST});
                break;
              case 2:
                if(_c+1<argc){
                    argv++;
                    s=*argv;
                    _c++;
                    while(_c<argc){
                        argL=(char**)realloc(argL, sizeof(char*)*(argLlen+2));
                        argL[argLlen]=(char*)calloc(strlen(s)+1, sizeof(char));
                        strcpy(argL[argLlen++], s);
                        argv++;
                        s=*argv;
                        _c++;
                        if(_c<argc)
                          if(*s=='-'){
                              argv--;
                              _c--;
                              break;
                          }
                    }
                }
                callArgFunc(temp, (ArgList){argLlen, argL});
                for(int i=0;i<argLlen;i++)
                  free(argL[i]);
                free(argL);
                argL=(char**)calloc(1, sizeof(char*));
                argLlen=0;
                break;
              case 3:
                callArgFunc(temp, (ArgList){0, EMPTY_LIST});
                break;
              case 4:
                if(_c+1<argc){
                    argv++;
                    s=*argv;
                    _c++;
                    if(_c<argc){
                        if(*s!='-'){
                            argL=(char**)realloc(argL, sizeof(char*)*(argLlen+2));
                            argL[argLlen]=(char*)calloc(strlen(s)+1, sizeof(char));
                            strcpy(argL[argLlen++], s);
                        }
                        else{
                            argv--;
                            _c--;
                        }
                    }
                }
                callArgFunc(temp, (ArgList){argLlen, argL});
                for(int i=0;i<argLlen;i++)
                  free(argL[i]);
                free(argL);
                argL=(char**)calloc(1, sizeof(char*));
                argLlen=0;
                break;
              case 5:
                callArgFunc(temp, (ArgList){argc, argvHead});
                break;
            }
        }
        _c++;
    }
}

extern double callArgFunc(Argument arg, ArgList al){
    _Function argfunc=arg.callf;
    double result=argfunc(al.argc, al.argv);
    return result;
}

extern double doLMFSWSCmd(const char* data){
    if(data==NULL){
        fprintf(stderr, "\033[91;1mFatal Error\033[0m: LMFSWS Command cannot be address 0x0!\n");
        return -1;
    }
    PtrFunction parse=GetFunc("parse").PtrFunc;
    ArrayList* cmds=parse(data);
    double result=0;
    for(int inter=0; inter<cmds->len; inter++){
        if(inter!=0){
            for(int j=0; j<cmds->l[inter-1]->length; j++){
                free(cmds->l[inter-1]->data[j]);
            }
            free(cmds->l[inter-1]->data);
        }
        Array* Args=cmds->l[inter];
        ArgList Arg;
        int type;
        if(Args[0].length<1||strcmp(Args[0].data[0], "")==0) continue;
        if(Args[0].data[0][0]=='#') return 0;
        Arg.argc=Args[0].length;
        Arg.argv=Args[0].data;
        if(strcmp(Arg.argv[0], "help")==0){
            if(Arg.argc>1){
                for(int i=1;i<Arg.argc;i++){
                    if(strcmp(Arg.argv[i], "help")==0){
                        printf("LMFS WorkStation - Work ToolBox - BUILTIN\n");
                        printf("    help <modules...>\n");
                        printf("LMFSWorkStation Builtin.\n");
                        continue;
                    }
                    int index=FindModule(Arg.argv[i]);
                    _Function helper=dlsym(Modules[index].dlheader, "mod_helper");
                    char* err=dlerror();
                    if(err!=NULL){
                        //fprintf(stderr, "\033[91;1mFatal Error\033[0m: Cannot load symbol `mod_helper', Reason:\n%s", err);
                        fprintf(stderr, "helper: <no document>\n");
                        return -1;
                    }
                    helper();
                }
                return 0;
            }
            else{
                printf("LMFS WorkStation 2022 - Work ToolBox - BUILTIN\n");
                printf("    help <modules...>\n");
                printf("LMFSWorkStation Builtin.\n");
                return 0;
            }
        }
        int FuncIndex=FindFunc(Arg.argv[0]);
        if(FuncIndex==-1){
            fprintf(stderr, "\033[91;1mError\033[0m: No function named '%s'\n", Arg.argv[0]);
            return -1;
        }
        if(FuncIndex!=-1){
            type=Func[FuncIndex].type;
            if(type!=-1){
                int isusermode=CheckUserMode(Arg.argv[0]);
                if(isusermode==1){
                    fprintf(stderr, "\033[91;1mOperation denied\033[0m: This function may just for <Program internal run mode>\n");
                    return -1;
                }
                if(isusermode==-1){
                    fprintf(stderr, "\033[91;1mMode Checker Failed\033[0m: This function was registered with unknow execution type\n");
                    return -1;
                }
                if(type==1){
                    result=GetFunc(Arg.argv[0])._Func(Arg.argc, Arg.argv);
                    if((int)result!=0){
                        printf("Result=%lf\n", result);
                    }
                }
                else if(type==2){
                    setresult(GetFunc(Arg.argv[0]).PtrFunc(Arg));
                }
            }
        }
        else{
        	fprintf(stderr, "\033[91;1mError\033[0m: `%s' not found\n", Arg.argv[0]);
        	return -1;
        }
    }
    return result;
}

/*
 char* s=(char*)malloc(2, sizeof(char));
 strcpy(s, "s");
 setresult(s); // Don't free `s'
 */
extern void setresult(char* s){
    free(rpS->resultp);
    rpS->resultp=s;
}

/*
 setresult_automalloc("abcdefg");
 */
extern void setresult_automalloc(const char* s){
    free(rpS->resultp);
    rpS->resultp=(char*)calloc(strlen(s)+1, sizeof(char));
    strcpy(rpS->resultp, s);
}

extern void UseState(LMFSWS_State* L){
    ALAM=L->als;
    DefaultArgPool=L->ale;
    rpS=L->rps;
    FuncL=L->fla;
    ModL=L->ml;
    _main_p=L->_mp;
}

extern int InitLMFSWS(){
    if(InitArgPool()!=0) return -1;
    if(InitModulePool()!=1) return -1;
    if(InitFuncPool()!=0){
        CloseModules();
        return -1;
    }
    if(initia_path()!=0) return -1;
    if(InitVarPool()!=0) return -1;
    rpS=(resultpShell*)calloc(1, sizeof(resultpShell));
    L_s=(LMFSWS_State*)calloc(1, sizeof(LMFSWS_State));
    L_s->als=ALAM;
    L_s->ale=DefaultArgPool;;
    L_s->rps=rpS;
    L_s->fla=FuncL;
    L_s->ml=ModL;
    L_s->_mp=_main_p;
    L_s->varp=default_varpool;
    if(LoadModuleByName("Input")!=-1){
        if(regist("Input")==-1) return -1;
    }
    else return -1;
    if(LoadModuleByName("cmdline")!=-1){
        if(regist("cmdline")==-1) return -1;
    }
    else return -1;
    if(LoadModuleByName("tools")!=-1){
        if(regist("tools")==-1) return -1;
    }
    else return -1;
    if(LoadModuleByName("register")!=-1){
        if(regist("register")==-1) return -1;
    }
    else return -1;
    if(LoadModuleByName("var")!=-1){
        if(regist("var")==-1) return -1;
    }
    else return -1;
#ifdef ENABLE_LOGIN
    if(LoadModuleByName("login")!=-1){
        if(regist("login")==-1) return -1;
    }
    else return -1;
#endif
    return 0;
}

extern int runNormallyFile(char* fp){
    FILE* _F=fopen(fp, "r");
    if(_F==NULL){
        fprintf(stderr, "\033[91;1mError\033[0m: Cannot open file: %s\n", fp);
        return -1;
    }
    char* buf=(char*)calloc(2, sizeof(char));
    unsigned int bufLen=0;
    int ch;
    int status;
    int lines=0;
    while((ch=fgetc(_F))!=EOF){
        if(ch=='\n'){
            lines++;
            buf[bufLen++]='\0';
            status=doLMFSWSCmd(buf);
            free(buf);
            buf=(char*)calloc(2, sizeof(char));
            bufLen=0;
            if(status==-1){
                fprintf(stderr, "Error found at %s, line %d\n", fp, lines);
            }
            continue;
        }
        buf=(char*)realloc(buf, sizeof(char)*(bufLen+2));
        buf[bufLen++]=ch;
    }
    if(strcmp(buf, "")!=0){
        lines++;
        buf[bufLen++]='\0';
        status=doLMFSWSCmd(buf);
        free(buf);
        buf=(char*)calloc(2, sizeof(char));
        bufLen=0;
        if(status==-1){
            fprintf(stderr, "Error found at %s, line %d\n", fp, lines);
        }
    }
    fclose(_F);
    CloseModules();
    CloseFuncPool();
    if(InitModulePool()!=1) exit(1);
    if(InitFuncPool()!=0){
        CloseModules();
        exit(1);
    }
    if(InitLMFSWS()==-1) exit(1);
    return 0;
}

extern void CloseArgPool(){
    while(DefaultArgPool->next!=NULL){
        DefaultArgPool=DefaultArgPool->next;
    }
    while(DefaultArgPool->last!=NULL){
        if(DefaultArgPool->last!=NULL){
            free(DefaultArgPool->arg.flagFull);
            free(DefaultArgPool->arg.desc);
            DefaultArgPool=DefaultArgPool->last;
            free(DefaultArgPool->next);
        }
    }
    free(ALAM);
}

extern void* get_result(){
    return result_p;
}

extern void CleanMemory(){
    CloseArgPool();
    CloseModules();
    CloseFuncPool();
    ClearPath();
    CleanVariables();
}

extern void ForceQuit(int status){
    CleanMemory();
    exit(status);
}

