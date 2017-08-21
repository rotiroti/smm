/*------------------------------------------------------------------------
*
* smm - simple memory manager
* Progetto AA0809 
* Laboratorio Sistemi Operativi I
* Include file for test software: 
*  typedefs and protos needed to call SMM syscalls
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
#ifndef SMM_LIB_H
#define SMM_LIB_H

// Typedefs
typedef struct smm_blk {
  void *start; /* pointer to the first byte of the block */
  void *last;  /* pointer to the last byte of the block */
  char  swap;  /* swapped in=0, swapped out=1 */
} struct_smm_blk;

// Prototypes

// Funzioni base per esonero n.2
extern void *smm_malloc(size_t size);
extern void *smm_calloc(size_t nmemb, size_t size);
extern void *smm_realloc(void *ptr, size_t size);
extern int smm_free(void *ptr);
extern int smm_put(void *buf, void *ptr, size_t size); 
extern int smm_get(void *ptr, void *buf, size_t size);
extern int smm_chk_ptr(void *ptr, struct smm_blk *blk); 

// Funzioni estese per Progetto
extern int smm_int_put(void *ptr, int value); 
extern int smm_int_get(void *ptr, int *valuep);
extern int smm_float_put(void *ptr, float value); 
extern int smm_float_get(void *ptr, float *valuep);
extern int smm_intarr_put(void *ptr, int *arr, size_t nmemb);
extern int smm_intarr_get(void *ptr, int *arr, size_t nmemb);
extern int smm_floatarr_put(void *ptr, float *arr, size_t nmemb);
extern int smm_floatarr_get(void *ptr, float *arr, size_t nmemb);

#endif // ! SMM_LIB_H                
/* ___oOo___ */
