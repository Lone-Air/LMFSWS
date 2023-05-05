/*
 * LMFS WorkStation
 * LICENSE SEE $ROOT/LICENSE
 *
 * SECURE SECTION - LOGIN SYSTEM - Optional
 *
 * Dependence: libcrypto
 */

#include "command-register.h"
#include "module-loader.h"
#include "cli.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include <unistd.h>

#include <sys/stat.h>
#include <sys/types.h>

#include <openssl/sha.h> // sha256

#ifdef SIZE
  #undef SIZE
  #define SIZE(x) sizeof(x)/sizeof(x[0])
#endif

#if !(__USE_MISC || (defined __USE_XOPEN && !defined __USE_XOPEN2K))
extern char *getpass (const char *__prompt); //__nonnull ((1));
#endif

#if !(defined __USE_XOPEN_EXTENDED || defined __USE_XOPEN2K)
extern ssize_t readlink (const char *__restrict __path,
			 char *__restrict __buf, size_t __len);
     //__THROW __nonnull ((1, 2)) __wur;
#endif

int mod_helper(){
    printf(
           "LMFS WorkStation - Security Module\n"
           "    chpasswd <user>\n"
           "    deluser <user>\n"
          );
    return 0;
}

typedef struct shadow_t{
    int* users;
    char** codes;
    unsigned int userl;
    unsigned codel;
}shadow_t;

static int sToi(char* s){
    int ch;
    int result=0;
    while((ch=*(s++))!=0){
        if(ch>='0'&&ch<='9'){
            result=result*10+ch-48;
        }
        else{
            return -1;
        }
    }
    return result;
}

shadow_t parseSha(const char* fp){
    int* userl;
    char** codel;
    shadow_t result;
    int ch;
    FILE* _f=fopen(fp, "r");
    bool id_fin=false;
    char* buf;
    buf=(char*)calloc(1, sizeof(char));
    size_t bl=0;
    size_t ul=0;
    size_t cl=0;
    userl=(int*)calloc(1, sizeof(int));
    codel=(char**)calloc(1, sizeof(char*));
    if(_f==NULL){
        userl[0]=-1;
        codel[0]=NULL;
        result.users=userl;
        result.codes=codel;
        result.userl=1;
        result.codel=1;
        return result;
    }
    unsigned int ln=1;
    while((ch=fgetc(_f))!=EOF){
        if(ch!='\n'){
            if(ch!=':'){
save_char:
                buf=(char*)realloc(buf, (++bl+1)*sizeof(char));
                buf[bl-1]=ch;
            }
            else{
                if(!id_fin){
                    buf=(char*)realloc(buf, (++bl+1)*sizeof(char));
                    buf[bl-1]='\0';
                    id_fin=true;
                    userl=(int*)realloc(userl, (++ul+1)*sizeof(int));
                    userl[ul-1]=sToi(buf);
                    if(userl[ul-1]==-1){
                        fprintf(stderr, "\033[91;1mSecurity Section - Failed: illegal uid at line %u\n", ln);
                        ForceQuit(1);
                    }
                    free(buf);
                    buf=(char*)calloc(1, sizeof(char));
                    bl=0;
                }
                else
                  goto save_char;
            }
        }
        else{
save_code:
            buf=(char*)realloc(buf, (++bl+1)*sizeof(char));
            buf[bl-1]='\0';
            codel=(char**)realloc(codel, (++cl+1)*sizeof(char*));
            codel[cl-1]=calloc(strlen(buf)+1, sizeof(char));
            strcpy(codel[cl-1], buf);
            free(buf);
            buf=(char*)calloc(1, sizeof(char));
            bl=0;
            id_fin=false;
        }
    }
    if(id_fin)
      goto save_code;
    userl=(int*)realloc(userl, (++ul)*sizeof(int));
    codel=(char**)realloc(codel, (++cl)*sizeof(char*));
    userl[ul-1]=-1;
    codel[cl-1]=NULL;
    result.users=userl;
    result.codes=codel;
    result.userl=ul;
    result.codel=cl;
    fclose(_f);
    return result;
}

static int writeSha(shadow_t sha, const char* fp){
    unsigned int counter=0;
    FILE* _f=fopen(fp, "w");
    if(_f==NULL){
        return -1;
    }
    while(sha.codes[counter]!=NULL && sha.users[counter]!=-1){
        char* text=(char*)calloc(strlen(sha.codes[counter])+15, sizeof(char));
        sprintf(text, "%d", sha.users[counter]);
        strcat(text, ":");
        strcat(text, sha.codes[counter]);
        strcat(text, "\n");
        fputs(text, _f);
        free(text);
        counter++;
    }
    fclose(_f);
    return 0;
}

