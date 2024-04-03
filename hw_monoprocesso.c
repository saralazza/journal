#include "utils.h"

// costruisce le righe da inseire in ogni colonna della pagina
char * leggi_riga(FILE * fp, int w, char * row, int * count_caratteri_parola,bool * flag_spazi, bool *flag_paragrafo, bool *flag_n, bool *errore){
    int num_caratteri=0;

    unsigned char c;

    int flag_utf8=0;

    while(!feof(fp) && num_caratteri<w){

        // lettura di un carattere dal file di input
        c=fgetc(fp);
        if(ferror(fp)!=0){
            // se la fgetc non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è stato possibile leggere un carattere dal file di input");
            *errore=true;
            return NULL;
        }

        // il numero di caratteri consecuitivi letti viene azzerato
        // se si incontra uno dei caratteri sotto riportati o se si
        // è alla fine del file. La variabile viene utilizzata per
        // gestire il caso in cui una parola è più grande della 
        // dimensione della riga w
        if(c==' '|| c=='\n'|| c=='\r' || c=='\t'||feof(fp)){
            (*count_caratteri_parola)=0;
        }
        else{
            (*count_caratteri_parola)++;
        }

        // se il numero di caratteri consecuiti letti è maggiore di w,
        // la parola è troppo grande per entrare in qualsiasi riga. Quindi
        //  la variabile che indica la presenza di un errore viene impostata 
        // a true e la funzione termina 
        if(*count_caratteri_parola>w){
            printf("errore: riga troppo piccola per inserire una stringa\n");
            *errore=true;
            return NULL;
        }

        // il carattere letto dal file di input viene inserito nella
        // stringa che poi verrà aggiunta alla lista delle righe della
        // pagina

        // vengono ignorati i seguenti:
        //   - carattere di terminazione riga
        //   - carattere di fine file
        //   - spazi se sono all'inizio della riga
        //   - carattere di tabulazione
        //   - spazo multipli tra le parole
        if(c!='\n'&&c!='\r' &&!feof(fp) && !(num_caratteri==0 && (c==' '||c=='\t')) && c!='\t' && !(row[num_caratteri-1]==' ' && (c==' '||c=='\t'))){
            row[num_caratteri]=c;
            num_caratteri++;
        }

        // se è stato aggiunto un paragrafo e vengono letti
        // i caratteri sotto indicati, la flag che indica 
        // la necessità di aggiungere un nuovo paragrafo
        // non viene resettata
        if(*flag_n==true && (c=='\n'|| c==' '|| c=='\t'))
            continue;

        // se non è stato aggiunto un paragrafo e viene letto
        // il carattere '\n', la variabile che indica la necessità
        // di aggiungere un paragrafo viene settata a true
        // e la lettura di caratteri dal file di input da inserire 
        // nella riga corrente viene interrotta
        if(c=='\n' && *flag_paragrafo==false){
            *flag_paragrafo=true;
            break;
        }
        else if(c!='\n'){

            // se viene letto un carattere che indica la presenza di 
            // un paragrafo, la flag che indica la necessità di aggiungere
            // un paragrafo viene resettata
            *flag_paragrafo=false;

            // se viene letto un carattere diverso da '\n' allora, l'ultima
            // riga non sarà più quella di un paragrafo e la flag viene 
            // resettata
            *flag_n=false;
        }
        else{

            continue;
        }

        // quando viene letto il primo carattere che rappresenta un carattere
        // utf-8, cioè quando la flag_utf8 è uguale a zero, viene controllato
        // il carattere letto dal file di input per verificare la presenza
        // di un carattere speciale
        if(flag_utf8==0&&!feof(fp)){

            // se è stato letto un carattere speciale, viene verificato il 
            // valore intero del carattere per capire da quanti caratteri 
            // speciali è composto il carattere utf-8. In questo modo viene
            // aumentata la grandezza della stringa w e viene riallocata
            // la stringa rappresentante la riga letta con my_realloc. Inoltre,
            // viene aggiornata la variabile flag_utf8 che rappresenta la quantità
            // di caratteri speciali che devono essere letti prima di rieffettuare
            // tale controllo. In questo modo si evita di allocare una stringa di 
            // dimensione eccessiva
            if(c>=128 && c<224){
                w++;
                flag_utf8=2;
                row=my_realloc(row, strlen(row)+1,strlen(row));
                if(row==NULL){
                    // se la my_realloc non è andata a buon fine, la variabile che indica
                    // la presenza di un errore viene impostata a true e la funzione
                    // termina 
                    printf("errore: non è possibile allocare il blocco di memoria");
                    *errore=true;
                    return NULL;
                }
            }
            else if(c>=224 && c<240){
                w+=2;
                flag_utf8=3;
                row=my_realloc(row, strlen(row)+2,strlen(row));
                if(row==NULL){
                    // se la my_realloc non è andata a buon fine, la variabile che indica
                    // la presenza di un errore viene impostata a true e la funzione
                    // termina 
                    printf("errore: non è possibile allocare il blocco di memoria");
                    *errore=true;
                    return NULL;
                }
            }
            else if(c>=240){
                w+=3;
                flag_utf8=4;
                row=my_realloc(row, strlen(row)+3,strlen(row));
                if(row==NULL){
                    // se la my_realloc non è andata a buon fine, la variabile che indica
                    // la presenza di un errore viene impostata a true e la funzione
                    // termina 
                    printf("errore: non è possibile allocare il blocco di memoria");
                    *errore=true;
                    return NULL;
                }
            }

        }

        // se flag_utf8 non è zero vuol dire che il carattere letto fa 
        // parte di un carattere utf-8 per il quale la stringa è già stata
        // riallocata. Quindi il controllo sovrastante non viene effettuato
        if(flag_utf8!=0)
            flag_utf8--;

    }

    // se viene letta una riga con tutti caratteri da ignorare,
    // la flag che indica che la riga non deve essere aggiunta
    // in cima ad una colonna viene settata a true
    if(num_caratteri==0){
        *flag_spazi=true;
        return row;
    }

    // se vengono letti una quantità di caratteri inferiori a w
    // a causa della presenza di un carattere indicante un paragrafo, 
    // la riga viene completata aggiungedo degli spazi
    while(num_caratteri < w&&!feof(fp)){
        row[num_caratteri]=' ';
        num_caratteri++;
    }

    row[num_caratteri]='\0';

    // viene letto il carattere successivo dal file di input
    // per capire se ci troviamo a metà di una parola che deve
    // essere spostata alla riga successiva o meno
    c=fgetc(fp);
    if(ferror(fp)!=0){
        // se la fgetc non è andata a buon fine, la variabile che indica
        // la presenza di un errore viene impostata a true e la funzione
        // termina 
        printf("errore: non è stato possibile leggere un carattere dal file di input");
        *errore=true;
        return NULL;
    }

    if((char)c!=EOF){

        // il puntatore del file di input viene spostato di una posizione
        // indietro per permettere di rileggere il carattere appena letto
        // alla riga successiva
        if(fseek(fp, -1, SEEK_CUR)==-1){
            // se la fseek non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è stato possibile spostare il puntatore del file");
            *errore=true;
            return NULL;
        }

        // se il carattere letto è un carattere alfanumerico, la riga termina
        // con una parola che deve essere spostata alla riga successiva
        if(c!=' '&&c!='\n'&&c!='\r'){
            row=vado_a_capo(fp,row,w,count_caratteri_parola,errore);
            if(*errore)
                return NULL;
        }
        
        // se la riga letta non è la fine di un paragrafo viene giustificata
        if(*flag_paragrafo!=true){
            giustificazione_riga(row,strlen(row),errore);
            if(*errore)
                return NULL;
        }
    }
    
    return row;

}

