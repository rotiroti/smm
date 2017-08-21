#ifndef _SMM_MEM_H
#define _SMM_MEM_H

#include <sys/types.h>
#include "utility.h"

#define KB 1024
#define MB KB * KB

extern unsigned char *smmMem;

typedef struct elem_list elem_list;

/* \struct Struttura utilizzata da smmd per mantenere informazioni sullo stato di smmMem */
struct info_smm_mem
{
     size_t max_hole;		                  /* blocco libero con dim massima */
     size_t empty_space;                          /* spazio libero della memoria */
     struct elem_list *holes;                     /* lista dei blocchi liberi */
     struct elem_list *processes;                 /* lista dei blocchi allocati */
};

typedef struct info_smm_mem info_smm_mem;

extern info_smm_mem *info;


/**
 * Costruttore per il tipo di dato puntatore elem_list (blocco).
 *
 * \return nuovo elemento elem_list.
 * \return NULL, in caso di errore da parte della syscall malloc().
 */

elem_list *new_elem_list(void);


/**
 * Funzione che assegna l'indirizzo fisico al blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \param physical_addr, indirizzo fisico da assegnare.
 */

void set_phy_start_addr(elem_list *blk, unsigned int physical_addr);


/**
 * Funzione che assegna l'indirizzo logico al blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \param logical_address, indirizzo logico da assegnare.
 */

void set_log_start_addr(elem_list *blk, unsigned int logical_addr);


/**
 * Funzione che assegna la dimensione al blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \param size, dimensione da assegnare.
 */

void set_size_block(elem_list *blk, size_t size);


/**
 * Funzione che assegna il pid al blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \param pid, pid da assegnare.
 */

void set_pid(elem_list *blk, pid_t pid);


/**
 * Funzione che assegna il next (blocco successivo) di un blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \param next, (blocco successivo) da assegnare.
 */

void set_next(elem_list *blk, elem_list *next);


/**
 * Funzione che ritorna l'indirizzo fisico del blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \return indirizzo fisico.
 */

unsigned int get_phy_start_addr(elem_list *blk);


/**
 * Funzione che ritorna l'indirizzo logico del blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \return indirizzo logico.
 */

unsigned int get_log_start_addr(elem_list *blk);


/**
 * Funzione che ritorna la dimensione del blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \return dimensione del blocco.
 */

size_t get_size_block(elem_list *blk);


/**
 * Funzione che ritorna il pid del blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \return pid.
 */

pid_t get_pid(elem_list *blk);


/**
 * Funzione che ritorna il next (blocco successivo) del blocco.
 *
 * \pre blk != NULL.
 * \param blk, blocco considerato.
 * \return next (blocco successivo).
 */
elem_list * get_next(elem_list *blk);



/**
 * Funzione che inizializza le strutture utilizzate 
 * da smmd per gestire la memoria e crea le FIFO per
 * la comunicazione con i clients.
 * 
 * \return 0, in caso di successo.
 * \return errno, in caso di errore.
 */

int init_smm_mem_info();


/**
 * Funzione che aggiorna i campi empty_space e 
 * max_hole dell' handler di memoria (info).
 *
 * \param size, dimensione da aggiornare.
 */

void update_smm_mem_info(size_t size);


/**
 * Funzione per inserire un blocco in una lista ordinata
 * per indirizzi fisici. All'interno viene invocata la 
 * funzione check_physical_addr(...) per verificare che 
 * il blocco nuovo da inserire non vada a sovrascrivere
 * un blocco precedentemente allocato.
 *
 * \param head, testa della lista.
 * \param physical_addr, indirizzo fisico.
 * \param logical_addr, indirizzo logico.
 * \param pid, pid.
 * \param size, dimensione.
 * \return testa della lista aggiornata.
 * \return NULL, in caso di errore
 */
elem_list * insert_block(elem_list *head, 
			 unsigned int physical_addr, 
			 unsigned int logical_addr, 
			 pid_t pid, 
			 size_t size);


/**
 * Funzione che dato un indirizzo, ricerca il blocco a 
 * cui questo appartiene.
 *
 * \param head, testa della lista.
 * \param address, indirizzo da ricercare.
 * \return blk, contenente l'indirizzo.
 * \return NULL, in caso di insuccesso.
 */

elem_list * search_block(elem_list *head, unsigned int address);


/**
 * Funzione che ritorna il numero di blocchi appartenenti
 * ad una lista.
 *
 * \param head, testa della lista.
 * \return numero di blocchi.
 */

int how_many_blocks(elem_list *head);


/**
 * Funzione che data una lista ed un indirizzo fisico, 
 * ricerca e successivamente rimuove il blocco a cui 
 * quell'indirizzo fisico appartiene.
 *
 * \param head, testa della lista.
 * \param physical_addr, indirizzo fisico da ricercare.
 * \return testa della lista aggiornata.
 */

