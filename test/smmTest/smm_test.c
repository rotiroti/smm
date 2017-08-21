/*------------------------------------------------------------------------
*
* smm - simple memory manager
* Progetto AA0809 
* Laboratorio Sistemi Operativi I
* Include file for test software.
*
* Creation date: 02/01/2009
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

// User prog includes
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <wait.h>

//----------------------------------------------------------
// Includes
//----------------------------------------------------------
#include "smm_def.h"
#include "smm_lib.h"
#include "smm_test.h"

//=========================================================================
//
// Main program
//
//=========================================================================
int main(int argc, char **argv) {

  int i;

  printf("\n*** Start test program for SMM Simple Memory Manager\n");

  // Get process PID
  TestPID = getpid();

  // Init write buffers
  TestInitWBufs();

  if (argc<=1) { // no args on command line 
    printf("--- Executing all the test cases\n");
    TestProcedure(NULL);
  }
  else {
    for (i=1;i<argc;i++) { 
      printf("--- [%i] Executing the test cases identified by the pattern <%s>\n\n",i,argv[i]);
      TestProcedure(argv[i]);
    }
  }

  printf("\n*** Test program for SMM Simple Memory Manager completed!\n\n");

  exit(0);

}

//=========================================================================
//
// Support functions
//
//=========================================================================

//-------------------------------------------------
// TestInitWBufs()
//-------------------------------------------------
static void TestInitWBufs(void)
{
  int i;  

  /*
  PRINT_VERBOSE("TestBufW=%p\n",TestBufW); 
  PRINT_VERBOSE("TestBufR=%p\n",TestBufR);  
  PRINT_VERBOSE("TestBufZ=%p\n",TestBufZ);  
  PRINT_VERBOSE("TestBufW1=%p\n",TestBufW1);  
  PRINT_VERBOSE("TestBufW2=%p\n",TestBufW2);   
  */

  for (i=0;i<DIM_16MB;i++) {
    // Init write buffer with the repeated sequence 0x00, 0x01, ... 0xFF
    TestBufW[i]=(unsigned char)i;
    // Init read buffer so that no single byte is equal to corresponding byte in write buffer
    TestBufR[i]=~((unsigned char)i); 
    //PRINT_VERBOSE("TestBufW[%d]=0x%02x - TestBufR[%d] 0x%02x\n",i,TestBufW[i],i,TestBufR[i]);  
  }

  for (i=0;i<DIM_4KB;i++) {
    // Init write buffer1 with the repeated sequence 0x00, 0x01, ... 0x7F
    TestBufW1[i]=0x7F & i;
    // Init write buffer1 with the repeated sequence 0x80, 0x81, ... 0xFF
    TestBufW2[i]=0x80 | TestBufW1[i];
    //PRINT_VERBOSE("TestBufW1[%d]=0x%02x - TestBufW2[%d]=0x%02x\n",
    //   i,TestBufW1[i],i,TestBufW2[i]);  
  }

  return;
}

//-------------------------------------------------
// TestSleep()
// Sleep for 'delay' millisecs
//-------------------------------------------------
static int TestSleep(int delay, void *arg)
{
  struct timespec t;

  // PRINT_VERBOSE("sleeping %i msec ...\n",delay);  
  if (delay>0) {
    t.tv_sec = delay / 1000;
    delay %= 1000;
    t.tv_nsec = delay*1000000; // convert millisecs into nanosecs
    if (nanosleep(&t,NULL)<0)
      fprintf(stderr,"ERROR: nanosleep() exited due to a signal! errno=%d -> %s\n",
	      errno,strerror(errno));
  }    

  return(0);
}

//=========================================================================
//
// Test procedures
//
//=========================================================================

//-------------------------------------------------
// TestMalloc()
//-------------------------------------------------
static int TestMalloc(int action, TestArg *arg)
{
  int ret=SMM_OK;
  size_t size=arg->mallocArg.size;
  int idx=arg->mallocArg.poutIdx;

  PRINT_VERBOSE("Allocate %d bytes\n",size);

  TestPtr[idx] = arg->mallocArg.pout = smm_malloc(size);

  PRINT_VERBOSE("smm_malloc(%d) --> ptr ret=%p\n",size,TestPtr[idx]);

  if (TestPtr[idx] == NULL)
    ret=SMM_ERR;

  return(ret);
}

