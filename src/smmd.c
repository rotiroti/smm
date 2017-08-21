#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <limits.h>

#include "utility.h"
#include "smmMem.h"


static int cmppid_tp(const void *p1, const void *p2);

static void exit_handler();

static void sigterm_handler();

static void kill_all(void);

static void sigterm_handler();


static int
cmppid_tp(const void *p1, const void *p2)
{
     pid_t pid1 = (pid_t) p1;
     pid_t pid2 = (pid_t) p2;

     if (pid1 < pid2)
	  return -1;
     else if (pid1 > pid1)
	  return 1;
     else
	  return 0;
}


static void 
exit_handler()
{
     /* clean-up delle informazioni relative alla memoria */
     if (info != NULL) {
	  remove_all_blocks(&(info->holes));
	  remove_all_blocks(&(info->processes));
	  free(info);
	  info = NULL;
     }

     /* clean-up di smmMem */
     if (smmMem != NULL) {
	  free(smmMem);
	  smmMem = NULL;
     }

     unlink(FIFOTO);
     unlink(FIFOFROM);
     unlink(FILEPID);
}



static void
sigterm_handler()
{
     kill_all();
     exit_handler();
     exit(EXIT_FAILURE);
}



void
sigusr1_handler()
{
     kill_all();
     exit_handler();
     exit(EXIT_SUCCESS);
}

unsigned char *smmMem = NULL;
info_smm_mem *info = NULL;


