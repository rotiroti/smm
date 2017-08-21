#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

#include "utility.h"


int
tokenizza(char *comando,char *vett_cmd[])
{
     char *app = NULL;
     int i = 0;

     app = strtok(comando, " ");

     while (app != NULL) {
	  if (i >= 2)
	       return -1;

	  vett_cmd[i] = (char *) calloc(strlen(app)+1,sizeof(char));

	  if (!vett_cmd[i])
	       return -2;

	  vett_cmd[i] = strcpy(vett_cmd[i], app);
	  i++;
	  app = strtok(NULL , " ");
     }

     return 0;
}

void reset_vett_cmd(char *vett_cmd[])
{
     int i;

     for (i=0; i < 2; i++) {
	  if(vett_cmd[i] != NULL) {
	       free(vett_cmd[i]);
	       vett_cmd[i] = NULL;
	  }
     }
}

command verifica_comando(char *vett_cmd[])
{
     if(strcmp(vett_cmd[0],"start") == 0 && vett_cmd[1] == NULL)
	  return sm_start;

     else if(strcmp(vett_cmd[0],"start") == 0 && (strcmp(vett_cmd[1], "-d")) == 0)
	  return sm_start;

     else if (strcmp(vett_cmd[0],"stop") == 0 && vett_cmd[1] == NULL)
	  return sm_stop;

     else if (strcmp(vett_cmd[0],"exit") == 0 && vett_cmd[1] == NULL)
	  return sm_exit;

     else if (strcmp(vett_cmd[0],"alloc") == 0 && vett_cmd[1] != NULL && verifica_valore(vett_cmd) == 0)
	  return sm_alloc;

     else if (strcmp(vett_cmd[0],"free") == 0 && verifica_indirizzo_esadecimale(vett_cmd) == 0)
	  return sm_free;

     else if (strcmp(vett_cmd[0],"check") == 0 && verifica_indirizzo_esadecimale(vett_cmd) == 0)
	  return sm_check;

     else if (strcmp(vett_cmd[0],"lab") == 0 && (verifica_valore(vett_cmd) == 0 || vett_cmd[1] == NULL))
	  return sm_lab;

     else if (strcmp(vett_cmd[0],"lfb") == 0 && vett_cmd[1] == NULL)
	  return sm_lfb;

     else
	  return sm_not_found;
}

int verifica_valore(char *vett_cmd[])
{
     char *p = NULL;
     unsigned int i;

     if (vett_cmd[1] == NULL) return 0; 
     
     p = (char *)calloc(strlen(vett_cmd[1])+1,sizeof(char));

     if (p != NULL){
	  strcpy(p,vett_cmd[1]);
	  for (i = 0; i < strlen(p) ; i++) {

	       if(isdigit((int)p[i]) == 0) {
		    free (p);
		    p = NULL;
		    return -1;
	       }
	  }
     }

     free (p);
     p = NULL;

     return 0;
}


int verifica_indirizzo_esadecimale(char *vett_cmd[])
{
     if (vett_cmd[1] == NULL || strlen(vett_cmd[1]) != 10 ) {
	  /* il numero passato non rispetta le specifiche */
	  return -1;
     }

     if((vett_cmd[1][0] == '0') && (vett_cmd[1][1] == 'x')) {
	  size_t i;
	  for(i = 2; i < strlen(vett_cmd[1]); i++) {
	       if((isxdigit(vett_cmd[1][i])) == 0)
		    return -1;
	  }
     }

     return 0;
}


command verifica_comando_server(char *cmd)
{
    
     if(strcmp(cmd,"malloc") == 0 )
	  return sm_malloc;

     else if (strcmp(cmd,"calloc") == 0 )
	  return sm_calloc;
     
     else if (strcmp(cmd,"realloc") == 0 )
	  return sm_realloc;
     
     else if (strcmp(cmd,"stop") == 0 )
	  return sm_stop;

     else if (strcmp(cmd,"exit") == 0 )
	  return sm_exit;

     else if (strcmp(cmd,"free") == 0 )
	  return sm_free;

     else if (strcmp(cmd,"check") == 0 )
	  return sm_check;

     else if (strcmp(cmd,"lab") == 0 )
	  return sm_lab;

     else if (strcmp(cmd,"lfb") == 0 )
	  return sm_lfb;

     else if (strcmp(cmd,"put") == 0 )
	  return sm_put;
     
     else if (strcmp(cmd,"get") == 0 )
	  return sm_get;
     
     else
	  return sm_not_found;
}


/**
 * Funzione di inizializzazione dei campi
 * di un packet.
 */
void
fill_packet(packet *p,
	    size_t size,
	    size_t type,
	    void *address,
	    char *cmd)
{
     p->pid = getpid();
     p->size = size;
     p->size_type = type;
     p->p_error = 0;
     p->address = (unsigned int) address;
     strcpy(p->cmd, cmd);
}



/*Funzione permette di scrivere dati*/
/*
 *input bytes da scrivere
 *buffer in cui sono presenti i dati da scrivere
 *file descriptor su cui scirvere i dati
 *buff_error per scrivere eventuali errori sul file di log
 */
int write_to(int fd_fifo,const void *buff, int w_bytes){
     ssize_t b_write = 0;
     ssize_t to_write = w_bytes;
     ssize_t tot_write = 0;

     char *tmp_buff = (char *) buff;

     while(tot_write < w_bytes ){
	  if ((b_write = write(fd_fifo, tmp_buff, to_write)) < 0) {
	       return errno;
	  }

	  tot_write += b_write;
	  tmp_buff += b_write;
	  to_write -= b_write;
     }

     return 0;
}

/*funzione che permette di leggere i dati*/

int read_from(int fd_fifo, const void *buff,int r_bytes){
     ssize_t b_read = 0;
     ssize_t to_read = r_bytes;
     ssize_t tot_read = 0;
     char *tmp_buff = (char *) buff;

     while(tot_read < r_bytes ){
	  if(( b_read = read(fd_fifo, tmp_buff, to_read)) < 0){
	       return errno;
	  }
	  tot_read += b_read;
	  tmp_buff += b_read;
	  to_read -= b_read;
     }
     
     return 0;
}


/**
 * Apre una fifo
 */
int
open_fifo(const char *fifo_name)
{
     int fd =  0;

     if ((fd = open(fifo_name, O_RDWR)) == -1)
     {
	  /*Errore nell'apertura di una fifo*/
	  /*server off-line*/
	  fprintf(stderr, "smmd: %s\n", strerror(errno));
     }

     return fd;
}
