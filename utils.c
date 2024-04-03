#include "utils.h"

//TODO: controllare nell'altro programma tutte le volte che faccio my_realloc che non restituisce NULL

// trasforma una stringa rappresentante un numero in un intero
int new_atoi(char n[]){

    int pos=1;
    int ris=0;
    int len=strlen(n)-1;
    int x;

    for(int i=len;i>=0;i--){

        // se il carattere non è un numero viene generato un errore
        if(n[i]<48 || n[i]>57){
            return -1;
        }

        // il carattere viene trasformato in cifra numerica
        x=n[i]-48;

        // calcolo del risultato intero seguendo la notazione decimale
        ris+=x*pos;
        pos*=10;
    }

    return ris;
}

// aggiunge un nodo alla fine della lista della pagina
void * push(pagina * p, nodo * elem){

    // se la lista è vuota, l'elemento viene aggiunto
    // ed il puntatore dell'inizio ed della fine della 
    // lista puntano all'unico elemento
    if(p->inizio==NULL && p->fine ==NULL){
        p->inizio=elem;
        p->fine=elem;
    }
    else{
        // altrimenti l'elemento viene inserito alla fine della lista
        p->fine->next=elem;
        p->fine=elem;
    }

    return p;
}

// restituisce il minimo tra i due parametri passati in ingresso
int min(int x,int y){
    if(x<y)
        return x;
    return y;
}

// alloca una nuova area di memoria di dimensione len, copiando ciò che è presente
// nell'area di memoria passata come parametro in quella nuova, e restituisce
// il nuovo puntatore
char* my_realloc(char* p, int len, int old_len) {

    // allocazione nuova aria di memoria di grandezza len + 1
    // includendo anche lo spazio per il carattere terminatore delle stringhe \0
    char* new_p = malloc((len + 1)*sizeof(char));
    if (new_p == NULL) {
        return NULL;
    }

    // la nuova area di memoria viene pulita
    memset(new_p,' ',len+1);

    // la stringa memorizzata nell'area di memoria puntata da p
    // viene copiata nell'area di memoria puntata da new_p
    memcpy(new_p, p, min(old_len,len));

    // alla fine della nuova area di memoria viene inserito 
    // il carattere terminatore delle stringhe
    new_p[len]='\0';

    // l'area di memoria puntata da p viene deallocata
    free(p);

    return new_p;
}

// sostituisce tutti i caratteri della parola che deve essere spostata a capo
// con spazi vuoti e restituisce il numero di caratteri utf8 che sono stati 
// rimpiazzati facendo questa sostituzione
int spazi_vado_a_capo(char *row, int last){
    int count_utf8=0;
    int flag=0;

    for(int i=last;i<strlen(row);i++){
        unsigned char c=(unsigned char)row[i];

        // la quantità di caratteri aggiunti per i caratteri utf-8 viene aumentata
        if(c>=128 && c<224 && flag==0){
            count_utf8++;
            flag=2;
        }
        else if(c>=224 && c<240 && flag==0){
            count_utf8+=2;
            flag=3;
        }
        else if(c>=240 && flag==0){
            count_utf8+=3;
            flag=4;
        }
        if(flag!=0)
            flag--;

        // i caratteri della parola a metà tra due righe vengono 
        // sostituiti con degli spazi vuoti
        row[i]=' ';
    }

    return count_utf8;
}

// ritorna l'indice dell'ultima occerrenza del carattere ' '
int last_index_of(char * row){

    for(int i=strlen(row); i>=0; i--){
        if(row[i]==' ')
            return i;
    }
    return 0;
}

// ritorna il numero di caratteri ' ' alla fine della stringa
int count_space(char * row){
    int count=0;
    int i=strlen(row)-1;
    while(i>=0 && row[i]==' '){
        count++;
        i--;
    }
    return count;
}

// ritorna il numero di parole all'interno della stringa
int count_word(char * row){
    int count=1;

    // il numero di parole viene aumentato ogni volta che nella stringa
    // è presente il carattere ' ' ed il carattere successivo è diverso da ' '
    for(int i=0;i<strlen(row)-1;i++){
        if(row[i]==' '&& row[i+1]!=' ')
            count++;
    }
    return count;
}

// dealloca dalla memoria tutta la memoria occupata dalla pagina
// passata come parametro
void pulisci_pagina(pagina *p){

    // variabile d'appoggio per scorrere la lista,
    // inizializzata alla testa della lista puntata
    nodo * next;
    next=p->inizio;

    while(next!=NULL){
        next=next->next;

        // deallocazione della stringa rappresentante la riga
        free(p->inizio->val);

        // deallocazione della struttra nodo
        free(p->inizio);

        p->inizio=next;
    }

}

// toglie gli spazi alla fine della riga, relativi alla spaziatura
// tra colonne, se essa non occupa interamente la riga del file 
// di output
void togli_spazi_superflui(pagina * p, bool *errore){

    // variabile d'appoggio per scorrere la lista,
    // inizializzata alla testa della lista puntata
    nodo * next;
    next= p->inizio;

    while(next!=NULL){

        // se la riga non occupa tutte le d colonne, la stringa
        // viene riallocata per eliminare gli spazi 
        // superflui relativi alla spaziatura tra colonne
        if(next->val[strlen(next->val)-1]==' '){
            int i;
            for(i=strlen(next->val)-1; i>=0; i--){
                if(next->val[i]!=' ')
                    break;
            }

            // TODO: controllare valore di ritorno del my_realloc
            next->val=my_realloc(next->val,i+1,strlen(next->val));
            if(next->val==NULL){
                // se la my_realloc non è andata a buon fine, la variabile che indica
                // la presenza di un errore, viene impostata a true e la funzione
                // termina 
                *errore=true;
                return;
            }

        }
        next=next->next;
    }
}

