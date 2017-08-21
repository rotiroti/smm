#ifndef _SMM_H
#define _SMM_H

#include <sys/types.h>

/* \struct contiene informazioni relative
 * al ptr passato nella smm_chk_ptr
 */

struct smm_blk
{
     void *start;		/* puntatore al primo byte del blocco */
     void *last;		/* puntatore all'ultimo byte del blocco */
     char swap;			/* swapped in=0, swapped out=0 */
};



/**
 * Funzione che alloca un blocco di memoria in smmMem di
 * dimensione pari a size byte.
 * La memoria non viene azzerata (wasted).
 *
 * \pre size > 0.
 *
 * \param size, numero elementi.
 *
 * \return puntatore all'inizio del blocco allocato.
 * \return NULL, se si e' verificato un errore.
 */

void *smm_malloc(size_t size);


/**
 * Funzione che alloca un blocco in smmMem per un array
 * di nmemb elementi di dimensione pari a size byute ognuno.
 * La memoria allocata e' inizializzata a 0.
 *
 * \pre nmemb >0 && size > 0.
 *
 * \param nmemb, numero elementi.
 * \param size, dimesione tipo dato.
 *
 * \return puntatore all'inizio del blocco.
 * \return NULL, se si e' verificato un errore.
 */

void *smm_calloc(size_t nmemb, size_t size);

/**
 * Funzione che cambia a 'size' byte la dimensione del blocco
 * di memoria gia' allocato in smmMem e puntato da 'ptr'.
 * Il contenuto rimane invariato per la porzione di memoria
 * di dimensione pari al minimo tra la vecchia e la nuova 
 * dimensione; eventuale memoria in aggiunta risultera' non
 * inizializzata. Se il blocco puntato richiede uno spostamento
 * (puntatore ritornato != da 'ptr'), la funzione garantisce la
 * liberazione del vecchio puntatore, ovvero, viene effettuata
 * una free('ptr').
 *
 * \pre size > 0
 * \pre ptr != NULL
 *
 * \param ptr, puntatore del blocco da rilocare
 * \param size, dimensione del blocco
 *
 * \return 'ptr' || un nuovo puntatore.
 * \return NULL, in caso di errore.
 */

void *smm_realloc(void *ptr, size_t size);


/**
 * Funzione che libera il blocco di memoria in smmMem 
 * precedentemente allocato e puntato dal puntatore 'ptr'.
 * 
 * \pre ptr != NULL.
 *
 * \param ptr, puntatore del blocco da liberare.
 * 
 * \return < 0 in caso di errore. 
 * \return 0 in caso di successo.
 */

int smm_free(void *ptr);


/**
 * Funzione che inserisce a partire dal puntatore 'ptr'
 * appartenente a un blocco di memoria precedentemente
 * allocato in smmMem un numero di bytes pari a 'size'
 * copiandoli dal buffer interno dell'applicazione 
 * puntato da 'buf'.
 *
 * \pre buf != NULL.
 * \pre ptr != NULL.
 * \pre size > 0.
 *
 * \param buf, dati da scrivere.
 * \param ptr, indirizzo del blocco.
 * \param size, numero dati da scrivere.
 *
 * \return 0, in caso di successo.
 * \return < 0, in caso di errore.
 */

int smm_put(void *buf, void *ptr, size_t size);


/**
 * Funzione che preleva a partire dal puntatore 'ptr'
 * appartenente a un blocco di memoria precedentemente
 * allocato in smmMem un numero di byte pari a 'size' e
 * li inserisce nel buffer interno all'applicazione
 * puntato da 'buf'.
 *
 * \pre ptr != NULL.
 * \pre buf != NULL.
 * \pre size > 0.
 *
 *
 * \param ptr, indirizzo del blocco. 
 * \param buff, dati da scrivere.
 * \param size, numero dati da scrivere.
 * 
 * \return 0, in caso di successo.
 * \return < 0, in caso di errore.
 */

int smm_get(void *ptr, void *buf, size_t size);


/**
 * Funzione che rileva le informazione al blocco a cui
 * appartiene ptr, riempiendo opportunamente i campi
 * della struttura pre-allocata dal chiamante (ex: smmMon).
 * Il campo swap della strttura viene impostato di default
 * a 0 (swapped in).
 * 
 * \pre ptr != NULL.
 * \pre blk != NULL.
 *
 * \param ptr, indirizzo da ricercare.
 * \param blk, blocco a cui appartiene ptr.
 *
 * \return 0, in caso di successo.
 * \return < 0, in caso di errore.
 */

int smm_chk_ptr(void *ptr, struct smm_blk *blk);


/**
 * Funzione utilizzata per richiedere il lock
 * sul file descriptor
 *
 * \param fd, file descriptor del file sul quale 
 *  richiedere il lock.
 *
 * \return 0, in caso di successo.
 * \return < 0, in caso di errore  
*/

int lock_mutex(int fd);

/**
 * Funzione utilizzata per rilasciare il lock
 * sul file descriptor
 *
 * \param fd, file descriptor del file da
 * rilasciare
 *   
 * \return 0, in caso di successo.
 * \return < 0, in caso di errore  
*/

int unlock_mutex(int);

#endif	/* smm.h */
