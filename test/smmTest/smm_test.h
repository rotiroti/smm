/*----------------------------------------------------------
*
* smm - simple memory manager
* Progetto AA0809 
* Laboratorio Sistemi Operativi I
* Include file for test software.
*
* Creation date: 02/01/2009
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
#ifndef SMM_TEST_H
#define SMM_TEST_H

//----------------------------------------------------------
// Error codes
//----------------------------------------------------------
#define SMM_OK   0
#define SMM_ERR -1 

//----------------------------------------------------------
// Flags for server up/down
//----------------------------------------------------------
#define SERVERISUP   1
#define SERVERISDOWN 0

//----------------------------------------------------------
// Macro for comparing generic data struct
//----------------------------------------------------------
#define EQ_ARG(testArg1, testArg2, n) \
	(memcmp((void *)(testArg1),(void *)(testArg2),(n)) == 0)

//----------------------------------------------------------
// Special symbolic values in the Test Case table
//----------------------------------------------------------
#define XX 0x0A0B0C0D	/* don't care (4 byte type) */
#define YY 0x0A0B	/* don't care (2 byte type) */
#define LL 0xA0B0C0D0	/* input arg is the last output arg OR
                           output arg is the last input arg */

// breakOnFail field value
#define GO 0 // GO to the next test also if error occurs
#define BR 1 // BReak and exit if error occurs

//==========================================================
// Data structs for SMM syscalls according to specs
///=========================================================
typedef struct _SMM_MallocArg {
  // --- Input values
  size_t size; 

  // --- Output  value
  void *pout;

  // --- Auxiliary parameters
  int poutIdx; // Index of the TestPtr[] pointer array where to store the pout,
               //  to be used for successive test cases
} SMM_MallocArg;

typedef struct _SMM_CallocArg {
  // --- Input values
  size_t nmemb;
  size_t size;

  // --- Output value
  void *pout;

  // --- Auxiliary parameters
  int poutIdx; // Index of the TestPtr[] pointer array where to store the pout,
               //  to be used for successive test cases
} SMM_CallocArg;

typedef struct _SMM_ReallocArg {
  // --- Input values
  void *pin;
  size_t size;


  // --- Output value
  void *pout;

  // --- Auxiliary parameters
  int ptrOffsetStart; // offset of the put ptr wrt the start 
                      // pointer of the block
  int pinIdx;  // Index of the TestPtr[] pointer array from where to get the input pointer;
               // -1 means that pin value in this struct has to be used
  int poutIdx; // Index of the TestPtr[] pointer array where to store the pout,
               //  to be used for successive test cases
} SMM_ReallocArg;

typedef struct _SMM_FreeArg {
  // --- Input values
  void *pin;

  // --- Output value
  int rc;

  // --- Auxiliary parameters
  int pinIdx;  // Index of the TestPtr[] pointer array from where to get the input pointer;
               // -1 means that pin value in this struct has to be used
} SMM_FreeArg;

typedef struct _SMM_PutArg {
  // --- Input values
  void *buf;
  void *pin;
  size_t size;

  // --- Output value
  int rc;

  // --- Auxiliary parameters
  int ptrOffsetStart; // offset of the put ptr wrt the start 
                      // pointer of the block
  int pinIdx;  // Index of the TestPtr[] pointer array from where to get the input pointer;
               // -1 means that pin value in this struct has to be used
} SMM_PutArg;

typedef struct _SMM_GetArg {
  // --- Input values
  void *pin;
  void *buf;
  size_t size;

  // --- Output value
  int rc;

  // --- Auxiliary parameters
  int ptrOffsetStart; // offset of the get ptr wrt the start 
                      // pointer of the block
  int pinIdx;  // Index of the TestPtr[] pointer array from where to get the input pointer;
               // -1 means that pin value in this struct has to be used
} SMM_GetArg;

typedef struct _SMM_CmpBufArg {
  // --- Input values
  void *buf1;
  void *buf2;
  size_t size;

  // --- Output value
  int rc;
} SMM_CmpBufArg;

typedef struct _SMM_ChkPtrArg {
  // --- Input values
  void *pin;

  // --- Output values
  struct_smm_blk *blk; // information is stored into blk
  int rc;

  // --- Auxiliary parameters
  int ptrOffsetStart; // offset of the checked ptr wrt the start 
                      // pointer of the block
  int blkExpSize; // if >0, means expected dimension of checked block
  int pinIdx;  // Index of the TestPtr[] pointer array from where to get the input pointer;
               // -1 means that pin value in this struct has to be used
} SMM_ChkPtrArg;

// MVE Da inserire struct per funzioni del Progetto