static void destroySha(shadow_t sha){
    free(sha.users);
    while(*sha.codes!=NULL){
        free(*sha.codes);
        sha.codes++;
    }
    free(sha.codes)
}

static bool islink(const char* file_p){
    char buf[1024];
    size_t len;
    if((len=readlink(file_p, buf, 1024-1))!=-1){
        return true;
    }
    return false;
}

int mod_init(LMFSWS_State* L){
    UseState(L);
    if(access(SHADOWPATH, F_OK)==0){
        if(islink(SHADOWPATH)){
            fprintf(stderr, "\033[91;1mSecurity Alert\033[0m: %s cannot be a link\n", SHADOWPATH);
            ForceQuit(1);
        }
        struct stat file;
        stat(SHADOWPATH, &file);
        int mod=file.st_mode;
        int counter=0;
        uid_t uid=getuid();
        if(mod&S_IXUSR || mod&S_IWGRP || mod&S_IXGRP || mod&S_IWOTH || mod&S_IXOTH){
            fprintf(stderr, "\033[91;1mSecurity Alert\033[0m: permission of %s is not 0644(-rw-r--r--)\n", SHADOWPATH);
            ForceQuit(1);
        }
        shadow_t sha=parseSha(SHADOWPATH);
        int trys=0;
        bool logged=false;
        while(sha.codes[counter]!=NULL){
            if(sha.users[counter]==uid){
                if(strcmp(sha.codes[counter], "x")==0){
                    logged=true;
                    break;
                }
                unsigned char* code_sha256=(unsigned char*)calloc(257, sizeof(unsigned char));
                char* passwd=getpass("Enter login password: ");
                SHA256_CTX ctx;
                SHA256_Init(&ctx);
                SHA256_Update(&ctx, passwd, sizeof(passwd));
                SHA256_Final(code_sha256, &ctx);
                char code_hex[128]={0};
                for(int i=0; i<strlen(code_sha256); i++){
                    snprintf(code_hex+i*2, 3, "%02x", code_sha256[i]);
                }
                if(strcmp(code_hex, sha.codes[counter])!=0){
                    fprintf(stderr, "\033[91;1mSecurity Alert\033[0m: Authentication failed\n");
                    trys++;
                    if(trys>=3){
                        fprintf(stderr, "\033[91;1mSecurity Alert\033[0m: Permission denied\n");
                        ForceQuit(1);
                    }
                    else
                      continue;
                }
                free(code_sha256);
                logged=true;
            }
            counter++;
        }
        if(!logged){
            fprintf(stderr, "\033[91;1mSecurity Alert\033[0m: you arenot in %s, please contact the installer of LMFSWS for help\n", SHADOWPATH);
            ForceQuit(1);
        }
        destroySha(sha);
    }
    else{
        fprintf(stderr, "\033[91;1mSecurity Alert - Fatal error\033[0m: %s was not found\n", SHADOWPATH);
        ForceQuit(1);
    }
    return 0;
}