//-------------------------------------------------
// TestCalloc()
//-------------------------------------------------
static int TestCalloc(int action, TestArg *arg)
{
  int ret=SMM_OK;
  size_t nmemb=arg->callocArg.nmemb;
  size_t size=arg->callocArg.size;
  int idx=arg->callocArg.poutIdx;

  PRINT_VERBOSE("Allocate and zero %d members of %d bytes\n",nmemb,size);

  TestPtr[idx] = arg->callocArg.pout = smm_calloc(nmemb,size);

  PRINT_VERBOSE("smm_calloc(%d,%d) --> ptr ret=%p\n",nmemb,size,TestPtr[idx]);

  if (TestPtr[idx] == NULL)
    ret=SMM_ERR;

  return(ret);
}


//-------------------------------------------------
// TestRealloc()
//-------------------------------------------------
static int TestRealloc(int action, TestArg *arg)
{
  int ret=SMM_OK;
  void *ptr;
  size_t size=arg->reallocArg.size;
  int ptrOffsetStart=arg->chkPtrArg.ptrOffsetStart;
  int inIdx=arg->reallocArg.pinIdx;
  int outIdx=arg->reallocArg.poutIdx;

  if (inIdx==-1)    
    ptr=arg->reallocArg.pin;
  else
    ptr=(void *)((int)TestPtr[inIdx]+ptrOffsetStart);

  PRINT_VERBOSE("Re-allocate block pointed by %p to %d bytes\n",ptr,size);

  TestPtr[outIdx] = arg->reallocArg.pout = smm_realloc(ptr,size);

  PRINT_VERBOSE("smm_realloc(%p,%d) --> ptr ret=%p\n",ptr,size,TestPtr[outIdx]);

  if (TestPtr[outIdx] == NULL)
    ret=SMM_ERR;

  return(ret);
}


//-------------------------------------------------
// TestFree()
//-------------------------------------------------
static int TestFree(int action, TestArg *arg)
{
  int ret=SMM_OK;
  void *ptr;
  int idx=arg->freeArg.pinIdx;

  if (idx==-1)    
    ptr=arg->freeArg.pin;
  else
    ptr=TestPtr[idx];

  PRINT_VERBOSE("Free %p pointer\n",ptr);

  TestRc = arg->freeArg.rc = smm_free(ptr);

  PRINT_VERBOSE("smm_free(%p) --> code ret=%d\n",ptr,TestRc);

  if (TestRc < 0)
    ret=SMM_ERR;

  return(ret);
}


//-------------------------------------------------
// TestChkPtr()
//-------------------------------------------------
static int TestChkPtr(int action, TestArg *arg)
{
  int ret=SMM_OK;
  void *ptr;
  struct_smm_blk *blk=arg->chkPtrArg.blk;
  int ptrOffsetStart=arg->chkPtrArg.ptrOffsetStart;
  int blkExpSize=arg->chkPtrArg.blkExpSize;
  int idx=arg->chkPtrArg.pinIdx;
  void *ptrExpStart, *ptrExpLast;

  if (idx==-1)    
    ptr=arg->chkPtrArg.pin;
  else
    ptr=(void *)((int)TestPtr[idx]+ptrOffsetStart);

  PRINT_VERBOSE("Check %p pointer (block expected size is %d)\n",ptr,blkExpSize);

  TestRc = arg->chkPtrArg.rc = smm_chk_ptr(ptr,blk);

  PRINT_VERBOSE("Retrieved block size is %d\n",(int)blk->last - (int)blk->start+1);
  PRINT_VERBOSE("smm_chk_ptr(%p,%p) --> code ret=%d\n",ptr,blk,TestRc);

  if (TestRc < 0)
    ret=SMM_ERR;
  else {
    ptrExpStart = (void *)((int)ptr-ptrOffsetStart);
    ptrExpLast  = (void *)((int)ptrExpStart+blkExpSize-1);
    if (blk->start != ptrExpStart || blk->last != ptrExpLast) {
      PRINT_VERBOSE("ERROR -> returned block pointers [start=%p,last=%p] are different wrt expected [start=%p,last=%p]\n\n",
      blk->start,blk->last,ptrExpStart,ptrExpLast);
      ret=SMM_ERR;
    }
  }

  return(ret);
}

