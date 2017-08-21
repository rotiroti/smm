#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#include "utility.h"
#include "smm.h"



void *
smm_malloc(size_t size)
{
     int fifo_from_fd = 0;
     int fifo_to_fd = 0;
     int rc = 0;
     packet data_send;


     /* dimensione size errata */
     if (size == 0 || size > 16777216) { 
	  errno = EINVAL;
	  fprintf(stderr, "%s\n", strerror(errno));
	  
	  return NULL;
     }

     open_fifos(fifo_to_fd, fifo_from_fd, NULL);
     fill_packet(&data_send, size, 0, 0, "malloc");
     rc = lock_mutex(fifo_to_fd);
     rc = lock_mutex(fifo_from_fd);

     if ((rc = write_to(fifo_to_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);

	  return NULL;
     }

     if((rc = read_from(fifo_from_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_from_fd);
	  close(fifo_to_fd);

	  return NULL;
     }

     rc = unlock_mutex(fifo_to_fd);
     rc = unlock_mutex(fifo_from_fd);
     close(fifo_to_fd);
     close(fifo_from_fd);
     
     /* controllo eventuali errori nel pacchetto */
     if(data_send.p_error != 0) {
	  errno = data_send.p_error;
	  fprintf(stderr, "smm_malloc: %s\n",strerror(errno));

	  return NULL;
     }

     return (void *) data_send.address;
}


void *
smm_calloc(size_t nmemb, size_t size)
{
     int fifo_from_fd = 0;
     int fifo_to_fd = 0;
     int rc = 0;
     packet data_send;

     
     /* dimensione errata */
     if (((nmemb*size) == 0) || ((nmemb*size) > 16777216)) {
	  errno = EINVAL;
	  fprintf(stderr,"%s\n", strerror(errno));
	  
	  return NULL;
     }

     open_fifos(fifo_to_fd, fifo_from_fd, NULL);
     fill_packet(&data_send, nmemb, size, 0, "calloc");

     rc = lock_mutex(fifo_to_fd);
     rc = lock_mutex(fifo_from_fd);

     if ((rc = write_to(fifo_to_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid,strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);

	  return NULL;
     }

     if ((rc = read_from(fifo_from_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid,strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_from_fd);
	  close(fifo_to_fd);
	  
	  return NULL;
     }

     rc = unlock_mutex(fifo_to_fd);
     rc = unlock_mutex(fifo_from_fd);
     close(fifo_to_fd);
     close(fifo_from_fd);
     
     /* controllo eventuali errori nel pacchetto */
     if(data_send.p_error != 0) {
	  errno = data_send.p_error;
	  fprintf(stderr, "smmd: %s\n",strerror(errno));

	  return NULL;
     }

     return (void *) data_send.address;
}


void *
smm_realloc(void *ptr, size_t size)
{
     int fifo_from_fd = 0;
     int fifo_to_fd = 0;
     int rc = 0;     
     packet data_send;

     
     if (ptr == NULL || size == 0 || size > 16777216) {
	  errno = EINVAL;
	  fprintf(stderr, "smm_realloc:%d: %s\n", (int)__LINE__, strerror(errno));
	  
	  return NULL;
     }

     open_fifos(fifo_to_fd, fifo_from_fd, NULL);
     fill_packet(&data_send, size, 0, ptr, "realloc");
     rc = lock_mutex(fifo_to_fd);
     rc = lock_mutex(fifo_from_fd);

     if ((rc = write_to(fifo_to_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] write:%d: %s\n", data_send.pid, (int)__LINE__, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return NULL;
     }

     if ((rc = read_from(fifo_from_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] read:%d: %s\n", data_send.pid, (int)__LINE__, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return NULL;
     }

     rc = unlock_mutex(fifo_to_fd);
     rc = unlock_mutex(fifo_from_fd);
     close(fifo_to_fd);
     close(fifo_from_fd);

     if(data_send.p_error != 0 || data_send.size == 0) {
	  fprintf(stderr, "[%d] smm_realloc:%d %s\n", data_send.pid,
		  (int)__LINE__, strerror(data_send.p_error));
	  
	  return NULL;
     }
     
     return (void *) data_send.address;
}


int
smm_free(void *ptr)
{
     int fifo_from_fd = 0;
     int fifo_to_fd = 0;
     int rc = 0;
     packet data_send;


     /* puntatore non valido */
     if (ptr == NULL) {	
	  errno = EINVAL;
	  fprintf(stderr,"%s\n", strerror(errno));

	  return -1;
     }

     open_fifos(fifo_to_fd, fifo_from_fd, -1);
     fill_packet(&data_send, 0, 0, ptr, "free");
     rc = lock_mutex(fifo_to_fd);
     rc = lock_mutex(fifo_from_fd);


     if ((rc = write_to(fifo_to_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid,strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);

	  return -1;
     }
     
     if ((rc = read_from(fifo_from_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid,strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_from_fd);
	  close(fifo_to_fd);

	  return -1;
     }


     rc = unlock_mutex(fifo_to_fd);
     rc = unlock_mutex(fifo_from_fd);
     close(fifo_to_fd);
     close(fifo_from_fd);

     /* controllo eventuali errori nel pacchetto */
     if(data_send.p_error != 0) {
	  errno = data_send.p_error;
	  fprintf(stderr, "smmd:%d: %s\n", (int) __LINE__, strerror(errno));

	  return -1;
     }

     return 0;
}



int
smm_put(void *buf, void *ptr, size_t size)
{
     int fifo_from_fd = 0;
     int fifo_to_fd = 0;
     int rc = 0;
     packet data_send;

     
     if (buf == NULL || ptr == NULL || size < 1) {
	  errno = EINVAL;
	  fprintf(stderr,"%s\n", strerror(errno));
	  
	  return -1;
     }

     open_fifos(fifo_to_fd, fifo_from_fd, -1);
     fill_packet(&data_send, size, 0, ptr, "put");
     rc = lock_mutex(fifo_to_fd);
     rc = lock_mutex(fifo_from_fd);

     if ((rc = write_to(fifo_to_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);

	  return -1;
     }


     if ((rc = write_to(fifo_to_fd, buf, size)) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);

	  return -1;
     }

     if ((rc = read_from(fifo_from_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return -1;
     }

     rc = unlock_mutex(fifo_to_fd);
     rc = unlock_mutex(fifo_from_fd);
     close(fifo_to_fd);
     close(fifo_from_fd);


     if (data_send.p_error != 0) {
	  errno = data_send.p_error;
	  fprintf(stderr, "smm:%d: %s\n", (int) __LINE__, strerror(errno));

	  return -1;
     }

     return 0;
}



int
smm_get(void *ptr, void *buf, size_t size)
{
     int fifo_from_fd = 0;
     int fifo_to_fd = 0;
     int rc = 0;
     packet data_send;


     if (buf == NULL || ptr == NULL || size < 1) {
	  errno = EINVAL;
	  fprintf(stderr,"%s\n", strerror(errno));
	  
	  return -1;
     }

     open_fifos(fifo_to_fd, fifo_from_fd, -1);
     fill_packet(&data_send, size, 0, ptr, "get");

     rc = lock_mutex(fifo_to_fd);
     rc = lock_mutex(fifo_from_fd);

     if ((rc = write_to(fifo_to_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return -1;
     }

     if ((rc = read_from(fifo_from_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return -1;
     }

     if (data_send.p_error == EINVAL) {
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);

	  return -1;
     }

     if ((rc = read_from(fifo_from_fd, buf, size)) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return -1;
     }

     rc = unlock_mutex(fifo_to_fd);
     rc = unlock_mutex(fifo_from_fd);
     close(fifo_to_fd);
     close(fifo_from_fd);

     if (data_send.p_error != 0) {
	  errno = data_send.p_error;
	  fprintf(stderr, "smm:%d: %s\n", (int) __LINE__, strerror(errno));
	  
	  return -1;
     }

     return 0;
}


int
smm_chk_ptr(void *ptr, struct smm_blk *blk)
{
     int fifo_from_fd = 0;
     int fifo_to_fd = 0;
     int rc = 0;
     packet data_send;


     /* verifica dei parametri in ingresso */
     if ((ptr == NULL) || (blk == NULL)) {

	  errno = EINVAL;
	  fprintf(stderr,"%s\n", strerror(errno));
	  
	  return -1;
     }


     fill_packet(&data_send, 0, 0, ptr, "check");
     open_fifos(fifo_to_fd, fifo_from_fd, -1);

     rc = lock_mutex(fifo_to_fd);
     rc = lock_mutex(fifo_from_fd);

     if((rc = write_to(fifo_to_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);

	  return -1;
     }

     if((rc = read_from(fifo_from_fd, &data_send, sizeof(data_send))) != 0) {
	  fprintf(stderr, "[%d] FIFO: %s\n", data_send.pid, strerror(rc));
	  rc = unlock_mutex(fifo_to_fd);
	  rc = unlock_mutex(fifo_from_fd);
	  close(fifo_to_fd);
	  close(fifo_from_fd);
	  
	  return -1;
     }

     rc = unlock_mutex(fifo_to_fd);
     rc = unlock_mutex(fifo_from_fd);
     close(fifo_to_fd);
     close(fifo_from_fd);

     if(data_send.p_error != 0) {
	  errno = data_send.p_error;
	  fprintf(stderr, "smmd: %s\n", strerror(errno));
	  
	  return -1;
     }

     blk->start = (void *) (data_send.address);
     blk->last = (void *) (data_send.address + data_send.size - 1);

     return 0;
}


int
lock_mutex(int fd)
{
     struct flock lock;

     lock.l_type = F_WRLCK;	              /* tipo di lock: read or write */
     lock.l_whence = SEEK_SET;		      /* esecuzione dall'inizio del file */
     lock.l_start = 0;			      /* impostazione dell'inizio del lock */
     lock.l_len = 0;			      /* impostazione della lunghezza del lock */

     return fcntl(fd, F_SETLKW, &lock);       /* esecuzione del lock */
}


int
unlock_mutex(int fd)
{
    struct flock lock;

    lock.l_type = F_UNLCK;	              /* tipo di lock: unlock */
    lock.l_whence = SEEK_SET;		      /* esecuzione dall'inizio del file */
    lock.l_start = 0;			      /* impostazione dell'inizio del lock */
    lock.l_len = 0;			      /* impostazione della lunghezza del lock */
    
    return fcntl(fd, F_SETLK, &lock);         /* esecuzione del lock */
}