double change_passwd(int argc, char* argv[]){
    if(argc<2){
        fprintf(stderr, "\033[91;1mError\033[0m: require an argument\n");
        return -1;
    }
    if(access(SHADOWPATH, F_OK)==0){
        struct stat file;
        stat(SHADOWPATH, &file);
        uid_t owner=file.st_uid;
        if(owner!=getuid()){
            fprintf(stderr, "\033[91;1mSecurity Alert\033[0m: you arenot the owner of %s, please contact the installer of LMFSWS for help\n", SHADOWPATH);
            return -1;
        }
        if(!(0<=sToi(argv[1])&&sToi(argv[1])<=65535)){
            fprintf(stderr, "\033[91;1mFailed\033[0m: Illegal uid\n");
            return -1;
        }
        shadow_t sha=parseSha(SHADOWPATH);
        unsigned counter=0;
        char* code=getpass("New password: ");
        char* code_hex=(char*)calloc(129, sizeof(char));
        if(strcmp(code, getpass("Confirm password: "))==0){
            unsigned char* code_sha256=(unsigned char*)calloc(257, sizeof(unsigned char));
            SHA256_CTX ctx;
            SHA256_Init(&ctx);
            SHA256_Update(&ctx, code, sizeof(code));
            SHA256_Final(code_sha256, &ctx);
            if(strcmp(code, "")!=0){
                for(int i=0; i<strlen(code_sha256); i++){
                    snprintf(code_hex+i*2, 3, "%02x", code_sha256[i]);
                }
            }
            else
              strcpy(code_hex, "x");
            free(code_sha256);
        }
        else{
            fprintf(stderr, "\033[91;1mFailed\033[0m: Passwords do not match\n");
            return -1;
        }
        bool set=false;
        while(sha.codes[counter]!=NULL){
            if(sha.users[counter]==sToi(argv[1])){
                free(sha.codes[counter]);
                sha.codes[counter]=(char*)calloc(strlen(code_hex)+1, sizeof(char));
                strcpy(sha.codes[counter], code_hex);
                set=true;
                break;
            }
            counter++;
        }
        if(!set){
            sha.users=(int*)realloc(sha.users, (++sha.userl+1)*sizeof(int));
            sha.users[sha.userl-2>=0?sha.userl-2:0]=sToi(argv[1]);
            sha.users[sha.userl-1>=0?sha.userl-1:0]=-1;
            sha.codes=(char**)realloc(sha.codes, (++sha.codel+1)*sizeof(char*));
            sha.codes[sha.codel-2>=0?sha.codel-2:0]=(char*)calloc(strlen(code_hex)+1, sizeof(char));
            strcpy(sha.codes[sha.codel-2>=0?sha.codel-2:0], code_hex);
            sha.codes[sha.codel-1>=0?sha.codel-1:0]=NULL;
        }
        free(code_hex);
        writeSha(sha, SHADOWPATH);
        destroySha(sha);
        fprintf(stdout, "Success\n");
    }
    else{
        fprintf(stderr, "\033[91;1mSecurity Section - Fatal Error\033[0m: Cannot access %s\n", SHADOWPATH);
        return -1;
    }
    return 0;
}

double del_user(int argc, char* argv[]){
    if(argc<2){
        fprintf(stderr, "\033[91;1mError\033[0m: require an argument\n");
        return -1;
    }
    if(access(SHADOWPATH, F_OK)==0){
        struct stat file;
        stat(SHADOWPATH, &file);
        uid_t owner=file.st_uid;
        if(owner!=getuid()){
            fprintf(stderr, "\033[91;1mSecurity Alert\033[0m: you arenot the owner of %s, please contact the installer of LMFSWS for help\n", SHADOWPATH);
            return -1;
        }
        if(!(0<=sToi(argv[1])&&sToi(argv[1])<=65535)){
            fprintf(stderr, "\033[91;1mFailed\033[0m: Illegal uid\n");
            return -1;
        }
        uid_t uid=sToi(argv[1]);
        shadow_t sha=parseSha(SHADOWPATH);
        bool deleted=false;
        shadow_t new;
        new.codes=(char**)calloc(1, sizeof(char*));
        new.users=(int*)calloc(1, sizeof(int));
        new.codel=0;
        new.userl=0;
        for(int i=0;i<sha.codel;i++){
            if(sha.users[i]==uid){
                deleted=true;
                continue;
            }
            new.codes=(char**)realloc(new.codes, (++new.codel+1)*sizeof(char*));
            if(sha.codes[i]!=NULL){
                new.codes[i]=(char*)calloc(strlen(sha.codes[i])+1, sizeof(char));
                strcpy(new.codes[i], sha.codes[i]);
            }
            else
              new.codes[i]=NULL;
            new.users=(int*)realloc(new.users, (++new.userl+1)*sizeof(int));
            new.users[i]=sha.users[i];
        }
        if(!deleted){
            fprintf(stderr, "\033[91;1mError\033[0m: uid %d isnot in shadow\n", uid);
        }
        destroySha(sha);
        writeSha(new, SHADOWPATH);
        destroySha(new);
        fprintf(stdout, "Success\n");
    }
    else{
        fprintf(stderr, "\033[91;1mSecurity Section - Fatal Error\033[0m: Cannot access %s\n", SHADOWPATH);
        return -1;
    }
    return 0;
}
FuncList Regist[]={
      {"chpasswd", 1, change_passwd, NULL},
      {"deluser", 1, del_user, NULL},
      {NULL, -1, NULL, NULL}
};

