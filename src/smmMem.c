#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#include "utility.h"
#include "smmMem.h"


/* \struct Struttura dati che definisce un blocco in memoria */
struct elem_list
{
     unsigned int phy_start_addr;                    /* indirizzo fisico di inizio*/
     unsigned int log_start_addr; 	             /* indirizzo logico  di inizio */
     size_t size_block;                              /* dimensione di un blocco */
     pid_t pid;                                      /* pid del processeso (relativo al blocco) */
     struct elem_list *next;                         /* puntaore al blocco successivo */
};


elem_list *
new_elem_list(void)
{
     elem_list *new = NULL;

     new = (elem_list *) malloc(sizeof(elem_list));
     
     return new;
}


void
set_phy_start_addr(elem_list *blk, unsigned int physical_addr)
{
     blk->phy_start_addr = physical_addr;
}


void
set_log_start_addr(elem_list *blk, unsigned int logical_addr)
{
     blk->log_start_addr = logical_addr;
}


void
set_size_block(elem_list *blk, size_t size)
{
     blk->size_block = size;
}


void
set_pid(elem_list *blk, pid_t pid)
{
     blk->pid = pid;
}

void
set_next(elem_list *blk, elem_list *next)
{
     blk->next = next;
}


unsigned int
get_phy_start_addr(elem_list *blk)
{
     return blk->phy_start_addr;
}


unsigned int
get_log_start_addr(elem_list *blk)
{
     return blk->log_start_addr;
}


size_t
get_size_block(elem_list *blk)
{
     return blk->size_block;
}


pid_t
get_pid(elem_list *blk)
{
     return blk->pid;
}


elem_list * 
get_next(elem_list *blk)
{
     return blk->next;
}


int
init_smm_mem_info()
{
     if ((info = (info_smm_mem *) calloc(1, sizeof(info_smm_mem))) == NULL)
     {
	  errno = ENOMEM;
	  return errno;
     }
     
     
     info->empty_space = 16*MB;
     info->holes = insert_block(info->holes, 0, 0, 0, 16*MB);
     info->max_hole = 16*MB;

     if((mkfifo(FIFOTO, 0666) == -1) || (mkfifo(FIFOFROM, 0666) == -1)) {
	  return errno;
     }

     return 0;
}


void
update_smm_mem_info(size_t size)
{
     size_t tmp_max = 0;

     
     info->empty_space -= size;
     tmp_max = search_max_hole(info->holes);

     if (info->max_hole < tmp_max) {
	  info->max_hole = tmp_max;
     }
}


elem_list *
insert_block(elem_list *head, unsigned int physical_addr,
	     unsigned int logical_addr, pid_t pid, size_t size)
{
     elem_list *current = head;
     elem_list *previous = NULL;
     elem_list *new = NULL;


     if (check_physical_addr(current, physical_addr) == -1) {
	  fprintf(stderr, "smmd error: smmMem overwrited");
	  return current;
     }

     if ((new = new_elem_list()) == NULL) {
	  return NULL;
     }

     /* inizializzazione campi del blocco da inserire */
     new->phy_start_addr = physical_addr;
     new->log_start_addr = logical_addr;
     new->pid = pid;
     new->size_block = size;
     new->next = NULL;
     
     while (current != NULL && current->phy_start_addr < physical_addr &&
	   physical_addr > (current->phy_start_addr + current->size_block - 1)) {

	  previous = current;
	  current = current->next;
     }
     
     if (previous != NULL) {
	  previous->next = new;
	  new->next = current;
     }
     else {
	  new->next = head;
	  head = new;
     }

     return head;
}


elem_list *
search_block(elem_list *head , unsigned int address)
{
     elem_list *current = head;

     if (current == NULL) {
	  return NULL;
     }


     if (get_log_start_addr(current) == 0) {

	  while(current != NULL) {
	       if (current->phy_start_addr == address) {
		    return current;
	       }
	       current = current->next;
	  }
     } else {

	  while(current != NULL) {
	       if(current->log_start_addr == address) {
		    return current;
	       }
	       current = current->next;
	  }     
     }

     return NULL;
}


int
how_many_blocks(elem_list *head)
{
     elem_list *current = head;
     int i = 0;


     while (current != NULL) {
	  current = current->next;
	  ++i;
     }

     return i;
}


