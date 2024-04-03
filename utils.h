#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define bool int
#define true 1
#define false 0

// struttura rappresentante il nodo di una lista
// val: stringa rappresentante una riga del file di output
// next: puntatore al prossimo nodo della lista rappresentante la riga successiva del file di output
struct nodoLista{
    char * val;
    struct nodoLista * next;

};
typedef struct nodoLista nodo;

// struttura rappresentante una una pagina del file di output
// inizio: puntatore al nodo di inizio della lista
// fine: puntatore al nodo di fine della lista
struct unaPagina{
    nodo * inizio;
    nodo * fine;
};
typedef struct unaPagina pagina;

// trasforma una stringa rappresentante un numero in un intero
int new_atoi(char n[]);

// aggiunge un nodo alla fine della lista della pagina
void * push(pagina * p, nodo * elem);

// restituisce il minimo tra i due parametri passati in ingresso
int min(int x,int y);

// alloca una nuova area di memoria di dimensione len, copiando ciò che è presente
// nell'area di memoria passata come parametro in quella nuova, e restituisce
// il nuovo puntatore
char* my_realloc(char* p, int len, int old_len);

// sostituisce tutti i caratteri della parola che deve essere spostata a capo
// con spazi vuoti e restituisce il numero di caratteri utf8 che sono stati 
// rimpiazzati facendo questa sostituzione
int spazi_vado_a_capo(char *row, int last);

// ritorna l'indice dell'ultima occerrenza del carattere ' '
int last_index_of(char * row);

// ritorna il numero di caratteri ' ' alla fine della stringa
int count_space(char * row);

// ritorna il numero di parole all'interno della stringa
int count_word(char * row);

// dealloca dalla memoria tutta la memoria occupata dalla pagina
// passata come parametro
void pulisci_pagina(pagina *p);

// toglie gli spazi alla fine della riga, relativi alla spaziatura
// tra colonne, se essa non occupa interamente la riga del file 
// di output
void togli_spazi_superflui(pagina * p, bool *errore);

// elimina dalla stringa passata per parametro eventuali parole
// che devono essere spostate alla riga successiva
char * vado_a_capo(FILE * fp,char * row, int w, int *count_vado_a_capo, bool *errore);

// giustifica la riga passata come parametro equidistribuendo tra le parole gli spazi
// aggiuntivi presenti alla fine della riga
void giustificazione_riga(char * row, int w, bool *errore);