int main()
{
     int pid_server = 0;	
     int fd_server_pid = 0;
     int fd_server_log = 0;
     int fifo_to = 0;      
     int fifo_from = 0;    
     int n_read = 0;   
     int w_bytes = 0;
     int rc = 0; 

     char server[8];
     char buff[256] = {0};
     char *buff_addr = NULL;

     packet send_data;

     command value;

     unsigned int logical_address = 1; 
     unsigned int tmp_phy_addr = 0;

     elem_list *ptr_blk = NULL;	
     elem_list *ptr2_blk = NULL;

     /* setto i segnali che devo gestire */
     signal(SIGTERM, sigterm_handler);
     signal(SIGQUIT, sigterm_handler);
     signal(SIGINT, sigterm_handler);
     signal(SIGUSR1, sigusr1_handler);


     if ((fd_server_log = open("/tmp/smmd.log", O_CREAT | O_WRONLY | O_APPEND, 0644)) == -1) {
	  sigterm_handler();

	  return -1;
     }

     /*creazione del file in cui il server salva i proprio PID*/
     if ((fd_server_pid = open(FILEPID, O_CREAT | O_WRONLY, 0644)) == -1) {
	  sigterm_handler();

	  return -1;
     }

     pid_server = getpid();
     snprintf(server, 8, "%d\n", pid_server);
     
     if ((rc = write_to(fd_server_pid,server,strlen(server))) < 0) {
	  write_to_log(fd_server_log, buff, "write", rc);
	  sigterm_handler();

	  return -1;
     }

     close(fd_server_pid);


     if ((smmMem = (unsigned char *) calloc (16*MB, sizeof(char))) == NULL) {
	  errno = ENOMEM;
	  write_to_log(fd_server_log, buff, "smmMem", errno);
	  close(fd_server_log);
	  sigterm_handler();

	  return -1;
     }

     if ((errno = init_smm_mem_info()) != 0) {
	  if (errno == ENOMEM) {
	       write_to_log(fd_server_log, buff, "init_smm_mem_info", errno);
	  }
	  else {
	       write_to_log(fd_server_log, buff, "init_smm_mem_info", errno);
	  }
	  sigterm_handler();

	  return -1;
     }

     /* Comunico a smmMon che è andato tutto bene tramite le FIFO */
     kill(getppid(), SIGCONT);

     if ((fifo_to = open(FIFOTO, O_RDWR)) == -1) {
	  write_to_log(fd_server_log, buff, "open FIFOtosmmd", errno);
	  sigterm_handler();

     	  return -1;
     }

     if ((fifo_from = open(FIFOFROM, O_RDWR)) == -1) {
	  write_to_log(fd_server_log, buff, "open FIFOfromsmmd", errno);
	  sigterm_handler();

     	  return -1;
     }


     while(1)
     {
	  if ((n_read = read(fifo_to, &send_data, sizeof(send_data))) != sizeof(send_data)) {
	       strcpy(send_data.cmd,"invalid");
	  }


	  if (logical_address > 2147483647) {
	       logical_address = 1;
	  }


	  /*  switch per le richieste */
	  switch (value = verifica_comando_server(send_data.cmd))
	  {
	  case sm_malloc:

	       if ((rc = smmd_alloc(&logical_address, &send_data)) < 0) {
		    write_to_log(fd_server_log, buff, "malloc", rc);
		    sigterm_handler();
	       }

	       if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
		    write_to_log(fd_server_log, buff, "write", rc);
		    sigterm_handler();
	       }

      	       break;

	  case sm_calloc:

	       if ((rc = smmd_alloc(&logical_address, &send_data)) < 0) {
		    write_to_log(fd_server_log, buff, "malloc", rc);
		    sigterm_handler();
	       }

	       if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
	       	    write_to_log(fd_server_log, buff, "write", rc);
		    sigterm_handler();
	       }

	       break;

	  case sm_realloc:
	       ptr_blk = search_block(info->processes, send_data.address);
	       
	       if (ptr_blk != NULL && get_pid(ptr_blk) == send_data.pid) {
		    
		    if (get_size_block(ptr_blk) > send_data.size){
			 
			 /* ridimensionamento */
			 rc = smmd_resize(ptr_blk,&send_data);
		    }else if (get_size_block(ptr_blk) < send_data.size ) {
			 /* espando */
			 rc = smmd_expand(ptr_blk, &send_data, &logical_address);
		    } 
		    
		    if (rc ==  -1) {
			 write_to_log(fd_server_log, buff, "malloc", ENOMEM);
			 sigterm_handler();
		    }
	       } else {
		    send_data.p_error = EPERM;
		    ptr_blk = NULL;
	       }
	       
	       if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
		    write_to_log(fd_server_log, buff, "write", rc);
		    sigterm_handler();
	       }
	       
	       break;

	  case sm_free:

	       ptr_blk = new_elem_list();
	       ptr2_blk = new_elem_list();

	       if (ptr_blk == NULL || ptr2_blk == NULL) {
		    write_to_log(fd_server_log, buff, "sm_free\n", rc);
		    sigterm_handler();
	       }

	       ptr_blk = search_block(info->processes, send_data.address);

	       /* verifica permessi per effettuare deallocazione */
	       if (ptr_blk != NULL && (get_pid(ptr_blk) == send_data.pid)) {
		    set_pid(ptr2_blk, 0);
		    set_phy_start_addr(ptr2_blk, get_phy_start_addr(ptr_blk));
		    set_log_start_addr(ptr2_blk, 0);
		    set_size_block(ptr2_blk, get_size_block(ptr_blk));
		    set_next(ptr2_blk, NULL);
		    
		    info->processes = remove_block(info->processes, get_phy_start_addr(ptr_blk));
		    info->empty_space += get_size_block(ptr2_blk);
		    info->holes = fusion_blocks(info->holes, ptr2_blk);
		    info->max_hole = search_max_hole(info->holes);

		    send_data.p_error = 0;
	       }
	       else {
		    send_data.p_error = EPERM;
	       }

	       if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
		    write_to_log(fd_server_log, buff, "write", errno);
		    sigterm_handler();
	       }

	       ptr_blk = NULL;
	       ptr2_blk = NULL;

	       break;

	  case sm_put:

	       if (((ptr_blk = check_logical_addr(info->processes, send_data.address)) == NULL) ||
		   (send_data.pid != get_pid(ptr_blk))) {

		    send_data.p_error = EINVAL;

		    if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
			 write_to_log(fd_server_log, buff, "write", rc);
			 sigterm_handler();
		    }
		    ptr_blk = NULL;

		    break;
	       }

	       tmp_phy_addr = logical_to_physical_addr(ptr_blk, send_data.address);
	       
	       if ((buff_addr = (char *) calloc(send_data.size, 1)) == NULL) {
		    errno = ENOMEM;	 
		    write_to_log(fd_server_log, buff, "sm_put\n", errno);
		    sigterm_handler();
	       }

	       if ((rc = read_from(fifo_to, buff_addr, send_data.size)) != 0) {
		    write_to_log(fd_server_log, buff, "write", rc);
		    free(buff_addr);
		    buff_addr = NULL;
		    ptr_blk = NULL;
		    sigterm_handler();
	       }

	       if ((tmp_phy_addr + send_data.size - 1) <= (get_phy_start_addr(ptr_blk) + get_size_block(ptr_blk) - 1)) {
		    memmove(smmMem + tmp_phy_addr, buff_addr, send_data.size);
		    send_data.p_error = 0;
	       }else{
		    send_data.p_error = ENOMEM;
	       }

	       if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
		    write_to_log(fd_server_log, buff, "write", rc);
		    sigterm_handler();
	       }

	       free(buff_addr);
	       buff_addr = NULL;
	       ptr_blk = NULL;

	       break;

	  case sm_get:
	       if (((ptr_blk = check_logical_addr(info->processes, send_data.address)) == NULL) ||
		   (send_data.pid != get_pid(ptr_blk))) {
		    
		    send_data.p_error = EINVAL;
		    
		    if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
			 write_to_log(fd_server_log, buff, "write", rc);
			 sigterm_handler();
		    }
		    ptr_blk = NULL;
		    break;
	       }

	       tmp_phy_addr = logical_to_physical_addr(ptr_blk, send_data.address);
	       
	       if ((buff_addr = (char *) calloc(send_data.size, 1)) == NULL){
		    errno = ENOMEM;	 
		    write_to_log(fd_server_log, buff, "sm_get\n", errno);
		    sigterm_handler();
	       }

	       if ((tmp_phy_addr + send_data.size - 1) <= (get_phy_start_addr(ptr_blk) + get_size_block(ptr_blk) - 1)) {
		    memmove(buff_addr, smmMem + tmp_phy_addr, send_data.size);
		    send_data.p_error = 0;
	       }else{
		    send_data.p_error = ENOMEM;
	       }

	       if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
		    write_to_log(fd_server_log, buff, "write", rc);
		    sigterm_handler();
	       }

	       if ((rc = write_to(fifo_from, buff_addr, send_data.size)) != 0) {
		    write_to_log(fd_server_log, buff, "write", rc);
		    sigterm_handler();
	       }

	       free(buff_addr);
	       buff_addr = NULL;
	       ptr_blk = NULL;

	       break;

	  case sm_check:
	       if (((ptr_blk = check_logical_addr(info->processes, send_data.address)) == NULL) ||
		   (send_data.pid != get_pid(ptr_blk))) {

		    send_data.p_error = EINVAL;
		    
		    if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
			 write_to_log(fd_server_log, buff, "write", rc);
			 sigterm_handler();
		    }

		    ptr_blk = NULL;

		    break;
	       }

	       send_data.address = get_log_start_addr(ptr_blk);
	       send_data.size = get_size_block(ptr_blk);
	       send_data.p_error = 0;

	       if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
		    write_to_log(fd_server_log, buff, "write", rc);
		    sigterm_handler();
	       }

	       ptr_blk = NULL;

	       break;

	  case sm_lab:
	       if (send_data.size_type != 0) {

		    /*creo la sottolista che devo stampare*/
		    ptr_blk = subset_allocated_blocks(info->processes, send_data.size_type);
		    
		    if(ptr_blk != NULL) {
			 if ((buff_addr = (char *) calloc(40 * how_many_blocks(ptr_blk), 1)) == NULL){
			      errno = ENOMEM;	 
			      write_to_log(fd_server_log, buff, "sm_lab", errno);
			      sigterm_handler();			      
			 }
			 info_smm_mem_blocks(ptr_blk, buff_addr);
			 send_data.size = strlen(buff_addr);
			 w_bytes = sizeof(send_data);

			 if ((rc = write_to(fifo_from, &send_data, w_bytes)) == 0) {
			      w_bytes = strlen(buff_addr);
		    
			      if((rc = write_to(fifo_from, buff_addr, w_bytes)) != 0) {
				   write_to_log(fd_server_log, buff, "write", errno);
				   sigterm_handler();
			      }
			 }
			 else {
			      write_to_log(fd_server_log, buff,"write", errno);
			      sigterm_handler();
			 }
			 free(buff_addr);
			 buff_addr = NULL;
			 ptr_blk = NULL;

			 break ;
		    } else {
			 send_data.size = 0;
			 send_data.p_error = EINVAL;
			 
			 if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
			      write_to_log(fd_server_log, buff, "write", errno);
			      sigterm_handler();
			 }
			 ptr_blk = NULL;

			 break;
		    }
	       } else {
		    send_data.address = (unsigned int) smmMem;
		    send_data.size_type = 16*MB;
		    send_data.p_error = 0;

		    if (info->processes != NULL) {
			 if ((buff_addr = (char *) calloc((40* how_many_blocks(info->processes)), 1)) 
			     == NULL) {
			      errno = ENOMEM;	 
			      write_to_log(fd_server_log, buff, "sm_lab", errno);
			      sigterm_handler();
			 }

			 info_smm_mem_blocks(info->processes, buff_addr);

			 send_data.size = strlen(buff_addr);
			 w_bytes = sizeof(send_data);

			 if ((rc = write_to(fifo_from, &send_data, w_bytes)) == 0) {
			      w_bytes = strlen(buff_addr);
		    
			      if ((rc = write_to(fifo_from, buff_addr, w_bytes)) != 0) {
				   write_to_log(fd_server_log, buff, "write", errno);
				   sigterm_handler();
			      }
			      free(buff_addr);
			      buff_addr = NULL;
			      ptr_blk = NULL;
			 }
			 else {
			      write_to_log(fd_server_log, buff,"write", errno);
			      sigterm_handler();
			 }
		    } else {
			 send_data.size = 0;

			 if ((rc = write_to(fifo_from, &send_data, sizeof(send_data))) != 0) {
			      write_to_log(fd_server_log, buff,"write", errno);
			      sigterm_handler();
			 }
		    }
	       }
		    
	       break;

	  case sm_lfb:
	       if ((buff_addr = (char *) calloc((40 * how_many_blocks(info->holes)), 1)) == NULL) {
		    errno = ENOMEM;	 
		    write_to_log(fd_server_log, buff, "sm_lfb", errno);
		    sigterm_handler();
	       }
	       else {
		    info_smm_mem_blocks(info->holes, buff_addr);
		    send_data.size = strlen(buff_addr);
		    w_bytes = sizeof(send_data);
		 
		    if (send_data.size != 0) {
			 if ((rc = write_to(fifo_from, &send_data, w_bytes)) == 0) {
			      w_bytes = strlen(buff_addr);

			      if ((rc = write_to(fifo_from,buff_addr,w_bytes)) != 0) {
				   write_to_log(fd_server_log, buff, "write", errno);
				   sigterm_handler();
			      }
			 }
			 else {
			      write_to_log(fd_server_log, buff," write", errno);
			      sigterm_handler();
			 }
		    } else {
			 if ((rc = write_to(fifo_from,&send_data,sizeof(send_data))) != 0 ) {
			      write_to_log(fd_server_log, buff," write", errno);
			      sigterm_handler();
			 }
		    }
	       }

	       free(buff_addr);
	       buff_addr = NULL;

	       break;

	  default: break;
	  }
     }
     return 0;
}



static void
kill_all(void)
{
     int i = 0;
     int nmemb = 0;
     elem_list *head = info->processes;
     pid_t *pids = NULL;
     char proc[14] = {0};
     struct stat buff;


     nmemb = how_many_blocks(info->processes);

     if ((pids = (pid_t *) malloc(nmemb * sizeof(pid_t))) == NULL) {
	  exit_handler();
	  exit(EXIT_FAILURE);
     }
     
     while (head != NULL) {
	  pids[i] = get_pid(head);
	  ++i;
	  head = get_next(head);
     }

     qsort(pids, nmemb, sizeof(pid_t), cmppid_tp);

     for (i = 0; i < nmemb; ++i) {
	  if (i == 0) {
	       snprintf(proc, 14, "/proc/%d", pids[i]);

	       if (stat(proc, &buff) == 0) {
		    kill(pids[i], SIGUSR1);
		    
	       }
	  } else {
	       if (pids[i] != pids[i+1]) {
		    snprintf(proc, 14, "/proc/%d", pids[i]);

		    if (stat(proc, &buff) == 0) {
			 kill(pids[i], SIGUSR1);

		    }
	       }
	  }
     }
     free (pids);
     pids = NULL;

}