//-------------------------------------------------
// TestPut()
//-------------------------------------------------
static int TestPut(int action, TestArg *arg)
{
  int ret=SMM_OK;
  void *buf=arg->putArg.buf;
  void *ptr;
  size_t size=arg->putArg.size;
  int ptrOffsetStart=arg->putArg.ptrOffsetStart;
  int idx=arg->putArg.pinIdx;

  if (idx==-1)    
    ptr=arg->putArg.pin;
  else
    ptr=(void *)((int)TestPtr[idx]+ptrOffsetStart);

  PRINT_VERBOSE("Copy %d bytes from local buffer %p to pointer %p\n",
	         size,buf,ptr);

  /* PRINT_VERBOSE("ptr=[0x%02x ... 0x%02x]\n",
                 ((unsigned char *)ptr)[0],((unsigned char *)ptr)[size-1]);
  */
  TestRc = arg->putArg.rc = smm_put(buf,ptr,size);

  /* PRINT_VERBOSE("ptr=[0x%02x ... 0x%02x]\n",
                 ((unsigned char *)ptr)[0],((unsigned char *)ptr)[size-1]);
  */
  PRINT_VERBOSE("smm_put(%p,%p,%d) --> code ret=%d\n",buf,ptr,size,TestRc);

  if (TestRc < 0)
    ret=SMM_ERR;

  return(ret);
}

//-------------------------------------------------
// TestGet()
//-------------------------------------------------
static int TestGet(int action, TestArg *arg)
{
  int ret=SMM_OK;
  void *ptr;
  void *buf=arg->getArg.buf;
  size_t size=arg->getArg.size;
  int ptrOffsetStart=arg->getArg.ptrOffsetStart;
  int idx=arg->getArg.pinIdx;

  if (idx==-1)    
    ptr=arg->getArg.pin;
  else
    ptr=(void *)((int)TestPtr[idx]+ptrOffsetStart);

  PRINT_VERBOSE("Copy %d bytes from pointer %p to local buffer %p\n",
	         size,ptr,buf);

  /* PRINT_VERBOSE("buf=[0x%02x ... 0x%02x]\n",
                 ((unsigned char *)buf)[0],((unsigned char *)buf)[size-1]);
  */
  TestRc = arg->getArg.rc = smm_get(ptr,buf,size);

  /* PRINT_VERBOSE("buf=[0x%02x ... 0x%02x]\n",
                 ((unsigned char *)buf)[0],((unsigned char *)buf)[size-1]);
  */
  PRINT_VERBOSE("smm_get(%p,%p,%d) --> code ret=%d\n",ptr,buf,size,TestRc);

  if (TestRc < 0)
    ret=SMM_ERR;

  return(ret);
}

//-------------------------------------------------
// TestCmpBuf()
//-------------------------------------------------
static int TestCmpBuf(int action, TestArg *arg)
{
  int ret=SMM_OK;
  void *buf1=arg->cmpBufArg.buf1;
  void *buf2=arg->cmpBufArg.buf2;
  size_t size=arg->cmpBufArg.size;
  int i;

/*  PRINT_VERBOSE("Compare %d bytes between buffer %p=[0x%02x ... 0x%02x] and buffer %p=[0x%02x ... 0x%02x]\n",
	         size,buf1,
                 ((unsigned char *)buf1)[0],((unsigned char *)buf1)[size-1],
                 buf2,
                 ((unsigned char *)buf2)[0],((unsigned char *)buf2)[size-1]);
*/

  PRINT_VERBOSE("Compare %d bytes between buffer %p and buffer %p\n",
	         size,buf1,buf2);

  TestRc = arg->cmpBufArg.rc = memcmp(buf1,buf2,size);

  PRINT_VERBOSE("--> code ret=%d\n",TestRc);

  // Reset the bytes just copied in the read buffer
  for (i=0;i<(int)size;i++) {
    // Init read buffer so that no single byte is equal to corresponding byte in write buffer
    TestBufR[i]=~((unsigned char)i); 
  }

  if (TestRc != 0)
    ret=SMM_ERR;

  return(ret);
}

