===========================================================
Test SOFTWARE per SMM=Simple Memory Manager
M. Verola

Versione 1.0
12/01/2009
===========================================================


Istruzioni:

1) Compilazione con make

Come precondizione, viene richiesto che il progetto SMM sia gia' compilato ed
installato nella directory $HOME/smm, cioe':
ls $HOME/smm
libsmm.a  smmd  smmMon

Lanciare il comando make.

Se si vuole eliminare il VERBOSE, che attiva una maggior numero di messaggi a
video durante l'esecuzione dei test, editare il Makefile e commentare la riga 
corrispondente.

2) Esecuzione di smm_test 

Lanciare il test completo:
smm_test

oppure lanciare i vari Test Group singolarmente (vedi documentazione sui test case):
smm_test LIB.AFOK
smm_test LIB.AFER
smm_test LIB.CPT
smm_test LIB.PG
smm_test LIB.CAL
smm_test LIB.REA
smm_test SMM.PER
smm_test SMM.PRO

3) Modifica del comportamento di smm_test

A) Delay tra l'esecuzione di un test case e il successivo
- nel file smm_test.h (linea 240) modificare la variabile TestSlowdown che
  indica quanti msec attendere (puo' essere messa anche a 0, cioe' nessun attesa)

B) Conferma prima di ogni test case
- per bloccarsi dopo ogni test case e aspettare un Enter di conferma,
  eliminare nel file smm_test.c il commento delle linee 704 e 705 (getchar()) 


Massimo Verola