// costruisce le colonne della pagina
void leggi_colonna(FILE *fp,pagina * p, int w,int h, int d,int num_colonna, int s, bool *errore){

    int num_righe=0;

    bool flag_paragrafo=false;

    bool flag_n=false;

    int count_caratteri_parola=0;

    while(num_righe< h && !feof(fp)){

        // la riga da leggere dal file viene allocata in memoria
        char * row;
        row=malloc(sizeof(char)*(w+1));
        if(row==NULL){
            // se la malloc non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile allocare il blocco di memoria");
            *errore=true;
            return;
        }

        // l'area di memoria della riga viene settata a ' '
        // e viene aggiunto il carattere terminatore di stringa
        memset(row,' ',sizeof(char)*(w+1));
        row[w]='\0';

        bool flag_spazi=false;

        // la riga dal file di output viene letta solo se non c'è la fine di un paragrafo
        if(flag_paragrafo==false){
            row=leggi_riga(fp, w, row,&count_caratteri_parola,&flag_spazi,&flag_paragrafo,&flag_n,errore);
            if(*errore){
                free(row);
                return;
            }
        }
        else{
            // altrimenti viene aggiunta la riga vuota alla pagina
            flag_paragrafo=false;

            // la variabile indica se l'ultima riga inserita è la riga vuota
            // di un paragrafo
            flag_n=true;

            flag_spazi=true;
        }
        
        // se viene letta una riga con tutti caratteri da ignorare che deve
        // essere aggiunta in cima ad una colonna, essa non viene aggiunta e 
        // row viene deallocata dalla memoria

        // se viene letta una riga con tutti spazi, che non è quella di fine
        // paragrafo, essa viene ignorata 
        if((num_righe==0 && flag_spazi==true)||(row[0]==' '&&flag_n==false) ){
            free(row);
            continue;
        }

        // fatta eccezzione per l'ultima colonna, la riga viene riallocata per contenere
        // gli spazi tra le colonne e vengono aggiunti tali spazi
        if(num_colonna<d-1){
            int len=strlen(row);
            row[strlen(row)]=' ';
            row=my_realloc(row,len+s,len);
            if(row==NULL){
                // se la my_realloc non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                printf("errore: non è possibile allocare il blocco di memoria");
                *errore=true;
                return;
            }

        }

        // durante la creazione della prima colonna, la struttura dati rappresentante la pagina
        // è vuota e deve essere riempita con nuove righe. Quindi, viene creato un nuovo nodo
        // della lista che rappresenta una riga della pagina nel file di output. Questo nodo ha come
        // valore la stringa letta dal file di input e viene inserito nella lista puntata 
        // rappresentante la pagina
        if(num_colonna==0){
            nodo * new_riga = malloc(sizeof(nodo));
            if( new_riga==NULL){
                // se la malloc non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                printf("errore: non è possibile allocare il blocco di memoria");
                *errore=true;
                free(row);
                return;
            }
            new_riga->val=row;
            new_riga->next=NULL;
            p=push(p,new_riga);
            
        }
        else{

            // durante la creazione delle altre colonne della pagina, i nodi rappresentanti le
            // righe del file di output sono già presenti. Quindi viene preso il nodo rappresentante
            // la riga in questione e la stringa contenuta nel nodo viene riallocata in modo tale da
            // contenere la nuova riga letta dal file di input. A questo punto, la riga letta 
            // viene concatenata alla vecchia riga presente nel nodo
            int i=0;
            nodo *appo;
            appo=p->inizio;
            while(i< num_righe){
                appo=appo->next;
                i++;
            }
            int prev_len=strlen(appo->val);
            appo->val= my_realloc(appo->val,strlen(appo->val)+strlen(row),strlen(appo->val));
            if(row==NULL){
                // se la my_realloc non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                printf("errore: non è possibile allocare il blocco di memoria");
                *errore=true;
                return;
            }
            memcpy(appo->val+ prev_len, row, strlen(row));
        }

        num_righe++;
    }
    
}