//=====================================================================
// Union of all possible args of SMM functions
//=====================================================================
typedef union _SMM_FuncArg {
  // Args of syscalls according to specs
  SMM_MallocArg    mallocArg;
  SMM_CallocArg    callocArg;
  SMM_ReallocArg   reallocArg;
  SMM_FreeArg      freeArg;
  SMM_PutArg       putArg;
  SMM_GetArg       getArg;
  SMM_CmpBufArg    cmpBufArg;
  SMM_ChkPtrArg    chkPtrArg;
  // MVE Da inserire struct per funzioni estese del Progetto
} SMM_FuncArg;

typedef SMM_FuncArg TestArg;

typedef int (*TestFunc)(int command, TestArg *arg); // test-function, NULL:none

// All test-steps are stated in table of this format:
typedef struct {
  const char    *ident;		// ident of the step "SMM.xx.yy"
  TestFunc       function;      // test-function
  const int      expRet;	// expected return code
  const int      breakOnFail;	// abort in case of failure
  // Optional params
  const int      action;	// action number
  const TestArg *argIn;         // pointer to input argument value
  const size_t   argInSize;     // actual size in bytes of the in argument structure
} TestCase;

//----------------------------------------------------------
// Prototypes for test support functions
//----------------------------------------------------------

static void TestInitWBufs(void);

static int  TestSleep(int delay, void *arg);
static int  TestProcedure(const char *ident);

static int  TestMalloc(int action, TestArg *arg);
static int  TestCalloc(int action, TestArg *arg);
static int  TestRealloc(int action, TestArg *arg);

static int  TestFree(int action, TestArg *arg);

static int  TestPut(int action, TestArg *arg);
static int  TestGet(int action, TestArg *arg);
static int  TestCmpBuf(int action, TestArg *arg);

static int  TestChkPtr(int action, TestArg *arg);

static int  TestPerformance(int action, TestArg *arg);
static int  TestProtection(int action, TestArg *arg);

// MVE Da inserire supporto per funzioni estese del Progetto

//------------------------------------------
// Global variables
//------------------------------------------
short TestPID; // PID of this process

int  TestRc;  // Return code of last called test proc
void *TestPtr[10]; // Array used to store pointers returned from the allocation functions
struct_smm_blk TestBlk; // Struct blk used for getting back block info
unsigned char TestBufW[DIM_16MB], TestBufR[DIM_16MB], TestBufZ[DIM_16MB];
unsigned char TestBufW1[DIM_4KB], TestBufW2[DIM_4KB]; 

int  TestSlowdown=100; // Wait time in millisecs for delaying execution of next test case
int  TestBreakOnFail=0; // Stop test sequence if one fails

//-----------------------------------------------------------
// Test procedure definition table
// It lists all the acceptance test of this device driver
//-----------------------------------------------------------

//-------------------------------------------------
//- ArgIn for TEST smm_malloc                  ----
//-------------------------------------------------
SMM_MallocArg ArgIn_Malloc_0B        = {DIM_0B,NULL,0};
SMM_MallocArg ArgIn_Malloc_1B        = {DIM_1B,NULL,0};
SMM_MallocArg ArgIn_Malloc_1KB       = {DIM_1KB,NULL,0};
SMM_MallocArg ArgIn_Malloc_4KB_P1    = {DIM_4KB,NULL,1};
SMM_MallocArg ArgIn_Malloc_4KB_P2    = {DIM_4KB,NULL,2};
SMM_MallocArg ArgIn_Malloc_1MB       = {DIM_1MB,NULL,0};
SMM_MallocArg ArgIn_Malloc_2MB_P5    = {DIM_2MB,NULL,5};
SMM_MallocArg ArgIn_Malloc_4MB       = {DIM_4MB,NULL,0};
SMM_MallocArg ArgIn_Malloc_4MB_P1    = {DIM_4MB,NULL,1};
SMM_MallocArg ArgIn_Malloc_4MB_P2    = {DIM_4MB,NULL,2};
SMM_MallocArg ArgIn_Malloc_4MB_P3    = {DIM_4MB,NULL,3};
SMM_MallocArg ArgIn_Malloc_4MB_P4    = {DIM_4MB,NULL,4};
SMM_MallocArg ArgIn_Malloc_5MB_P6    = {DIM_5MB,NULL,6};
SMM_MallocArg ArgIn_Malloc_8MB_P1    = {DIM_8MB,NULL,1};
SMM_MallocArg ArgIn_Malloc_8MBp1B_P2 = {DIM_8MB+1,NULL,2};
SMM_MallocArg ArgIn_Malloc_16MB      = {DIM_16MB,NULL,0};
SMM_MallocArg ArgIn_Malloc_16MBp1B   = {DIM_16MB+1,NULL,0};

