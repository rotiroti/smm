#include <stdio.h>
#include <unistd.h>
#include "smm.h"

int
main(void)
{
     void *ptr = NULL;
     int rc = 0;
     int i = 0;
     
     
     for (i =0; i < 256; ++i) {
	     ptr = smm_malloc(16777216);
	     printf("%#010X\n", (unsigned int)ptr);


	     rc = smm_free(ptr); 
	     printf("ptr: %d\n", rc); 
     }

     return 0;
}