// scrive sul file di output la pagina passata come parametro
void stampa_su_file(pagina * p, FILE * fp_out, FILE * fp_in, bool *errore){

    // variabile d'appoggio per scorrere la lista,
    // inizializzata alla testa della lista puntata
    nodo * next;
    next=p->inizio;

    // fino a quando tutta la lista rappresentante le righe di una pagina non viene letta,
    // le righe vengono scritte sul file di output aggiugendo '\n' alla fine
    while(next!=NULL){

        fprintf(fp_out,"%s\n",next->val);
        if(ferror(fp_out)!=0){
            // se la fprintf non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è stato possibile scrivere sul file di output");
            *errore=true;
            return;
        }

        next=next->next;
    }

    // allocazione e scrittura del carattere di terminazione pagina
    // solo se non è l'ultima pagina del file di output
    if(!feof(fp_in)){

        char * fine_pagina=malloc(sizeof(char)*7);
        if( fine_pagina==NULL){
            // se la malloc non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile allocare il blocco di memoria");
            *errore=true;
            return;
        }
        strcpy(fine_pagina, "\n%%%\n\n");
        fprintf(fp_out,"%s",fine_pagina);
        if(ferror(fp_out)!=0){
            // se la fprintf non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            *errore=true;
            return;
        }
        free(fine_pagina);

    }

}

