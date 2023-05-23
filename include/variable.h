/*
 * LMFS WorkStation - Variable Controller - HEADER FILE
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#ifndef _LMFSWS_VARIABLE_H
#define _LMFSWS_VARIABLE_H

#include "lmfstypes.h"

extern struct varpool* default_varpool;
extern ArgList emptylist;

#define vpool default_varpool->var_pool
#define vlen default_varpool->varpool_len

extern int InitVarPool();
extern void newVar(char*, var_type, var_data);

extern int search_var(char*);
extern int update_var(char*, var_type, var_data);
extern int delete_var(char*);

extern void destroy_var(int);
extern int CleanVariables();

#endif