//-------------------------------------------------
// TestPerformance()
// action=0 -> performance of smm_malloc+smm_free
// action=1 -> performance of smm_put
// action=2 -> performance of smm_get
//-------------------------------------------------
static int TestPerformance(int action, TestArg *arg)
{
  int ret=SMM_OK;
  int i, j;
  size_t size=1; /* block dim, from 1B up to a 16MB */
  int N=24;      /* number of measurements */
  int NITER=1000;  /* number of meas. samples for each block dim */
  long elapsed;  /* time measurements in msec */
  struct timeval ts, te;
  float meanTime;
  void *ptr=NULL;
  const char *funcName[]={"smm_malloc","smm_put","smm_get"};

  PRINT_VERBOSE("Timing %s() performance\n",funcName[action]);

  if (action>=1) { // do it only for smm_put and smm_get
    ptr=smm_malloc(DIM_16MB);
    if (ptr == NULL) { 
      PRINT_VERBOSE("ERROR when generating precondition for this test: smm_malloc(%d)\n",DIM_16MB);
      return(SMM_ERR);
    }

    if (action==2) { // do it only for smm_get
      ret=smm_put(TestBufW,ptr,DIM_16MB);
      if (ret<0) {
        PRINT_VERBOSE("ERROR when generating precondition for this test: smm_put(%p,%p,%d)\n",
                TestBufW,ptr,DIM_16MB);
        smm_free(ptr);
        return(SMM_ERR);
      }
    }
  }
      
  for (i=0;i<=N;i++) {

    ret = gettimeofday(&ts, NULL);
    if (ret<0) {
      fprintf(stderr,"ERROR when acquiring start time (gettimeofday) for this test\n");
      smm_free(ptr);
      return(SMM_ERR);
    }

    //PRINT_VERBOSE("Started time measurement ...ts.[tv_sec=%ld,tv_usec=%ld]\n",
    //                 ts.tv_sec,ts.tv_usec);

    switch (action) {
    case 0:
      for (j=0;j<NITER;j++) {
        ptr=smm_malloc(size);
        if (ptr) 
          smm_free(ptr);
        else 
          return(SMM_ERR);
      }
      break;

    case 1:
      for (j=0;j<NITER/10;j++) {
        ret=smm_put(TestBufW,ptr,size);
        if (ret<0) {
          smm_free(ptr);
          return(SMM_ERR);
        } 
      }
      break;

    case 2:
      for (j=0;j<NITER/10;j++) {
        ret=smm_get(ptr,TestBufR,size);
        if (ret<0) {
          smm_free(ptr);
          return(SMM_ERR);
        } 
      }
      break;

    default:
      fprintf(stderr,"CRITICAL SOFTWARE ERROR: bad action number=%d\n",action);
      exit(-1);
    }
      
    ret = gettimeofday(&te, NULL);
    if (ret<0) {
      fprintf(stderr,"ERROR when acquiring stop time (gettimeofday) for this test\n");
      smm_free(ptr);
      return(SMM_ERR);
    }

    // PRINT_VERBOSE("Stopped time measurement ...te.[tv_sec=%ld,tv_usec=%ld]\n",
    //                 te.tv_sec,te.tv_usec);

    elapsed  = (te.tv_sec  - ts.tv_sec)*1000000;
    elapsed += (te.tv_usec - ts.tv_usec);

    //PRINT_VERBOSE("elapsed %ld, NITER %d\n",elapsed,NITER);

    meanTime=(float)elapsed/NITER;

    PRINT_VERBOSE("%s(%8d) avg time (usec) = %10.3f\n",funcName[action],size,meanTime);

    size*=2;
  } // end for (i=0;i<=N;i++)

  if (action>=1) { // do it only for smm_put and smm_get
      smm_free(ptr);
  }

  PRINT_VERBOSE("--> code ret=%d\n",TestRc);

  return(ret);

}