// la funzione elimina dalla stringa passata per parametro eventuali parole
// che devono essere spostate alla riga successiva
char * vado_a_capo(FILE * fp,char * row, int w, int *count_vado_a_capo,  bool *errore){

    // last è l'indice dell'ultima occorrenza del carattere ' '
    // cioè l'indice di inizio della parola che deve essere spostata a capo
    int last=last_index_of(row);

    // se last è 0, nella stringa è presente un'unica parola che non deve 
    // essere spostata a capo
    if(last==0)
        return row;

    // se esiste una parola da spostare a capo, i caratteri di tale parola vengono
    // rimpiazzati da ' '. Inoltre, vengono contati il numero di caratteri utf-8
    // eliminati dalla parola
    int count_utf8=spazi_vado_a_capo(row,last);

    // se sono presenti caratteri utf-8 aggiuntivi, la stringa viene reallocata
    // con la dimensione adatta per eliminare l'eccesso dovuto ai caratteri speciali
    if(count_utf8!=0){
        row[strlen(row)-count_utf8]='\0';
        // TODO: controllare valore di ritorno del my_realloc
        row=my_realloc(row,strlen(row),strlen(row));
        if(row==NULL){
            // se la my_realloc non è andata a buon fine, la variabile che indica
            // la presenza di un errore, viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile allocare il blocco di memoria");
            *errore=true;
            return NULL;
        }
    }

    // la variabile che conta il numero di caratteri in un parola viene aggiornata
    // togliendo la lunghezza della parola che viene mandata a capo
    *count_vado_a_capo+=(-1)*w+last+1;

    // il puntatore al file viene spostato indietro per permettere di rileggere la parola
    // che è stata eliminata dalla riga corrente per essere spostata alla riga successiva
    if(fseek(fp,(-1)*w+last+1,SEEK_CUR)==-1){
        // se la fseek non è andata a buon fine, la variabile che indica
        // la presenza di un errore, viene impostata a true e la funzione
        // termina 
        printf("errore: non è stato possibile tornare indietro nel file");
        *errore=true;
        return NULL;
    }

    return row;

}

// giustifica la riga passata come parametro equidistribuendo gli spazi
// aggiuntivi presenti alla fine della riga tra le parole presenti
void giustificazione_riga(char * row, int w, bool *errore){

    // la variabile rappresenta il numero di spazi presenti alla fine
    // della riga, cioè il numero di spazi che devono essere ridistribuiti
    // tra le parole
    int num_space=count_space(row);

    // la variabile rappresena il numero di parole presenti nella riga
    int num_parole=count_word(row);

    // se nella riga è presente una sola parola, la riga non deve essere
    // giustificata
    if(num_parole-1==0)
        return;
    
    // la variabile rappresenta il numero di spazi aggiuntivi da inserire
    // tra una parola e l'altra
    int distr_space=(int)(num_space/ (num_parole-1));

    // la variabile rappresenta il numero di spazi che non possono essere
    // distribuiti in modo equo. Questi spazi vengono distrubuiti singolarmente
    // tra una parola e l'altra fino ad esaurimento
    int ecc_space=num_space%(num_parole-1);

    // se non ci sono spazi da distribuire, non viene fatto nulla
    if(num_space==0)
        return;

    // allocazione buffer d'appoggio per costruire la stringa giustificata
    char * str_appo = calloc(w+1,sizeof(char));
    if(str_appo==NULL){
        // se la calloc non è andata a buon fine, la variabile che indica
        // la presenza di un errore, viene impostata a true e la funzione
        // termina 
        printf("errore: non è possibile allocare il blocco di memoria");
        *errore=true;
        return;
    }

    int i=0;
    int j=0;
    while(j<w){

        // i caratteri alfanumerici della stringa da giustificare
        // vengono copiati nella nuova stringa
        str_appo[j]=row[i];
        j++;

        // lo spazio individua la fine di una parola. Quindi devono
        // essere aggiunti gli spazi aggiuntivi per la giustificazione
        if(row[i+1]==' '){
            int z=0;

            // vengono aggiunti gli spazi equidistribuiti
            while(z<distr_space&& num_space>0){
                str_appo[j]=' ';
                j++;
                num_space--;
                z++;
            }

            // viene aggiunto uno spazio aggiuntivo se ancora gli spazi
            // in eccesso, sopra definiti, non sono esauriti
            if(ecc_space>0){
                str_appo[j]=' ';
                j++;
                ecc_space--;
            }

        }
        i++;
    }

    str_appo[w]='\0';

    // la stringa giustificata viene copiata nella stringa originale
    memcpy(row, str_appo, w);

    // deallocazione del buffer di appoggio per la giustificazione
    free(str_appo);
    
}