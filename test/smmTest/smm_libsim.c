/*------------------------------------------------------------------------
*
* smm - simple memory manager
* Progetto AA0809 
* Laboratorio Sistemi Operativi I
* Source file for test software: 
*  wrapper functions based on Linux syscalls to simulate libsmm.a
*
* WARNING: this file is compiled and the obj module converted into
*  a static library named libsmmsim.a ONLY if the Makefile of this
*  test package is executed in simulating mode (MODE =   # empty).
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
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "smm_def.h"
#include "smm_lib.h"
#include "smm_libsim.h"

void *smm_malloc(size_t size) {
  void *p;
  int rc; 

  if (size<=0 || size>FreeMem()) {
    PRINT_VERBOSE("Memory under/overflow: attempt to allocate %d bytes, while there is space for %d bytes\n",size,FreeMem()); 
    return(NULL);
  }

  p=malloc(size);
  if (p) { // If Linux malloc is OK, perform smmd simulation inserting
           //  the new block in the linked list
    rc=AddBlk(TestPID,p,size);
    if (rc!=0) {
      free(p);
      p=NULL;
    }
  }

  return(p);
}

void *smm_calloc(size_t nmemb, size_t size) {
  void *p;
  int rc; 

  if (nmemb<=0 || size<=0 || nmemb*size>FreeMem()) {
    PRINT_VERBOSE("Memory under/overflow: attempt to allocate %d bytes, while there is space for %d bytes\n",nmemb*size,FreeMem()); 
    return(NULL);
  }

  p=calloc(nmemb,size);
  if (p) { // If Linux calloc is OK, perform smmd simulation inserting
           //  the new block in the linked list
    rc=AddBlk(TestPID,p,nmemb*size);
    if (rc!=0) {
      free(p);
      p=NULL;
    }
  }

  return(p);
}

void *smm_realloc(void *ptr, size_t size) {
  BlkInfo *p;
  void *pnew;
  int rc1,rc2;

  // PRINT_VERBOSE("Finding block [pid=%d,ptr=%p] ...",TestPID,ptr); 
  p = FindBlk(TestPID,ptr);
  if (!p) {
    // PRINT_VERBOSE("block NOT found!\n");   
    return(NULL);
  } 
  // else {
  //  PRINT_VERBOSE("block found!\n");
  //}

  if (size<=0 || size>(FreeMem()+((int)p->last - (int)p->start + 1))) {
    PRINT_VERBOSE("Memory under/overflow: attempt to reallocate %d bytes, while there is space for %d bytes\n",
                  size,FreeMem()+((int)p->last - (int)p->start+1)); 
    return(NULL);
  }

  pnew=realloc(ptr,size);
  if (pnew) { // If Linux realloc is OK, perform smmd simulation deleting the
              // old block and inserting the reallocated block in the linked list
    rc1=DelBlk(TestPID,ptr);
    rc2=AddBlk(TestPID,pnew,size);
    if (rc1<0 || rc2<0) {
      free(pnew);
      pnew=NULL;
    }
  } 

  return(pnew);
}

int smm_free(void *ptr) {
  int rc;

  rc=DelBlk(TestPID,ptr);
  if (rc<0)
    return(-1);

  free(ptr);

  return(0);
}


int smm_put(void *buf, void *ptr, size_t size) { 
  BlkInfo *p;

  // PRINT_VERBOSE("Finding block [pid=%d,ptr=%p] ...",TestPID,ptr); 
  p = FindBlk(TestPID,ptr);
  if (!p) {
    // PRINT_VERBOSE("block NOT found!\n");   
    return(-1);
  } 
  // else {
  //  PRINT_VERBOSE("block found!\n");
  // }

  if (size<=0 || (int)size>((int)p->last - (int)ptr + 1)) {
    PRINT_VERBOSE("Buffer under/overflow: attempt to write %d bytes, while there is space for %d bytes\n",
                   size,(int)p->last - (int)ptr + 1); 
    return(-1);
  }

  memcpy(ptr,buf,size);

  return(0);
}

int smm_get(void *ptr, void *buf, size_t size) {
  BlkInfo *p;

  // PRINT_VERBOSE("Finding block [pid=%d,ptr=%p] ...",TestPID,ptr); 
  p = FindBlk(TestPID,ptr);
  if (!p) {
    // PRINT_VERBOSE("block NOT found!\n");   
    return(-1);
  } 
  // else {
  //  PRINT_VERBOSE("block found!\n");
  // }

  if (size<=0 || (int)size>((int)p->last-(int)ptr+1)) {
    PRINT_VERBOSE("Buffer under/overflow: attempt to write %d bytes, while there is space for %d bytes\n",
                   size,(int)p->last-(int)ptr+1); 
    return(-1);
  }

  memcpy(buf,ptr,size);
 
  return(0);
}

int smm_chk_ptr(void *ptr, struct smm_blk *blk) { 
  BlkInfo *p=NULL; 

  PRINT_VERBOSE("Finding block [pid=%d,ptr=%p] ...",TestPID,ptr); 
  p = FindBlk(TestPID,ptr);
  if (!p) {
    PRINT_VERBOSE("block NOT found!\n");   
    return(-1);
  } else {
    PRINT_VERBOSE("block found!\n");
    blk->start=p->start;
    blk->last=p->last;
    blk->swap=p->swap;
  }

  return(0);
}

//-------------------------------------------------
// AddBlk()
// Add the allocated block in the global list to simulate smmd behaviour
//-------------------------------------------------
static int AddBlk(pid_t pid, void *start, size_t size)
{
  BlkInfo *p;
  void *last=(void *)((int)start+size-1);

  // PRINT_VERBOSE("Adding block [pid=%d,start=%p,last=%p] ...\n",pid,start,last); 

  p = (BlkInfo *)malloc(sizeof(BlkInfo));
  if (!p) {
    PRINT_VERBOSE("AddBlk: malloc failed!\n");
    return(-1);
  }  

  // Load info in the new block info item
  p->pid=pid, p->start=start, p->last=last; p->swap=0, p->next=p->prev=NULL;
 
  // Add the new block on the head of the list
  if (BlkListHead) { // there is already at least 1 item in the list
    p->next=BlkListHead;
    BlkListHead->prev=p;
  }

  // This block becomes head of list
  BlkListHead=p;
 
  return(0);
}

//-------------------------------------------------
// DelBlk()
// Delete the allocated block in the global list to simulate smmd behaviour
//-------------------------------------------------
static int DelBlk(pid_t pid, void *start)
{
  BlkInfo *p;

  // PRINT_VERBOSE("Deleting block [pid=%d,start=%p] ...\n",pid,start); 

  p = FindBlk(pid,start);
  if (!p) {
    PRINT_VERBOSE("DelBlk: FindBlk failed, block not found!\n");
    return(-1);
  }  

  // If there is a next block, re-assign its prev pointer
  if (p->next)
    p->next->prev=p->prev;

  // If there is a prev block, re-assign its next pointer
  if (p->prev)
    p->prev->next=p->next;

  if (p->prev==NULL) // block found is head of list
    BlkListHead=p->next;
     
  free(p);

  return(0);
}

//-------------------------------------------------
// FindBlk()
// Find the allocated block in the global list to simulate smmd behaviour
//-------------------------------------------------
static BlkInfo *FindBlk(pid_t pid, void *ptr)
{
  BlkInfo *p=BlkListHead;

  // PRINT_VERBOSE("Finding block [pid=%d,ptr=%p] ...",pid,ptr); 

  while (p) {
    // PRINT_VERBOSE("Comparing block [pid=%d,ptr=%p] with block [pid=%d,start=%p,last=%p] ...\n",pid,ptr,p->pid,p->start,p->last); 
    if (p->pid==pid && ptr>=p->start && ptr<=p->last)
      break;
    p=p->next;
  }

/*  if (p)
    PRINT_VERBOSE("block found!\n");
  else
    PRINT_VERBOSE("block NOT found!\n");
*/

  return(p); // If block not found, p is NULL
}
            
//-------------------------------------------------
// FreeMem()
// Compute free memory size in smmMem
//-------------------------------------------------
static size_t FreeMem()
{
  int freeMem=DIM_16MB;
  size_t size;
  BlkInfo *p=BlkListHead;
  
  // PRINT_VERBOSE("Computing available memory in smmMem ..."); 

  while (p) {
    size=(int)p->last-(int)p->start+1;
    freeMem-=size;
    p=p->next;
  }

  // PRINT_VERBOSE("free mem is %d B!\n",freeMem);

  return(freeMem);
}
             
   
/* ___oOo___ */