//-------------------------------------------------
//- ArgIn for TEST smm_calloc                  ----
//-------------------------------------------------
SMM_CallocArg ArgIn_Calloc_1Elemx1B      = {1,DIM_1B,NULL,1};
SMM_CallocArg ArgIn_Calloc_1Elemx4MB     = {1,DIM_4MB,NULL,2};
SMM_CallocArg ArgIn_Calloc_4MElemx1B     = {4194304,DIM_1B,NULL,3};
SMM_CallocArg ArgIn_Calloc_1Elemx16MB    = {1,DIM_16MB,NULL,0};
SMM_CallocArg ArgIn_Calloc_4KElemx4KB    = {4096,DIM_4KB,NULL,0};
SMM_CallocArg ArgIn_Calloc_16MElemx1B    = {16777216,DIM_1B,NULL,0};
SMM_CallocArg ArgIn_Calloc_1Elemx0B      = {1,DIM_0B,NULL,0};
SMM_CallocArg ArgIn_Calloc_0Elemx1B      = {0,DIM_1B,NULL,0};
SMM_CallocArg ArgIn_Calloc_1Elemx16MBp1B = {1,DIM_16MB+1,NULL,0};
SMM_CallocArg ArgIn_Calloc_16Mp1Elemx1B  = {16777217,DIM_1B,NULL,0};
SMM_CallocArg ArgIn_Calloc_4Kp1Elemx4KB  = {4097,DIM_4KB,NULL,0};
SMM_CallocArg ArgIn_Calloc_4KElemx4KBp1B = {4096,DIM_4KB+1,NULL,0};

//-------------------------------------------------
//- ArgIn for TEST smm_realloc                 ----
//-------------------------------------------------
SMM_ReallocArg ArgIn_Realloc_2KB_P1P3         = {NULL,DIM_2KB,NULL,0,1,3};
SMM_ReallocArg ArgIn_Realloc_8KB_P3P4         = {NULL,DIM_8KB,NULL,0,3,4};
SMM_ReallocArg ArgIn_Realloc_0B_P4P5          = {NULL,DIM_0B,NULL,0,4,5};
SMM_ReallocArg ArgIn_Realloc_16MBp1B_P4P5     = {NULL,DIM_16MB+1,NULL,0,4,5};
SMM_ReallocArg ArgIn_Realloc_4KB_OFF16MB_P4P5 = {NULL,DIM_4KB,NULL,DIM_16MB,4,5};
SMM_ReallocArg ArgIn_Realloc_BadPtr           = {(void *)0x12345678,DIM_1B,NULL,0,-1,0};
SMM_ReallocArg ArgIn_Realloc_NULL             = {NULL,DIM_1B,NULL,0,-1,0};