elem_list * 
remove_block(elem_list *head, unsigned int physical_addr)
{
     elem_list *current = head;
     elem_list *previous = NULL;
     
     
     if (head->phy_start_addr == physical_addr) {
	  head = head->next;
	  free(current);
	  
	  return head;

     } else {
	  previous = head;
	  current = head->next;
	  
	  while(current != NULL) {
	       if (current->phy_start_addr == physical_addr) {    
		    previous->next = current->next;
		    free(current);
		    current = NULL;

		    return head;
	       }
	       previous = current;
	       current = current->next;
	  }
     }
 
     return head;
}


int
remove_all_blocks(elem_list **head_ref)
{
     elem_list *current = *head_ref;
     elem_list *tmp = NULL;


     while (current != NULL) {
	  tmp = current->next;
	  free(current);
	  current = tmp;
     }
     *head_ref = NULL;

     return 0;
}


void
info_smm_mem_blocks(elem_list *head, char *buff)
{
     elem_list *current = head;
     int i = 0;


     if (!current) return;
     
     if (current->pid == 0) {
	  while (current != NULL) {
	       i += snprintf(buff + i, 40, "%p %p %d\n",
			(void *)(smmMem + current->phy_start_addr),
			(void *)(smmMem + (current->phy_start_addr + current->size_block - 1)),
			current->size_block);
	       current = current->next;
	  }
     } else {

	  while (current != NULL) {
	       i += snprintf(buff + i, 40, "%d %p %p %d\n", current->pid,
			(void *)(smmMem + current->phy_start_addr),
			(void *)(smmMem + (current->phy_start_addr + current->size_block - 1)),
			current->size_block);
	       current = current->next;
	  }
     }
}


elem_list *
subset_allocated_blocks(elem_list *head, pid_t pid)
{
     elem_list *current = head;
     elem_list *new = NULL;


     while (current != NULL) {

	  if (current->pid == pid) {
	       new = insert_block(new,
				  current->phy_start_addr,
				  current->log_start_addr, 
				  pid,
				  current->size_block);
	  }
	  current = current->next;
     }
     
     return new;
}


size_t
search_max_hole(elem_list *head)
{
     elem_list *current = head;     
     size_t max = 0;

     
     while(current != NULL) {

	  if(current->size_block > max) {
	       max = current->size_block;
	  }
	  current = current->next;
     }

     return max;
}


elem_list * 
fusion_blocks(elem_list *head, elem_list *blk_fusion)
{
     elem_list *current = head;
     elem_list *tmp = NULL;
     unsigned int sum = blk_fusion->phy_start_addr + blk_fusion->size_block-1;


     while (current != NULL) {
	  if (sum+1 < current->phy_start_addr) {

	       /* blk_fusion non puo' essere fuso con nessun altro blocco libero. */
	       head = insert_block(head, blk_fusion->phy_start_addr, 0, 0, blk_fusion->size_block); 

	       return head;
	  }
	  else if (sum+1 == current->phy_start_addr) {

	       /* blk_fusion viene fuso con il successore. */
	       current->phy_start_addr = blk_fusion->phy_start_addr;
	       current->size_block += blk_fusion->size_block;
	       free(blk_fusion);
	       blk_fusion = NULL;

	       return head;
	  }
	  else if ((current->phy_start_addr + current->size_block) == blk_fusion->phy_start_addr) {
	       /* blk_fusion viene fuso con il predecessore */
	       current->size_block += blk_fusion->size_block;
	       free(blk_fusion);
	       blk_fusion = NULL;

	       /* verifico se il nodo appena aggiornato puo' essere fuso con il suo successore. */
	       sum = current->phy_start_addr + current->size_block;
	       
	       if (current->next != NULL && sum == current->next->phy_start_addr) {
		    tmp = current->next;
		    current->size_block += current->next->size_block;
		    current->next = current->next->next;
		    free(tmp);
		    tmp = NULL;
	       }
	       
	       return head;
	  }
	  current = current->next;
     }
     head = insert_block(head, blk_fusion->phy_start_addr, 0, 0, blk_fusion->size_block);      

     return head;
} 


int 
allocator(elem_list *best_fit, size_t size, unsigned int *logical_addr, pid_t pid)
{
     /* inserimento del blocco nella lista dei blocchi occupati */
     info->processes = insert_block(info->processes,
					  best_fit->phy_start_addr,
					  *logical_addr,
					  pid,
					  size);
     
     if (info->processes == NULL) {
	  return -1;
     }

     /*Cancellazione o ridimensioamento 
       del blocco nella lista dei liberi */
     if(best_fit->size_block == size) {
	  info->holes = remove_block(info->holes, best_fit->phy_start_addr);
     } else {
	  best_fit->phy_start_addr += size;
	  best_fit->size_block -= size;
     }

     *logical_addr += size;

     return 0;
}


