#ifndef _UTILITY_H
#define _UTILITY_H

#include <sys/types.h>

#define MAX 256
#define LINES 2

#define FIFOTO		"/tmp/FIFOtosmmd"
#define FIFOFROM	"/tmp/FIFOfromsmmd"
#define FILEPID         "/tmp/smmd.pid"
#define FILEPIDSHELL    "/tmp/smmMon.pid"


/* \enum elenco di comandi */
enum command {
     sm_start,
     sm_stop,
     sm_exit,
     sm_alloc,
     sm_free,
     sm_check,
     sm_lab,
     sm_lfb,
     sm_not_found,
     sm_malloc,
     sm_calloc,
     sm_realloc,
     sm_put,
     sm_get
};

typedef enum command command;

/* \struct pacchetto per lo scambio
 * di informazioni tra smmd e smmMon/client.        
 */
struct packet
{
     int p_error;		/* variabile di errore */
     pid_t pid;			        /* pid del processo */
     size_t size;			/* dimensione */
     size_t size_type;			/* dimensione del tipo di dato */
     unsigned int address;		/* indirizzo logico */
     char cmd[10];			/* comando */
};

typedef struct packet packet;



/**
 * Funzione che permette attraverso ripetute
 * chiamate di strtok() di suddividere una stringa
 * in token e di memorizzarli in un vettore di 
 * puntatori di stringhe.
 *
 * \param command, comando da analizzare. 
 * \param array_cmd, array di stringhe.
 *
 * \return <0, in caso di errore.
 * \return 0, in caso di successo.
 */

int tokenizza(char *command, char *array_cmd[]);


/**
 * Funzione che permette di deallocare il
 * vettore di puntatori di stringhe.
 *
 * \param array_cmd, array di stringhe 
 */

void reset_vett_cmd(char *array_cmd[]);


/**
 * Funzione che verifica la correttezza di
 * un comando (lato smmMon).
 * 
 * \param array_cmd, array di stringe.
 *
 * \return command, comando ritornato.
 */

command verifica_comando(char *array_cmd[]);


/**
 * Funzione che verifica se una stringa
 * passata come argometo è composta da
 * soli numeri.
 *
 * \param array_cmd, array di stringhe.
 *
 * \return 0, in caso di successo.
 * \return < 0, in caso di errore.
 */

int verifica_valore(char *array_cmd[]);


/**
 * Funzione che verifica se una stringa
 * è in formato esadecimale.
 * 
 * \param array_cmd, array di stringhe.
 * \return 0, in caso di successo.
 * \return <0, in caso di errore.
 */

int verifica_indirizzo_esadecimale(char *array_cmd[]);


/**
 * Funzione che verifica la correttezza di
 * un comando (lato smmd).
 * 
 * \param cmd, stringa del comando.
 *
 * \return command, comando ritornato.
 */

command verifica_comando_server(char *cmd);


/**
 * Funzione che permette di inizializzare
 * i campi di un packet.
 *
 * \param p, puntatore al packet.
 * \param size, dimensione.
 * \param type, dimensione del tipo di dato.
 * \param addrres, indirizzo.
 * \param cmd, comando da eseguire.
 */

void fill_packet(packet *p, size_t size, size_t type, void *address, char *cmd);


/**
 * Wrapper della syscall write.
 *
 * param fd_fifo, file descriptor.
 * \param buff, dati da scrivere.
 * \param w_bytes, numero dei dati da scrivere.
 *
 * \return 0, in caso di successo.
 * \return errno, in caso di errore
 */

int write_to(int fd_fifo, const void *buff, int w_bytes);


/**
 * Wrapper della syscall read.
 *
 * \param fd_fifo, file descriptor.
 * \param buff, dati letti.
 * \param r_bytes, numero dei dati letti.
 *
 * \return 0, in caso di successo.
 * \return errno, in caso di errore
 */

int read_from(int fd_fifo, const void *buff, int r_bytes);


/**
 * Funzione che permette di aprire una fifo.
 *
 * \param fifo_name, path.
 *
 * \return 0, in caso di successo.
 * \return -1, in caso di errore.
 */

int open_fifo(const char *fifo_name);


/* \def open_fifos wrapper per l'apertura
 * delle fifo.
  */
#define open_fifos(fdto, fdfrom, ret)		\
     do {					\
	  fdto = open_fifo(FIFOTO);		\
	  fdfrom = open_fifo(FIFOFROM);		\
	  					\
	  if((fdto < 0) || (fdfrom < 0)) {	\
	       return (ret);			\
	  }					\
     } while(0)


#endif	/* utility.h */