// costruisce una pagina e la stampa su file di output
void leggi_pagina(FILE * fp, FILE * fpo, int w, int h, int d,int s, bool *errore){

    // ogni pagina è rappresentata da un numero d di colonne
    // ogni colonna contiene h righe con w caratteri
    
    int num_colonne=0;

    // allocazione di una pagina del file di output
    pagina * p=malloc(sizeof(pagina));
    if(p==NULL){
        // se la malloc non è andata a buon fine, la variabile che indica
        // la presenza di un errore viene impostata a true e la funzione
        // termina 
        printf("errore: non è stato possibile allocare una pagina");
        *errore=true;
        return;
    }
    p->inizio=NULL;
    p->fine=NULL;

    while(num_colonne<d&& !feof(fp)){

        // creazione delle colonne della pagina
        leggi_colonna(fp, p, w,h,d,num_colonne,s,errore);
        if(*errore){
            pulisci_pagina(p);
            return;
        }

        num_colonne++;

    }

    // se vengono scritte un numero di colonne inferiore a d, 
    // gli spazi tra colonne delle righe incomplete vengono eliminati
    togli_spazi_superflui(p, errore);

    // se avviene un errore nella funzione togli_spazi_superflui, causato
    // da un errore nel my_realloc, viene pulita la pagina che non verrà
    // stampata sul file di output e la funzione termina
    if(*errore){
        pulisci_pagina(p);
        return;
    }

    // scrittura della pagina sul file di output
    stampa_su_file(p, fpo, fp, errore);
    if(*errore){
        pulisci_pagina(p);
        return;
    }

    // dealloco tutta la memoria occupata dalla pagina dalla memoria
    pulisci_pagina(p);

    // dealloco la pagina dalla memoria
    free(p);

}


int main(int argc, char * argv[]){
    // ordine parametri da linea di comando
    // ./a.out file_input file_output s h w d
    // s = spazio tra due colonne
    // h =  numero di righe di una colonna
    // w = numero di caratteri di una riga di una colonna
    // d = numero di colonne
    if(argc<6){
        printf("troppi pochi parametri\n");
        return 0;
    }

    // i parametri vengono acquisiti da linea di comando e vengono
    // trasformati in interi
    int s,h,d,w;
    d=new_atoi(argv[6]);
    s=new_atoi(argv[3]);
    h=new_atoi(argv[4]);
    w=new_atoi(argv[5]);

    // apertura file di input
    FILE * fp_input= fopen(argv[1],"r");
    if(fp_input==NULL){
        printf("errore durante l'apertura del file di input\n");
        return 1;
    }

    // apertura file di output
    FILE * fp_output= fopen(argv[2],"w");
    if(fp_output==NULL){
        printf("errore durante l'apertura del file di output\n");
        return 1;
    }

    // la variabile indica se c'è stato un errore nel corso del programma
    bool errore=false;

    // creazione delle pagine fino a quando il file non finisce
    while(!feof(fp_input)&& !errore){
        leggi_pagina(fp_input,fp_output,w,h,d,s,&errore);
    }

    // chiusura file di input
    if(fclose(fp_input)!=0){
        printf("errore durante la chiusura del file di input\n");
    }
    else{
        errore=true;
    }

    // chiusura file di output
    if(fclose(fp_output)!=0){
        printf("errore durante la chiusura del file di output\n");
    }
    else{
        errore=true;
    }

    return errore;
}