int
compact_smm_mem()
{
	elem_list *list_free = NULL;
	elem_list *list_full = NULL;
	elem_list *tmp = NULL;
	unsigned int offset = 0;
	unsigned int old_ph_start_cpy = 0;
	unsigned int ph_last_cpy = 0;


	list_full = info->processes;
	list_free = info->holes;

	
	while (list_full != NULL) {

	     if ((list_full->phy_start_addr - offset) != 0) {
		  old_ph_start_cpy = list_full->phy_start_addr;    
		  list_full->phy_start_addr = offset;
		  offset = (list_full->phy_start_addr + list_full->size_block);
		  memmove((smmMem + (list_full->phy_start_addr)),
			  (smmMem + old_ph_start_cpy), 
			  list_full->size_block);
	     }
	     else {
		  offset += list_full->size_block;
	     }

	     ph_last_cpy = (list_full->phy_start_addr + list_full->size_block);
	     list_full = list_full->next;
	}

	list_free->phy_start_addr = ph_last_cpy;
	list_free->size_block = info->empty_space;
	tmp = list_free->next;
	list_free->next = NULL;
	remove_all_blocks(&tmp);
	
	return 0;
}


elem_list *
best_fit(elem_list *head, size_t size)
{
     elem_list *current = head;
     elem_list *tmp = NULL;


     while (current != NULL) {

	  if (current->size_block >= size) {

	       if (tmp == NULL) {
		    tmp = current;
	       } else {

		    if (current->size_block < tmp->size_block) {
			 tmp = current;
		    }
	       }
	  }
	  current = current->next;
     }
     
     return tmp;
}


int
check_physical_addr(elem_list *head, unsigned int physical_addr)
{
     elem_list *current = head;


     while (current != NULL) {
	  if (physical_addr >= current->phy_start_addr &&
	      physical_addr <= (current->phy_start_addr + current->size_block - 1)) {
	       return -1;
	  }
	  current = current->next;
     }

     return 0;
}


unsigned int
logical_to_physical_addr(elem_list *blk, unsigned int logical_addr)
{
     unsigned int offset = 0;

     offset = (logical_addr - blk->log_start_addr);
     
     return (blk->phy_start_addr + offset);
}


elem_list *
check_logical_addr(elem_list *head, unsigned int logical_addr)
{
     elem_list *current = head;

     while (current != NULL) {
	  if (logical_addr >= current->log_start_addr &&
	      logical_addr <= (current->log_start_addr + current->size_block - 1)) {
	       return current;
	  }
	  current = current->next;
     }

     return NULL;
}


void
log_msg_error(char msg[256], char *func, int err)
{
     time_t timep;
     char log_time[30];

     (void)time(&timep);
     strcpy(log_time, ctime(&timep));
     log_time[strlen(log_time)-1] = '\0';
     snprintf(msg, 256, "%s - %s - %s: %s\n", log_time, "smmd", func, strerror(err));
}


int
smmd_realloc(elem_list *blk, packet *send_data, unsigned int *logical_addr) 
{
     int rc = 0;
     unsigned int tmp_addr = 0;
     size_t offset = 0;
     elem_list *new = NULL;
     elem_list *bestfit = NULL;
     char *buff = NULL;



     if ((buff = (char *) calloc (get_size_block(blk), 1)) == NULL) {
	  send_data->p_error = ENOMEM;

	  return -1;
     } else {
	  memcpy(buff, smmMem+get_phy_start_addr(blk), get_size_block(blk));

	  if ((new = new_elem_list()) == NULL) {
	       send_data->p_error = ENOMEM;

	       return -1;
	  }
	  	  
	  offset = get_size_block(blk);
	  set_pid(new, get_pid(blk));
	  set_size_block(new, get_size_block(blk));
	  set_phy_start_addr(new,get_phy_start_addr(blk));
	  set_log_start_addr(new,0);
	  set_next(new, NULL);


	  info->processes = remove_block(info->processes, get_phy_start_addr(blk));
	  info->holes = fusion_blocks(info->holes, new);
	  info->empty_space += offset;
	  info->max_hole = search_max_hole(info->holes);
	  
	  if (send_data->size > info->max_hole) {
	       compact_smm_mem();
	       info->max_hole = info->empty_space;
	  }

	  send_data->address = *logical_addr;
	  bestfit = best_fit(info->holes, send_data->size);
	  tmp_addr = get_phy_start_addr(bestfit);
	  
	  rc = allocator(bestfit, send_data->size, logical_addr, send_data->pid);

	  if (rc < 0) {
	       send_data->p_error = ENOMEM;

	       return -1;
	  }
	  
	  update_smm_mem_info(send_data->size);
	  memcpy(smmMem+tmp_addr, buff, offset);
	  send_data->p_error = 0;
	  free(buff);
	  buff = NULL;
     }
     return 0;
}