//-------------------------------------------------
// TestProtection()
// action=0 -> protect wrt smm_free
// action=1 -> protect wrt smm_realloc
// action=2 -> protect wrt smm_put
// action=3 -> protect wrt smm_get
// action=4 -> protect wrt smm_chkptr
//-------------------------------------------------
static int TestProtection(int action, TestArg *arg)
{
  int ret=SMM_OK;
  pid_t pid;
  void *ptr,*ptr1;
  int status;
  const char *funcName[]={"smm_free","smm_realloc","smm_put","smm_get","smm_chk_ptr"};
  size_t size=DIM_1KB, size1=DIM_2KB;

  PRINT_VERBOSE("Checking memory protection against %s() function\n",funcName[action]);

  ptr=smm_malloc(size);
  PRINT_VERBOSE("PID=%d: smm_malloc(%d) --> ptr ret=%p\n",TestPID,size,ptr);  
  if (ptr == NULL) { 
    PRINT_VERBOSE("ERROR when generating precondition for this test: smm_malloc(%d)\n",size);
    return(SMM_ERR);
  }

  // Avoid buffering problems
  fflush(NULL);

  // === start code fork+wait

  pid=fork(); // creating child process, which has a different PID
  if (pid == -1) {
    fprintf(stderr,"ERROR when calling fork()\n");
    smm_free(ptr);
    return(SMM_ERR); // test case cannot continue
  }
  else if (pid==0) { // we are here in the child process

    // update TestPID with the PID of the child process
    TestPID = getpid();

    switch (action) {    
    case 0:
      ret=smm_free(ptr);
      PRINT_VERBOSE("PID=%d: smm_free(%p) --> code ret=%d\n",TestPID,ptr,ret);

      if (ret<0) exit(1); // error in trying to free the pointer, return 1 to parent process
      else exit(0); // no error
      break;  

    case 1:
      ptr1=smm_realloc(ptr,size1);
      PRINT_VERBOSE("PID=%d: smm_realloc(%p,%d) --> ptr ret=%p\n",TestPID,ptr,size1,ptr1);

      if (ptr1==NULL)  exit(1); // error in trying to reallocate the pointer, return 1 to parent process
      else exit(0); // no error
      break;  

    case 2:
      ret=smm_put(TestBufW,ptr,size);
      PRINT_VERBOSE("PID=%d: smm_put(%p,%p,%d) --> code ret=%d\n",TestPID,TestBufW,ptr,size,ret);

      if (ret<0) exit(1); // error in trying to free the pointer, return 1 to parent process
      else exit(0); // no error
     break;  

    case 3:
      ret=smm_get(ptr,TestBufR,size);
      PRINT_VERBOSE("PID=%d: smm_get(%p,%p,%d) --> code ret=%d\n",TestPID,ptr,TestBufR,size,ret);

      if (ret<0) exit(1); // error in trying to free the pointer, return 1 to parent process
      else exit(0); // no error
     break;  

    case 4:
      ret=smm_chk_ptr(ptr,&TestBlk);
      PRINT_VERBOSE("PID=%d: smm_chk_ptr(%p,%p) --> code ret=%d\n",TestPID,ptr,&TestBlk,ret);

      if (ret<0) exit(1); // error in trying to free the pointer, return 1 to parent process
      else exit(0); // no error
     break;  

    default:
      fprintf(stderr,"CRITICAL SOFTWARE ERROR: bad action number=%d\n",action);
      exit(2);
    }
  }

  else { // we are in the parent process
    pid=wait(&status);  
    if (WIFEXITED(status)) {
      PRINT_VERBOSE("Child process (PID %d) terminated normally with exit status=%d!\n",
	     pid,WEXITSTATUS(status));
      if (WEXITSTATUS(status)!=1)
        ret=SMM_ERR;
    }
    else {
      PRINT_VERBOSE("Child process (PID %d) terminated ABNORMALLY!\n",pid);
      ret=SMM_ERR;
    }

  }
  // === end code fork+wait

  // Parent process has to free the block
  smm_free(ptr);

  return(ret);
}

