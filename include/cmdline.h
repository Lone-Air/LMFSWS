/*
 * LMFS WorkStation - COMMAND LINE INPUT LIBRARY - HEADER FILE
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#ifndef LMFSWS_CMDLINE_H
#define LMFSWS_CMDLINE_H

#include <stdio.h>

typedef struct Array{
    char** data;
    size_t length;
}Array;

extern Array* parse(const char* s);

#endif

