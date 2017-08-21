/*----------------------------------------------------------
*
* smm - simple memory manager
* Progetto AA0809 
* Laboratorio Sistemi Operativi I
* Include file for functions simulating libsmm.a when test
*   software is running in simulating mode.
*
* Creation date: 04/01/2009
* Authors: M.Verola
*
* ----------------------------------------------------------
*
* CVS INFO
*
* $Id:$
*
* ----------------------------------------------------------
*/
#ifndef SMM_LIBSIM_H
#define SMM_LIBSIM_H

// typedefs
typedef struct _BlkInfo {
  struct _BlkInfo *next;
  struct _BlkInfo *prev;
  pid_t pid;
  void *start;
  void *last;
  char  swap;
} BlkInfo; 

// Protos
static int AddBlk(pid_t pid, void *start, size_t size);
static int DelBlk(pid_t pid, void *start);
static BlkInfo *FindBlk(pid_t pid, void *start);
static size_t FreeMem(void);

// Static variables
static BlkInfo *BlkListHead=NULL; // Head of the allocated block linked list

// Extern variables
extern short TestPID; // PID of this process
extern void *TestPtr; // Pointer returned from the last called test proc

#endif // SMM_LIBSIM_H