void
write_to_log(int fd_server_log,char *buff_error,char *syscall, int error) 
{
     log_msg_error(buff_error, syscall, error);
     write(fd_server_log, buff_error, strlen(buff_error));
}


int
smmd_alloc(unsigned int *logical_addr, packet *send_data)
{
     int rc = 0;
     size_t size = 0;
     elem_list *blk = NULL;

     if (strcmp(send_data->cmd, "malloc") == 0) {
	  size = send_data->size;
     } else {
	  size = send_data->size * send_data->size_type;
     }


     if (info->empty_space >= size) {
          if(info->max_hole <= size) {
               compact_smm_mem();
               info->max_hole = info->empty_space;
          }

          send_data->address = *logical_addr;

          if ((rc = allocator(best_fit(info->holes, size),
                              size,
                              logical_addr,
                              send_data->pid)) < 0) {
               return rc;
          }

          update_smm_mem_info(size);
          send_data->p_error = 0;

	  if (strcmp(send_data->cmd, "calloc") == 0) {
	       blk = search_block(info->processes, send_data->address);
	       memset((smmMem + get_phy_start_addr(blk)), '\0', get_size_block(blk));
	  }
     }
     else {
          send_data->p_error = ENOMEM;
     }

     return 0;
}


int 
smmd_resize(elem_list *blk, packet *send_data)
{
     elem_list *new_hole = NULL;
     size_t offset = 0;

     
     offset = get_size_block(blk) - send_data->size;
     set_size_block(blk, send_data->size);
     
     if((new_hole = new_elem_list()) == NULL ){
	  send_data->p_error =  ENOMEM;
	  return -1;
     }
     
     set_phy_start_addr(new_hole, get_phy_start_addr(blk) + get_size_block(blk));
     set_size_block(new_hole, offset);
     info->empty_space += offset;
     info->holes = fusion_blocks(info->holes, new_hole);
     info->max_hole = search_max_hole(info->holes);
     send_data->p_error = 0;

     return 0;
}


int
smmd_expand(elem_list *blk, packet *send_data, unsigned int *logical_addr)
{
     int rc = 0;
     elem_list *new_hole = NULL;
     size_t offset = 0;
     unsigned int tmp_addr = 0;


     offset = send_data->size - get_size_block(blk);
     
     if (send_data->size > info->empty_space) {
	  send_data->p_error = ENOMEM;
	  return -2;
     } else {
	  /*ho spazio*/
	  tmp_addr = get_phy_start_addr(blk) + get_size_block(blk);
	  new_hole = search_block(info->holes,tmp_addr);

	  if(new_hole == NULL) {

	       /*no hole adiacente*/
	       if(get_next(blk) == NULL) {

		    /* ultimo blocco */
		    compact_smm_mem();
		    info->max_hole = search_max_hole(info->holes);
		    set_size_block(blk, send_data->size);
		    set_phy_start_addr(info->holes,get_phy_start_addr(info->holes)+offset);
		    set_size_block(info->holes, get_size_block(info->holes) - offset);
		    update_smm_mem_info(offset);
		    *logical_addr += offset;
		    send_data->p_error = 0;

		    return 0;
	       }else{

		    /* blocco nel mezzo */
		     rc = smmd_realloc(blk, send_data, logical_addr);

		     return rc;
	       }
	  } else {

	       /*hole adiacente*/
	       if (offset == get_size_block(new_hole)) {
		    set_size_block(blk,send_data->size);
		    info->holes = remove_block(info->holes, get_phy_start_addr(new_hole));
		    update_smm_mem_info(offset);

		    send_data->address = get_log_start_addr(blk);
		    send_data->p_error = 0;
		    
		    return 0;

	       } else if (offset > get_size_block(new_hole)) {

		     rc = smmd_realloc(blk, send_data, logical_addr);

		     return rc;
	       } else {
		    set_size_block(blk, send_data->size);
		    set_phy_start_addr(new_hole, get_phy_start_addr(new_hole)+offset);
		    set_size_block(new_hole, get_size_block(new_hole)-offset);
		    update_smm_mem_info(offset);
		    *logical_addr += offset;
		    send_data->p_error = 0;

		    return 0;
	       }
	  }
     }
}