//-------------------------------------------------
// Main function to execute test-steps
//-------------------------------------------------
static int TestProcedure(const char *ident)
{
  int nFailed  = 0; // num of test cases executed and failed
  int nSkipped = 0; // num of test cases skipped
  int nTests   = sizeof(TestList) / sizeof(TestList[0]) - 1; // total num of tests defined
  int iTest; // current test index
  TestArg currArg; // current argument struct (might be input or output arg)
  TestArg lastArgIn, lastArgOut; // last input/output argument struct
  TestCase *p; // pointer to current test case
  int actualRet; // return code of the current test function
  
  // main loop, scanning the test list
  for (iTest = 0; iTest < nTests; iTest++) {
 
    // Get the pointer to the current test case record
    p = &TestList[iTest];
 
    if (ident!=NULL)
       if (strncmp(ident, p->ident, strlen(ident)) != 0) {
         //----------------------------------------------------------------------
         // Current test case identifier does not match with requested pattern
         //----------------------------------------------------------------------
         nSkipped++;
         continue;  // Test not requested, skip it
       }

    printf("TEST CASE %-40s", p->ident);
    if (p->function == (TestFunc)XX) {
      //----------------------------------------------------------------------
      // Function of current test case is NULL
      //----------------------------------------------------------------------
      nSkipped++;
      printf("- WARNING Test function is NULL: test skipped!");
      continue;	// No test-function, skip it
    }

    // Setup the argument value
    if (p->argIn == (TestArg *)XX) 
      //----------------------------------------------------------------------
      // Input argument is DON'T CARE
      //----------------------------------------------------------------------
      memset(&currArg, 0, sizeof(TestArg)); // Clean up input arg struct

    else if (p->argIn == (TestArg *)LL) 
      //----------------------------------------------------------------------
      // Input argument value gets the last output argument value
      //----------------------------------------------------------------------
      currArg = lastArgIn = lastArgOut;

    else {
      // currArg = lastArgIn = *p->argIn;
      memcpy(&lastArgIn,p->argIn,p->argInSize);
      memcpy(&currArg,&lastArgIn,p->argInSize);
    }

    // ------------------------------------------------------------
    // Call the test-function with action-number and argument
    // ------------------------------------------------------------
    actualRet    = (*(p->function))(p->action, &currArg);
    lastArgOut   = currArg; // In case the test proc sets values in currArg struct
    
    // Check the returned value
    if (p->expRet != XX && p->expRet != actualRet) {
      printf("==> FAILED: return value expected:%s - actual:%s\n",
	(p->expRet==SMM_OK)?"OK":"ERR",
	(actualRet==SMM_OK)?"OK":"ERR");
      nFailed++;
      if (TestBreakOnFail || p->breakOnFail) 
	break; // quit the main loop and exit the test program!
    }
    else {
      printf("==> PASSED\n");      
    }

    PRINT_VERBOSE("\n");    

    TestSleep(TestSlowdown,NULL);

    // MVE request a confirmation to proceed to the next test case
    //printf("Press Enter to proceed\n");
    //getchar();

  } // for(iTest)

  // Tell overall test result
  if (p->breakOnFail && nFailed > 0) {
    printf("Cannot continue from test %s\n", p->ident);
  }
  else if (TestBreakOnFail && nFailed > 0) {
    printf("TestBreakOnFail requested after test %s\n", p->ident);
  }
  
  printf("========================================================\n"
	 "==========   TOTAL: %3d Tests, %3d Error(s)   ==========\n"
	 "========================================================\n", 
	 iTest-nSkipped, nFailed);
  
  return(nFailed);
}