//-------------------------------------------------
//- ArgIn for TEST smm_chk_ptr                 ----
//-------------------------------------------------
SMM_ChkPtrArg ArgIn_ChkPtr_1B            = {NULL,&TestBlk,0,0,DIM_1B,0};
SMM_ChkPtrArg ArgIn_ChkPtr_1KB           = {NULL,&TestBlk,0,0,DIM_1KB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_2KB_P3        = {NULL,&TestBlk,0,0,DIM_2KB,3};
SMM_ChkPtrArg ArgIn_ChkPtr_4KB_P1        = {NULL,&TestBlk,0,0,DIM_4KB,1};
SMM_ChkPtrArg ArgIn_ChkPtr_8KB_P4        = {NULL,&TestBlk,0,0,DIM_8KB,4};
SMM_ChkPtrArg ArgIn_ChkPtr_1MB           = {NULL,&TestBlk,0,0,DIM_1MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_4MB           = {NULL,&TestBlk,0,0,DIM_4MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_4MB_OFF1B     = {NULL,&TestBlk,0,1,DIM_4MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_4MB_OFF2MB    = {NULL,&TestBlk,0,DIM_2MB,DIM_4MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_4MB_OFF4MBm2B = {NULL,&TestBlk,0,DIM_4MB-2,DIM_4MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_4MB_OFF4MBm1B = {NULL,&TestBlk,0,DIM_4MB-1,DIM_4MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_4MB_OFF4MB    = {NULL,&TestBlk,0,DIM_4MB,DIM_4MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_4MB_OFF8MB    = {NULL,&TestBlk,0,DIM_8MB,DIM_4MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_4MB_OFF16MB   = {NULL,&TestBlk,0,DIM_16MB,DIM_4MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_16MB          = {NULL,&TestBlk,0,0,DIM_16MB,0};
SMM_ChkPtrArg ArgIn_ChkPtr_BadPtr        = {(void *)0x12345678,&TestBlk,0,0,0,-1};
SMM_ChkPtrArg ArgIn_ChkPtr_NULL          = {NULL,&TestBlk,0,0,0,-1};

//-------------------------------------------------
//- ArgIn for TEST smm_free                  ----
//-------------------------------------------------
SMM_FreeArg ArgIn_Free        = {NULL,0,0};
SMM_FreeArg ArgIn_Free_P1     = {NULL,0,1};
SMM_FreeArg ArgIn_Free_P2     = {NULL,0,2};
SMM_FreeArg ArgIn_Free_P3     = {NULL,0,3};
SMM_FreeArg ArgIn_Free_P4     = {NULL,0,4};
SMM_FreeArg ArgIn_Free_P5     = {NULL,0,5};
SMM_FreeArg ArgIn_Free_P6     = {NULL,0,6};
SMM_FreeArg ArgIn_Free_NULL   = {NULL,0,-1};
SMM_FreeArg ArgIn_Free_BadPtr = {(void *)0x12345678,0,-1};

//-------------------------------------------------
//- ArgIn for TEST smm_put                     ----
//-------------------------------------------------
SMM_PutArg ArgIn_Put_0B          = {TestBufW,NULL,DIM_0B,0,0,0};
SMM_PutArg ArgIn_Put_1B          = {TestBufW,NULL,DIM_1B,0,0,0};
SMM_PutArg ArgIn_Put_1B_OFF16MB  = {TestBufW,NULL,DIM_1B,0,DIM_16MB,0};
SMM_PutArg ArgIn_Put_16B_OFF16MBm15B = {TestBufW,NULL,DIM_16B,0,DIM_16MB-15,0};
SMM_PutArg ArgIn_Put_1KB         = {TestBufW,NULL,DIM_1KB,0,0,0};
SMM_PutArg ArgIn_Put_1KB_OFF8MBp128B = {TestBufW,NULL,DIM_1KB,0,DIM_8MB+128,0};
SMM_PutArg ArgIn_Put_4KB_Buf1_P1 = {TestBufW1,NULL,DIM_4KB,0,0,1};
SMM_PutArg ArgIn_Put_4KB_Buf2_P2 = {TestBufW2,NULL,DIM_4KB,0,0,2};
SMM_PutArg ArgIn_Put_1MB         = {TestBufW,NULL,DIM_1MB,0,0,0};
SMM_PutArg ArgIn_Put_16MB        = {TestBufW,NULL,DIM_16MB,0,0,0};
SMM_PutArg ArgIn_Put_16MBp1B     = {TestBufW,NULL,DIM_16MB+1,0,0,0};
SMM_PutArg ArgIn_Put_BadPtr      = {TestBufW,(void *)0x12345678,DIM_1B,0,0,-1};
SMM_PutArg ArgIn_Put_NULL        = {TestBufW,NULL,DIM_1B,0,0,-1};

//-------------------------------------------------
//- ArgIn for TEST smm_get                     ----
//-------------------------------------------------
SMM_GetArg ArgIn_Get_0B          = {NULL,TestBufR,DIM_0B,0,0,0};
SMM_GetArg ArgIn_Get_1B          = {NULL,TestBufR,DIM_1B,0,0,0};
SMM_GetArg ArgIn_Get_1B_P1       = {NULL,TestBufR,DIM_1B,0,0,1};
SMM_GetArg ArgIn_Get_1B_OFF16MB  = {NULL,TestBufR,DIM_1B,0,DIM_16MB,0};
SMM_GetArg ArgIn_Get_16B_OFF16MBm15B = {NULL,TestBufR,DIM_16B,0,DIM_16MB-15,0};
SMM_GetArg ArgIn_Get_1KB         = {NULL,TestBufR,DIM_1KB,0,0,0};
SMM_GetArg ArgIn_Get_1KB_OFF8MBp128B = {NULL,TestBufR,DIM_1KB,0,DIM_8MB+128,0};
SMM_GetArg ArgIn_Get_2KB_P3      = {NULL,TestBufR,DIM_2KB,0,0,3};
SMM_GetArg ArgIn_Get_2KB_P4      = {NULL,TestBufR,DIM_2KB,0,0,4};
SMM_GetArg ArgIn_Get_4KB_P2      = {NULL,TestBufR,DIM_4KB,0,0,2};
SMM_GetArg ArgIn_Get_1MB         = {NULL,TestBufR,DIM_1MB,0,0,0};
SMM_GetArg ArgIn_Get_4MB_P2      = {NULL,TestBufR,DIM_4MB,0,0,2};
SMM_GetArg ArgIn_Get_4MB_P3      = {NULL,TestBufR,DIM_4MB,0,0,3};
SMM_GetArg ArgIn_Get_16MB        = {NULL,TestBufR,DIM_16MB,0,0,0};
SMM_GetArg ArgIn_Get_16MBp1B     = {NULL,TestBufR,DIM_16MB+1,0,0,0};
SMM_GetArg ArgIn_Get_BadPtr      = {(void *)0x12345678,TestBufR,DIM_1B,0,0,-1};
SMM_GetArg ArgIn_Get_NULL        = {NULL,TestBufR,DIM_1B,0,0,-1};

//-------------------------------------------------
//- ArgIn for TEST TestCmpBuf                  ----
//-------------------------------------------------
SMM_GetArg ArgIn_CmpBuf_1B        = {TestBufW,TestBufR,DIM_1B,0};
SMM_GetArg ArgIn_CmpBuf_1B_Zero   = {TestBufZ,TestBufR,DIM_1B,0};
SMM_GetArg ArgIn_CmpBuf_1KB       = {TestBufW,TestBufR,DIM_1KB,0};
SMM_GetArg ArgIn_CmpBuf_2KB_Buf1  = {TestBufW1,TestBufR,DIM_2KB,0};
SMM_GetArg ArgIn_CmpBuf_4KB_Buf2  = {TestBufW2,TestBufR,DIM_4KB,0};
SMM_GetArg ArgIn_CmpBuf_1MB       = {TestBufW,TestBufR,DIM_1MB,0};
SMM_GetArg ArgIn_CmpBuf_4MB_Zero  = {TestBufZ,TestBufR,DIM_4MB,0};
SMM_GetArg ArgIn_CmpBuf_16MB      = {TestBufW,TestBufR,DIM_16MB,0};
SMM_GetArg ArgIn_CmpBuf_16MB_Zero = {TestBufZ,TestBufR,DIM_16MB,0};


//========================================================================================
// Complete LIST of TESTS
//========================================================================================
static TestCase TestList[] = {
  // ident, function, expRet, breakOnFail, 
  //   action, argIn, argInSize, expArgOut, argOutSize

  //--------------------------------------------------------------------------------------
  // LIB.AFOK Test Group -> LIBrary: Alloc and Free with OK return code
  //--------------------------------------------------------------------------------------
  { "LIB.AFOK.01-Malloc1B",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_1B,sizeof(SMM_MallocArg)},

  { "LIB.AFOK.02-ChkPtr1B",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_1B,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFOK.03-Free1B",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  { "LIB.AFOK.04-ChkPtr1B",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_1B,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFOK.05-Malloc1KB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_1KB,sizeof(SMM_MallocArg)},

  { "LIB.AFOK.06-ChkPtr1KB",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_1KB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFOK.07-Free1KB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  { "LIB.AFOK.08-ChkPtr1KB",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_1KB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFOK.09-Malloc1MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_1MB,sizeof(SMM_MallocArg)},

  { "LIB.AFOK.10-ChkPtr1MB",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_1MB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFOK.11-Free1MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  { "LIB.AFOK.12-ChkPtr1MB",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_1MB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFOK.13-Malloc16MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_16MB,sizeof(SMM_MallocArg)},

  { "LIB.AFOK.14-ChkPtr16MB",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_16MB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFOK.15-Free16MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  { "LIB.AFOK.16-ChkPtr16MB",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_16MB,sizeof(SMM_ChkPtrArg)},

  //--------------------------------------------------------------------------------------
  // LIB.AFER Test Group -> LIBrary: Alloc and Free with ERror return code
  //--------------------------------------------------------------------------------------
  { "LIB.AFER.01-Malloc0B",TestMalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Malloc_0B,sizeof(SMM_MallocArg)},

  { "LIB.AFER.02-Malloc16MB+1B",TestMalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Malloc_16MBp1B,sizeof(SMM_MallocArg)},

  { "LIB.AFER.03-FreeNULL",TestFree,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Free_NULL,sizeof(SMM_FreeArg)},

  { "LIB.AFER.04-FreeBadPtr",TestFree,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Free_BadPtr,sizeof(SMM_FreeArg)},

  { "LIB.AFER.05-Malloc8MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_8MB_P1,sizeof(SMM_MallocArg)},

  { "LIB.AFER.06-Malloc8MB+1B",TestMalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Malloc_8MBp1B_P2,sizeof(SMM_MallocArg)},

  { "LIB.AFER.07-Free8MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P1,sizeof(SMM_FreeArg)},

  { "LIB.AFER.08-Free8MBAgain",TestFree,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Free_P1,sizeof(SMM_FreeArg)},

  { "LIB.AFER.09-Free8MB+1B",TestFree,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Free_P2,sizeof(SMM_FreeArg)},

  { "LIB.AFER.10-Malloc4MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_4MB,sizeof(SMM_MallocArg)},

  { "LIB.AFER.11-ChkPtr4MB",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4MB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.12-ChkPtr4MB+Offset1B",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4MB_OFF1B,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.13-ChkPtr4MB+Offset2MB",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4MB_OFF2MB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.14-ChkPtr4MB+Offset4MB-2B",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4MB_OFF4MBm2B,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.15-ChkPtr4MB+Offset4MB-1B",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4MB_OFF4MBm1B,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.16-ChkPtr4MB+BadOffset4MB",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4MB_OFF4MB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.17-ChkPtr4MB+BadOffset8MB",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4MB_OFF8MB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.18-ChkPtr4MB+BadOffset16MB",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4MB_OFF16MB,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.19-ChkPtrBadPtr",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_BadPtr,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.20-ChkPtrNULL",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_NULL,sizeof(SMM_ChkPtrArg)},

  { "LIB.AFER.21-Free4MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  //--------------------------------------------------------------------------------------
  // LIB.CPT Test Group -> LIBrary: ComPacTing memory functionality
  //--------------------------------------------------------------------------------------
  { "LIB.CPT.01-Malloc4MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_4MB_P1,sizeof(SMM_MallocArg)},

  { "LIB.CPT.02-Malloc4MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_4MB_P2,sizeof(SMM_MallocArg)},

  { "LIB.CPT.03-Malloc4MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_4MB_P3,sizeof(SMM_MallocArg)},

  { "LIB.CPT.04-Malloc4MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_4MB_P4,sizeof(SMM_MallocArg)},

  { "LIB.CPT.05-Free4MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P1,sizeof(SMM_FreeArg)},

  { "LIB.CPT.06-Malloc2MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_2MB_P5,sizeof(SMM_MallocArg)},

  { "LIB.CPT.07-Free4MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P3,sizeof(SMM_FreeArg)},

  { "LIB.CPT.08-Malloc5MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_5MB_P6,sizeof(SMM_MallocArg)},

  { "LIB.CPT.09-Free4MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P2,sizeof(SMM_FreeArg)},

  { "LIB.CPT.10-Free4MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P4,sizeof(SMM_FreeArg)},

  { "LIB.CPT.11-Free2MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P5,sizeof(SMM_FreeArg)},

  { "LIB.CPT.12-Free5MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P6,sizeof(SMM_FreeArg)},

  //--------------------------------------------------------------------------------------
  // LIB.PG Test Group -> LIBrary: Put and Get
  //--------------------------------------------------------------------------------------
  { "LIB.PG.01-Malloc16MB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_16MB,sizeof(SMM_MallocArg)},

  { "LIB.PG.02-Put1B",TestPut,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Put_1B,sizeof(SMM_PutArg)},

  { "LIB.PG.03-Get1B",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_1B,sizeof(SMM_GetArg)},

  { "LIB.PG.04-CmpBuf_1B",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_1B,sizeof(SMM_CmpBufArg)},

  { "LIB.PG.05-Put1KB",TestPut,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Put_1KB,sizeof(SMM_PutArg)},

  { "LIB.PG.06-Get1KB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_1KB,sizeof(SMM_GetArg)},

  { "LIB.PG.07-CmpBuf_1KB",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_1KB,sizeof(SMM_CmpBufArg)},

  { "LIB.PG.08-Put1MB",TestPut,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Put_1MB,sizeof(SMM_PutArg)},

  { "LIB.PG.09-Get1MB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_1MB,sizeof(SMM_GetArg)},

  { "LIB.PG.10-CmpBuf_1MB",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_1MB,sizeof(SMM_CmpBufArg)},

  { "LIB.PG.11-Put16MB",TestPut,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Put_16MB,sizeof(SMM_PutArg)},

  { "LIB.PG.12-Get16MB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_16MB,sizeof(SMM_GetArg)},

  { "LIB.PG.13-CmpBuf_16MB",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_16MB,sizeof(SMM_CmpBufArg)},

  { "LIB.PG.14-Put1KB+Offset8MB+128B",TestPut,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Put_1KB_OFF8MBp128B,sizeof(SMM_PutArg)},

  { "LIB.PG.15-Get1KB+Offset8MB+128B",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_1KB_OFF8MBp128B,sizeof(SMM_GetArg)},

  { "LIB.PG.16-CmpBuf_1KB",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_1KB,sizeof(SMM_CmpBufArg)},

  { "LIB.PG.17-Put0B",TestPut,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Put_0B,sizeof(SMM_PutArg)},

  { "LIB.PG.18-Get0B",TestGet,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Get_0B,sizeof(SMM_GetArg)},

  { "LIB.PG.19-Put16MB+1B",TestPut,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Put_16MBp1B,sizeof(SMM_PutArg)},

  { "LIB.PG.20-Get16MB+1B",TestGet,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Get_16MBp1B,sizeof(SMM_GetArg)},

  { "LIB.PG.21-PutBufOverflow",TestPut,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Put_16B_OFF16MBm15B,sizeof(SMM_PutArg)},

  { "LIB.PG.22-GetBufOverflow",TestGet,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Get_16B_OFF16MBm15B,sizeof(SMM_GetArg)},

  { "LIB.PG.23-PutToBadOffset",TestPut,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Put_1B_OFF16MB,sizeof(SMM_PutArg)},

  { "LIB.PG.24-GetFromBadOffset",TestGet,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Get_1B_OFF16MB,sizeof(SMM_GetArg)},

  { "LIB.PG.25-Free16MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  { "LIB.PG.26-Put_BadPtr",TestPut,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Put_BadPtr,sizeof(SMM_PutArg)},

  { "LIB.PG.27-Get_BadPtr",TestGet,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Get_BadPtr,sizeof(SMM_GetArg)},

  { "LIB.PG.28-Put_NULL",TestPut,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Put_NULL,sizeof(SMM_PutArg)},

  { "LIB.PG.29-Get_NULL",TestGet,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Get_NULL,sizeof(SMM_GetArg)},

  //--------------------------------------------------------------------------------------
  // LIB.CAL Test Group -> LIBrary: CALloc
  //--------------------------------------------------------------------------------------
  { "LIB.CAL.01-Calloc1Elemx1B",TestCalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Calloc_1Elemx1B,sizeof(SMM_CallocArg)},

  { "LIB.CAL.02-Get1B",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_1B_P1,sizeof(SMM_GetArg)},

  { "LIB.CAL.03-CmpBuf_1BwrtZero",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_1B_Zero,sizeof(SMM_CmpBufArg)},

  { "LIB.CAL.04-Calloc1Elemx4MB",TestCalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Calloc_1Elemx4MB,sizeof(SMM_CallocArg)},

  { "LIB.CAL.05-Get4MB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_4MB_P2,sizeof(SMM_GetArg)},

  { "LIB.CAL.06-CmpBuf_4MBwrtZero",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_4MB_Zero,sizeof(SMM_CmpBufArg)},

  { "LIB.CAL.07-Calloc4MElemx1B",TestCalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Calloc_4MElemx1B,sizeof(SMM_CallocArg)},

  { "LIB.CAL.08-Get4MB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_4MB_P3,sizeof(SMM_GetArg)},

  { "LIB.CAL.09-CmpBuf_4MBwrtZero",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_4MB_Zero,sizeof(SMM_CmpBufArg)},

  { "LIB.CAL.10-Free1B",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P1,sizeof(SMM_FreeArg)},

  { "LIB.CAL.11-Free4MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P2,sizeof(SMM_FreeArg)},

  { "LIB.CAL.12-Free4MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P3,sizeof(SMM_FreeArg)},

  { "LIB.CAL.13-Calloc1Elemx16MB",TestCalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Calloc_1Elemx16MB,sizeof(SMM_CallocArg)},

  { "LIB.CAL.14-Get16MB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_16MB,sizeof(SMM_GetArg)},

  { "LIB.CAL.15-CmpBuf_16MBwrtZero",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_16MB_Zero,sizeof(SMM_CmpBufArg)},

  { "LIB.CAL.16-Free16MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  { "LIB.CAL.17-Calloc4KElemx4KB",TestCalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Calloc_4KElemx4KB,sizeof(SMM_CallocArg)},

  { "LIB.CAL.18-Get16MB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_16MB,sizeof(SMM_GetArg)},

  { "LIB.CAL.19-CmpBuf_16MBwrtZero",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_16MB_Zero,sizeof(SMM_CmpBufArg)},

  { "LIB.CAL.20-Free16MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  { "LIB.CAL.21-Calloc16MElemx1B",TestCalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Calloc_16MElemx1B,sizeof(SMM_CallocArg)},

  { "LIB.CAL.22-Get16MB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_16MB,sizeof(SMM_GetArg)},

  { "LIB.CAL.23-CmpBuf_16MBwrtZero",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_16MB_Zero,sizeof(SMM_CmpBufArg)},

  { "LIB.CAL.24-Free16MB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free,sizeof(SMM_FreeArg)},

  { "LIB.CAL.25-Calloc1Elemx0B",TestCalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Calloc_1Elemx0B,sizeof(SMM_CallocArg)},

  { "LIB.CAL.26-Calloc0Elemx1B",TestCalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Calloc_0Elemx1B,sizeof(SMM_CallocArg)},

  { "LIB.CAL.27-Calloc1Elemx16MB+1B",TestCalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Calloc_1Elemx16MBp1B,sizeof(SMM_CallocArg)},

  { "LIB.CAL.28-Calloc16M+1Elemx1B",TestCalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Calloc_16Mp1Elemx1B,sizeof(SMM_CallocArg)},

  { "LIB.CAL.29-Calloc4K+1Elemx4KB",TestCalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Calloc_4Kp1Elemx4KB,sizeof(SMM_CallocArg)},

  { "LIB.CAL.30-Calloc4KElemx4KB+1B",TestCalloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Calloc_4KElemx4KBp1B,sizeof(SMM_CallocArg)},

  //--------------------------------------------------------------------------------------
  // LIB.REA Test Group -> LIBrary: REAlloc
  //--------------------------------------------------------------------------------------
  { "LIB.REA.01-Malloc4KB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_4KB_P1,sizeof(SMM_MallocArg)},

  { "LIB.REA.02-Put4KB",TestPut,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Put_4KB_Buf1_P1,sizeof(SMM_PutArg)},

  { "LIB.REA.03-Malloc4KB",TestMalloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Malloc_4KB_P2,sizeof(SMM_MallocArg)},

  { "LIB.REA.04-Put4KB",TestPut,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Put_4KB_Buf2_P2,sizeof(SMM_PutArg)},

  { "LIB.REA.05-Realloc4KBto2KB",TestRealloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Realloc_2KB_P1P3,sizeof(SMM_ReallocArg)},

  { "LIB.REA.06-Get2KB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_2KB_P3,sizeof(SMM_GetArg)},

  { "LIB.REA.07-CmpBuf_2KB",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_2KB_Buf1,sizeof(SMM_CmpBufArg)},

  { "LIB.REA.08-Get4KB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_4KB_P2,sizeof(SMM_GetArg)},

  { "LIB.REA.09-CmpBuf_4KB",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_4KB_Buf2,sizeof(SMM_CmpBufArg)},

  { "LIB.REA.10-ChkPtr2KB",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_2KB_P3,sizeof(SMM_ChkPtrArg)},

  { "LIB.REA.11-ChkPtr4KB",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_4KB_P1,sizeof(SMM_ChkPtrArg)},

  { "LIB.REA.12-Realloc2KBto8KB",TestRealloc,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Realloc_8KB_P3P4,sizeof(SMM_ReallocArg)},

  { "LIB.REA.13-Get2KB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_2KB_P4,sizeof(SMM_GetArg)},

  { "LIB.REA.14-CmpBuf_2KB",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_2KB_Buf1,sizeof(SMM_CmpBufArg)},

  { "LIB.REA.15-Get4KB",TestGet,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Get_4KB_P2,sizeof(SMM_GetArg)},

  { "LIB.REA.16-CmpBuf_4KB",TestCmpBuf,SMM_OK,GO,0,
    (TestArg *)&ArgIn_CmpBuf_4KB_Buf2,sizeof(SMM_CmpBufArg)},

  { "LIB.REA.17-ChkPtr8KB",TestChkPtr,SMM_OK,GO,0,
    (TestArg *)&ArgIn_ChkPtr_8KB_P4,sizeof(SMM_ChkPtrArg)},

  { "LIB.REA.18-ChkPtr2KB",TestChkPtr,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_ChkPtr_2KB_P3,sizeof(SMM_ChkPtrArg)},

  { "LIB.REA.19-Realloc8KBto0B",TestRealloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Realloc_0B_P4P5,sizeof(SMM_ReallocArg)},

  { "LIB.REA.20-Realloc8KBto16MB+1B",TestRealloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Realloc_16MBp1B_P4P5,sizeof(SMM_ReallocArg)},

  { "LIB.REA.21-ReallocPtr+Offset16MB",TestRealloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Realloc_4KB_OFF16MB_P4P5,sizeof(SMM_ReallocArg)},

  { "LIB.REA.22-Free4KB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P2,sizeof(SMM_FreeArg)},

  { "LIB.REA.23-Free8KB",TestFree,SMM_OK,GO,0,
    (TestArg *)&ArgIn_Free_P4,sizeof(SMM_FreeArg)},

  { "LIB.REA.24-ReallocBadPtr",TestRealloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Realloc_BadPtr,sizeof(SMM_ReallocArg)},

  { "LIB.REA.25-ReallocNULL",TestRealloc,SMM_ERR,GO,0,
    (TestArg *)&ArgIn_Realloc_NULL,sizeof(SMM_ReallocArg)},

  //--------------------------------------------------------------------------------------
  // SMM.PER Test Group -> SMM PERformance
  //--------------------------------------------------------------------------------------
  { "SMM.PER.01-Malloc",TestPerformance,SMM_OK,GO,0,
    NULL,0},

  { "SMM.PER.02-Put",TestPerformance,SMM_OK,GO,1,
    NULL,0},

  { "SMM.PER.03-Get",TestPerformance,SMM_OK,GO,2,
    NULL,0},

  //--------------------------------------------------------------------------------------
  // SMM.PRO Test Group -> SMM PROtection
  //--------------------------------------------------------------------------------------
  { "SMM.PRO.01-Free",TestProtection,SMM_OK,GO,0,
    NULL,0},

  { "SMM.PRO.02-Realloc",TestProtection,SMM_OK,GO,1,
    NULL,0},

  { "SMM.PRO.03-Put",TestProtection,SMM_OK,GO,2,
    NULL,0},

  { "SMM.PRO.04-Get",TestProtection,SMM_OK,GO,3,
    NULL,0},

  { "SMM.PRO.05-ChkPtr",TestProtection,SMM_OK,GO,4,
    NULL,0},

  // End-marker, don't delete:
  //--------------------------------------------------------------------------------------
  { "" }
};


#endif /* ! SMM_TEST_H */                
/* ___oOo___ */