elem_list * remove_block(elem_list *head, unsigned int physical_addr);


/**
 * Funzione che elimina tutti i blocchi appartenenti
 * alla lista passata come parametro.
 *
 * \param head, testa della lista.
 * \return 0.
 */

int remove_all_blocks(elem_list **head_ref);


/**
 * Funzione usata a seguito del comando lab/lfb di smmMon, che
 * copia nel buff passato come parametro le informazioni contenute
 * nella lista. Questo viene fatto attraverso l'uso della funzione
 * snprintf(...). Nel caso del comando lfb la sintassi usata e' la 
 * seguente:
 * 
 * snprintf(buff, 40, "%p %p %d\n",phy_start, phy_last, size);
 *
 * nel caso del comando lab o lab [PID] la sintassi usata e' la
 * seguente:
 *
 * snprintf(buff, 40, "%d %p %p %d\n", PID, phy_start, phy_last, size).
 *
 * \pre buff != NULL.
 * \param head, testa della lista.
 * \param buff, buffer contenente i dati scritti.
 */

void info_smm_mem_blocks(elem_list *head, char *buff);


/**
 * Funzione che presa in input una lista e un pid, crea
 * una sottolista contenente il PID richiesto.
 *
 * \param head, testa della lista.
 * \param pid, pid richiesto.
 * \return sottolista, in caso di successo.
 * \return NULL, in caso di errore.
 */

elem_list * subset_allocated_blocks(elem_list *head, pid_t pid);


/**
 * Funzione che presa in input la lista dei blocchi liberi
 * (info->holes), ritorna il blocco di dimensione massima.
 *
 * \param head, testa della lista.
 * \return 0, se la lista e' vuota.
 * \return dimensione massima
 */

size_t search_max_hole(elem_list *head);


/**
 * Funzione che inserisce un blocco rimosso
 * dalla lista dei blocchi occupati (info>processes), nella
 * lista dei blocchi liberi (info->holes). L'inserimento puo'
 * avviene confrontando l'indirizzo fisico del blocco da inserire
 * con l'indirizzo fisico del blocco della lista preso in esame:
 *
 *  1) il blocco può essere inserito
 *  2) fuso con il suo successire 
 *  3) oppure fuso con il suo predecessore ed eventualmente anche
 *      con il suo successore.
 *    
 * \param head, testa della lista.
 * \param blk_fusion
 * \return testa della lista aggiornata.
 */

elem_list * fusion_blocks(elem_list *head, elem_list *blk_fusion);


/**
 * Funzione che permette di inserire un nuovo blocco nella
 * lista dei blocchi occupati: 
 * 
 * 1) invocazione della funzione insert_block() a cui, tra gli
 *    altri parametri viene passato il blocco calcolato attraverso
 *    l'invocazione della funzione best_fit().
 *
 * 2) viene ridimensionato o cancellato il blocco bestfit a seguito
 *    dell'inserimento.
 *
 * 3) viene calcolato il prossimo indirizzo logico da assegnare.
 *
 * \param best_fit, blocco "migliore"
 * \param size, dimensione del blocco da allocare
 * \param logical_addr, indirizzo logical_addr da assegnare al nuovo blocco
 * \param pid, pid.

 * \return testa della lista aggiornata.
 * \return < 0, in caso di errore.
 */

int allocator(elem_list *best_fit, size_t size, unsigned int *logical_addr, pid_t pid);


/**
 * Funzione che permette di compattare la lista dei blocchi 
 * occupati (info->processes): 
 *
 * 1) per ogni blocco in info->processes, viene decrementato
 *    l'indirizzo fisico di un offset opportunamente calcolato.
 *
 * 2) vengono spostati in smmMem, i dati relativi al blocco
 *    preso in esame.
 *
 * \return 0.
 */

int compact_smm_mem();


/**
 * Funzione che implementa l'algoritmo di selezione
 * dell'area (placement algorithm) di tipo best-fit.
 * La funzione prende in input la lista dei blocchi
 * liberi (info->holes) e ricerca il blocco piu'
 * piccolo >= di size. 
 * 
 * \param head, testa della lista.
 * \param size, dimensione da ricercare.
 * \return puntatore, al blocco trovato.
 * \return NULL, blocco non trovato.
 */

elem_list * best_fit(elem_list *head, size_t size);


/**
 * Funzione che presa in input una lista ed un 
 * indirizzo fisico, verifica se tale indirizzo
 * appartiene ad un range.
 *
 * \param head, testa della lista.
 * \param physical_addr, indirizzo da verificare.
 * \return 0, in caso di successo.
 * \return -1 , in caso di insuccesso.
 */

int check_physical_addr(elem_list *head, unsigned int physical_addr);

/**
 * Funzione che presa in input una lista ed un
 * indirizzo logico, verifica se tale indirizzo
 * appartiene ad un range.
 *
 * \param head, testa della lista.
 * \param logical_addr, indirizzo da verificare.
 * \return blocco contiene l'indirizzo logico.
 * \return NULL, in caso di insuccesso.
 */

