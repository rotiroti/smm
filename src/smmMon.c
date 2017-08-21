#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <limits.h>
#include <signal.h>
#include <sys/wait.h>
#include <setjmp.h>
#include <limits.h>

#include "utility.h"
#include "smm.h"


static int smm_lb(char *cmd, pid_t pid);

int status_server = 0;


/* Handlers per la gestione dei segnali */
static void 
sigcont_handler()
{
     fprintf(stdout, "smmd start: success\n");
     status_server = 1;

     return;
}


static void
sigusr1_handler()
{
     return;

}

static void
exit_handler()
{
     exit(EXIT_SUCCESS);
}




int 
main(void)
{
     int msg = 0;
     int option = 0;
     int rc = 0;
     int server_pid_fd = 0;	
     int status = 0;
     pid_t pid = 0;

     char buff_pid[6] = { 0 };
     char cmd[MAX] = { 0 };
     char *vett_cmd[LINES] = { NULL, NULL };

     command value;

     struct stat buf;
     struct smm_blk *blk = NULL;

     void *ptr = NULL; 
     
     
     /* settaggio dei segnali */
     signal(SIGCONT, sigcont_handler);
     signal(SIGINT, exit_handler);
     signal(SIGTERM, exit_handler);
     signal(SIGUSR1, sigusr1_handler);
     signal(SIGQUIT, exit_handler);

     
     while (1) {

	  printf("# ");
	  fgets(cmd, MAX, stdin);
	  cmd[strlen(cmd) - 1] = '\0';
	  
	  msg = tokenizza(cmd, vett_cmd);
	  
	  if (msg == -1) {
	       fprintf(stdout, "command not found: arguments list is too long\n");
	       reset_vett_cmd(vett_cmd);
	  }
 	  
	  if (vett_cmd[0] != NULL) {

	       switch(value = verifica_comando(vett_cmd)) {

	       case sm_start:
		    if (stat("/tmp/FIFOfromsmmd", &buf) == -1) { 

			 switch (pid = fork()) {

			 case -1:
			      perror("Fork failed");
			      break;

			 case 0:
			      if (vett_cmd[1] != NULL && (strcmp(vett_cmd[1], "-d")) == 0) {
				   execl("smmd", vett_cmd[1], (char *)0);
				   perror("Exec failed: ");
			      }
			      else {
				   execl("smmd", (char *)0, (char *)0);
				   perror("Exec failed: ");
			      }
			      break;
			 }

			 
			 while(status_server == 0) ;

			 status_server = 0;

		    } else {
			 fprintf(stdout, "smmd: is up\n");
		    }
		     
		    break;
		    
	       case sm_alloc:
		    option = atoi(vett_cmd[1]);
		    ptr = smm_malloc((size_t)option);
			
		    if(ptr != NULL) {
			 fprintf(stdout, "alloc: %#010X\n", (unsigned int) ptr);
		    } else {
			 fprintf(stdout, "alloc: failed\n");
		    }

		    break;
		    
	       case sm_free:
		
		    option = strtol(vett_cmd[1], NULL, 16);

		    if ((rc = smm_free((void *) option)) == 0) {
			 fprintf(stdout, "free[%#010X]: success\n", option);
		    }else {
			 fprintf(stdout, "free[%#010X]: failed\n", option);
		    }
		
		    break;
		    
	       case sm_check:
		    option = strtol(vett_cmd[1], NULL, 16);

		    if ((blk = (struct smm_blk *) calloc(1, sizeof(struct smm_blk))) != NULL) {
			 if ((rc = smm_chk_ptr((void *)option, blk)) == 0) {
			      fprintf(stdout, "check: success\n");
			 }
		    } else {
			 fprintf(stderr, "check: calloc failed\n");
		    }

		    fprintf(stdout, "%#010X %#010X %d\n", (unsigned int)blk->start, (unsigned int)blk->last, blk->swap);

		    break;
		    
	       case sm_lab:
		    option = 0;
		    
		    if (vett_cmd[1]!= NULL) {
			 option = atoi(vett_cmd[1]); 
		    }
		    smm_lb("lab", option);

		    break;
		    
	       case sm_lfb :
		    smm_lb("lfb", 0);

		    break;
		    
	       case sm_stop :
		    if ((server_pid_fd = open(FILEPID, O_RDONLY)) < 0){
			 printf("smmd: is down\n");

			 break;
		    }
		    rc = read(server_pid_fd,buff_pid,5);
		    
		    kill(atoi(buff_pid),SIGUSR1);

		    do {
			 waitpid(atoi(buff_pid), &status, WCONTINUED);


			 if (WIFEXITED(status)) {
			      printf("stop smmd: success\n");
			 }

		    } while (!WIFEXITED(status));
			 
		    break;
	       
	       case sm_exit:
		    reset_vett_cmd(vett_cmd);
		    exit(EXIT_SUCCESS);

		    break;
		    
	       case sm_not_found :
		    printf("command not found\n");
		    break;
	       
	       default:
		    break;
	       }
	       reset_vett_cmd(vett_cmd);
	  }
     }

     return 0;
}



static int
smm_lb(char *cmd,pid_t pid)
{
     int rc = 0;
     int fifo_to_fd = 0;
     int fifo_from_fd = 0;
     char *buff = NULL;
     packet data_send;


     open_fifos(fifo_to_fd, fifo_from_fd, -1);
     fill_packet(&data_send, 0, 0, 0, cmd);

     if (pid != 0) {
	  data_send.size_type = pid;
     }

     rc = lock_mutex(fifo_to_fd);
     rc = lock_mutex(fifo_from_fd);

     if( (rc = write_to(fifo_to_fd,&data_send,sizeof(data_send))) != 0) {
	  
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);

	  return -1;
     }

     if ((rc = read_from(fifo_from_fd, &data_send, sizeof(data_send))) != 0) {
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return -1;
     }

     if (data_send.p_error != 0) {
	  fprintf(stderr,"lb: %s\n", strerror(data_send.p_error));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return -1;
     }


     if (data_send.address != 0) {
	  fprintf(stdout, "%p %p %d\n", (void *)data_send.address,
     		 (void *) (data_send.address + data_send.size_type - 1),
     		 data_send.size_type);
     }

     if (data_send.size != 0) {

	  if ((buff = (char *) calloc((data_send.size+1), 1)) == NULL) {
     	       errno = ENOMEM;
     	       fprintf(stderr, "lb calloc: %s\n", strerror(errno));
     	       rc = unlock_mutex(fifo_to_fd);
     	       rc = unlock_mutex(fifo_from_fd);
     	       close(fifo_to_fd);
     	       close(fifo_from_fd);

     	       return -1;
	  }

     	  if ((rc = read_from(fifo_from_fd,buff,data_send.size)) != 0) {
     	       fprintf(stderr, "lb read: %s\n", strerror(rc));
     	       rc = unlock_mutex(fifo_to_fd);
     	       rc = unlock_mutex(fifo_from_fd);
     	       close(fifo_to_fd);
     	       close(fifo_from_fd);
     	       free(buff);
     	       buff = NULL;

     	       return -1;
     	  }

	  printf("%s", buff);
	  free(buff);
	  buff = NULL;
     }

     rc = unlock_mutex(fifo_to_fd);
     rc = unlock_mutex(fifo_from_fd);
     close(fifo_to_fd);
     close(fifo_from_fd);

     return 0;
}
