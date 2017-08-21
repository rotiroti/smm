/*------------------------------------------------------------------------
*
* smm - simple memory manager
* Progetto AA0809 
* Laboratorio Sistemi Operativi I
* Include file for test software: 
*  common defines
*
* Creation date: 04/01/2009
* Authors: M.Verola
*
* ------------------------------------------------------------------------
*
* CVS INFO
*
* $Id:$
*
* ------------------------------------------------------------------------
*/
#ifndef SMM_DEF_H
#define SMM_DEF_H

//----------------------------------------------------------
// Macros for trace log print
//----------------------------------------------------------
#ifdef SMM_VERBOSE
#define PRINT_VERBOSE(...) printf(__VA_ARGS__)
#else
#define PRINT_VERBOSE(...)
#endif // SMM_VERBOSE

//----------------------------------------------------------
// Symbolic names for useful quantities
//----------------------------------------------------------
#define DIM_0B          0
#define DIM_1B          1
#define DIM_16B        16
#define DIM_1KB      1024
#define DIM_2KB      2048
#define DIM_4KB      4096
#define DIM_8KB      8192
#define DIM_1MB   1048576
#define DIM_2MB   2097152
#define DIM_4MB   4194304
#define DIM_5MB   5242880
#define DIM_8MB   8388608
#define DIM_16MB 16777216

#endif // ! SMM_DEF_H                
/* ___oOo___ */
