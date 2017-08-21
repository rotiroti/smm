/* test sull'assegnazione degli indirizzi logici a seguito di una realloc  */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "smm.h"

int
main(void)
{
     void *ptr = NULL;
     void *ptr2 = NULL;
     void *ptr3 = NULL;
     void *ptr_reall = NULL;
     int rc = 0;

     ptr = smm_malloc(4096);
     printf("ptr: %p\n", ptr);

     ptr2 = smm_malloc(4096);
     printf("ptr2: %p\n", ptr2);

     ptr3 = smm_malloc(4096);
     printf("ptr3: %p\n", ptr3);

     rc = smm_free(ptr2);

     ptr_reall = smm_realloc(ptr,8192);
     printf("ptr: %p\n", ptr);
     printf("ptr_reall: %p\n", ptr_reall);

     rc = smm_free(ptr3);
     rc = smm_free(ptr_reall);

     return 0;
}