elem_list *check_logical_addr(elem_list *head, unsigned int logical_addr);


/**
 * Funzione che mappa un indirizzo logico con un 
 * indirizzo fisico corrispondente.
 *
 * \param blk, blocco contenente l'indirizzo fisico.
 * \param logical_addr, indirizzo logico.
 * \return indirizzo fisico.
 */

unsigned int logical_to_physical_addr(elem_list *blk, unsigned int logical_addr);


/**
 * Funzione che crea un messaggio di errore
 * da scrivere nel file smmd.log del server.
 * 
 * \param msg, messaggio da scrivere.
 * \param func, nome della syscall da scrivere.
 * \param err, errore da scrivere.
 */
void log_msg_error(char msg[], char *func, int err);


/**
 * Funzione che scrive sul file smmd.log
 * il messaggio di errore, precedentemente
 * creato dalla funzione log_msg_error(...).
 *
 * \param fd_server_log, file descriptor di smmd.log
 * \param buff_error, messaggio di errore
 * \param syscall, parametro func di log_msg_error(...).
 * \param error, errore da settare.
 */
void write_to_log(int fd_server_log, char *buff_error, char *syscall, int error);


/**
 * Funzione che viene eseguita a seguito di una richiesta
 * di smm_malloc/smm_calloc:
 *
 * 1) verifica dello spazio disponibile per poter soddisfare
 *    la richiesta.
 *
 * 2) se la richiesta puo' essere soddisfatta, viene eseguito
 *    un ulteriore controllo per vericare se e' necessario
 *    eseguire compattazione.
 *
 * 3) viene chiamata la funzione allocator(...)
 *    
 * 4) nel caso in cui la richiesta si smm_calloc(), la memoria
 *    assegnata al processo viene inizializzata a 0.
 *
 * \param logical_address, indirizzo logico da assegnare.
 * \param send_data, richiesta.
 *
 * \return 0, in caso di successo.
 * \return -1, in caso di errore syscall malloc().
 */
int smmd_alloc(unsigned int *logical_address, packet *send_data);


/**
 * Funzione invocata a seguito di una richiesta di smm_realloc().
 * La permette di:
 *
 * 1) ridimensionare il blocco 'blk', aggiornando
 *    la sua dimensione alla nuova dimensione.
 *
 * 2) aggiornare la lista dei blocchi liberi, attraverso
 *    l'invocazione della funzione fusion().
 *
 * \param blk, blocco da ridimensionare
 * \param send_data, richiesta
 *
 * \return 0, in caso di successo.
 * \return -1, in caso di errore syscall malloc().
 */
int smmd_resize(elem_list *blk, packet *send_data);


/**
 * Funzione che permette di espandere il blocco 'blk'
 * a seguito di una richiesta di smm_realloc().
 * Il procedimento avviene nel seguente modo:
 * 
 * 1) ricerca della hole adiacente del blocco da
 *    riallocare.
 *
 * 2) se la hole trovata, soddisfa la richiesta:
 *
 *    2.a) vengono opportunamente aggiornate le 
 *         informazioni del blocco da riallocare (dimensione).
 *
 *    2.b) aggiornamento campi della hole o rimozione della
 *         hole.
 *
 * 3) se la hole trovata, non soddisfa la richiesta:
 *    se il blocco da riallocare e' l'ultimo blocco
 *    della lista dei blocchi occupati, viene effettuata
 *    compattazione e aggiornti i campi della e le 
 *    informazioni relative ad smmMem;
 *    altrimenti viene invocata la funzione smmd_realloc()
 *
 * \param blk, blocco da riallocare
 * \param send_data, richiesta.
 * \param logical_addr, indirizzo logico da assegnare
 *
 * \return 0, in caso di successo.
 * \return -1, in caso di errore syscall malloc().
 */
int smmd_expand(elem_list *blk, packet *send_data, unsigned int *logical_addr);


/**
 * Funzione che permette di spostare fisicamente il blocco 'blk'
 * da una zona di smmMem ad un' altra spostando opportunamente
 * anche i suoi dati.
 *
 * 1) viene effettuata una copia dei dati temporanea.
 *
 * 2) viene creata una copia del blocco da riallocare.
 *
 * 3) viene rimosso il blocco da riallocare.
 *
 * 4) viene inserito nuovamente il blocco
 *
 * 5) vengono ricopiati i dati in smmMem nella sua nuova
 *    locazione.
 * 
 * \param blk, blocco da riallocare.
 * \param send_data, richiesta.
 * \param logical_addr, indirizzo logico da assegnare.
 *
 * \return 0, in caso di successo.
 * \return -1, in caso di errore syscall malloc().
 */
int smmd_realloc(elem_list *blk, packet *send_data, unsigned int *logical_addr);


#endif	/* smmMem.h */
