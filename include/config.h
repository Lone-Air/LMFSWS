/*
 * LMFS WorkStation - Configuration
 *
 * LICENSE SEE $ROOT/LICENSE
 */

#ifndef LMFSWS_CFG_H
#define LMFSWS_CFG_H

#define LIBNAME "/lib/"
#define BINNAME "/bin/"

#ifdef ENABLE_LOGIN
  #define ETCNAME PREFIX"/etc/lmfsws.d/"
  #define SHADOWPATH ETCNAME"/shadow"
#endif

#define MODNAME LIBNAME"/LMFSWSModules/"

#define MODULE PREFIX MODNAME

#endif

