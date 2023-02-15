# LMFS WorkStation

## Introduction

LMFS WorkStation (hereinafter referred to as LMFSWS) is an easy-to-use, small size, easy to expand a tool set of procedures, you can easily compile your program into an LMFSWS extension module to facilitate the use of LMFSWS.

## How to build it from source code?
```
$ git clone https://github.com/Lone-Air/LMFSWS
$ cd LMFSWS
$ sudo ./build.sh
```
## How to build a external module?

1. Required function: mod_init
2. How to register functions to LMFSWS:
```
#include <LMFSWS/command-register.h>
#include <LMFSWS/module-loader.h>
#include <LMFSWS/cli.h>
#include <stdlib.h>

resultpShell* rpS;

int mod_init(ModuleList* ModL_m, FuncListArr* FuncL_m, resultpShell* rpS_m){
    ModL=ModL_m;
    FuncL=FuncL_m;
    rpS=rpS_m;
    /* Do something else... */
    return 0; // If it successfully
}

double Fun1(){ return 1+1; }
char* Fun2(){ return "abc"; }

double NF(){ return -1; } // Empty Function

FuncList Regist[]={
    {
        "Fun1", // Name of command in LMFSWS
        1, // Type of command (1: normal(returns double), 2: pointer)
        Fun1, // If type of command is 1, it will be filled of Function name
        NULL, 
    },
    {
        "Fun2",
        2,
        NF, // Empty Function to fill the Normal Function space
        (void*)Fun2 // Pointer Function (force to convert the type to void*!)
    },
    {
        NULL, // End of Register List. You should follow the style
        -1,
        NF,
        NULL
    }
};
```
3. Set a XXX.mode file (XXX is the module name)
    Content(Example):
        LAZY
    Write "LAZY" to tell LMFSWS module-loader use RTLD_LAZY Mode in LIBDL
4. In the end. Compile XXX.c to XXX.so and put XXX.mode to the same as directory of XXX.so
   Start "LMFSWS" and
```
include XXX // This is the name of your module
regist XXX // Call Register to regist the function in Regist of XXX
execext <Function> // The Function of XXX was loaded
```

## Thanks for using!
