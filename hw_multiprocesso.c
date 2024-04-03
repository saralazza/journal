#include <unistd.h>
#include <signal.h>
#include <sys/errno.h>
#include <sys/wait.h>

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
            else if(c>=224 && c<240 ){
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

// legge da file i caratteri, crea la riga e la formatta
void processo_leggi_riga(int pipe_read_proc, FILE * fp_input,int s,int w ,int h,int d, bool *errore){
    bool flag_paragrafo=false;

    bool flag_n=false;

    int count_caratteri_parola=0;
    
    int num_righe=0;

    bool flag_eof=false;

    while(!feof(fp_input)){

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
            row=leggi_riga(fp_input, w, row,&count_caratteri_parola,&flag_spazi,&flag_paragrafo,&flag_n,errore);
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

        // se il numero di righe lette raggiunge h, una nuova colonna deve essere
        // iniziata quindi il contatore viene azzerato
        num_righe++;
        if(num_righe==h)
            num_righe=0;

        // la lunghezza della parola, comprensiva di '\0' finale, viene scritta
        // sulla pipe
        int len_row=strlen(row)+1;
        if(write(pipe_read_proc, &len_row, sizeof(int))==-1){
            // se la write non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            return;
        }

        // la parola viene scritta sulla pipe
        if(write(pipe_read_proc, row, strlen(row)+1)==-1){
            // se la write non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            return;
        }

        // la variabile che indica se il file è finito viene scritta su pipe
        flag_eof=feof(fp_input);
        if(write(pipe_read_proc, &flag_eof, sizeof(int))==-1){
            // se la write non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            return;
        }

    }
}

// trasforma la pagina in una stringa unica
void pagina_toString(char * str_pagina, pagina * p){

    nodo * appo;
    appo=p->inizio;

    int offset_pagina=0;

    while(appo!=NULL){

        // la stringa contenuta nel nodo, rappresentante una riga del file di output
        // viene copiata nella stringa dell'intera pagina
        memcpy(str_pagina+offset_pagina, appo->val,strlen(appo->val));

        // alla fine della riga viene aggiunto il carattere '\n'
        str_pagina[offset_pagina+strlen(appo->val)]='\n';

        offset_pagina+=strlen(appo->val)+1;
        appo=appo->next;
    }

    
}

// costruisce le pagine del file di output
void processo_costruisci_pagina(int pipe_read_proc, int pipe_proc_write, int s ,int w, int h, int d, bool *errore){

    int len_row;

    // allocazione di una pagina del file di output
    pagina *p=malloc(sizeof(pagina));
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

    int num_righe=0;
    int num_colonna=0;

    int dim_pagina=0;

    bool flag_eof=false;

    while(read(pipe_read_proc,&len_row, sizeof(int))){

        // la dimensione della parola da inserire viene letta dalla pipe

        // la riga da leggere dalla pipe viene allocata in memoria
        char * row= malloc(len_row);
        if(row==NULL){
            // se la malloc non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile allocare il blocco di memoria");
            *errore=true;
            return;
        }

        // la parola da inserire viene letta da pipe
        if(read(pipe_read_proc,row, len_row)==-1){
            // se la read non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            free(row);
            return;
        }

        // la variabile che indica la fine del file viene letta da pipe
        if(read(pipe_read_proc,&flag_eof, sizeof(int))==-1){
            // se la read non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            free(row);
            return;
        }

        // fatta eccezzione per l'ultima colonna, la riga viene riallocata per contenere
        // gli spazi tra le colonne e vengono aggiunti tali spazi
        if((num_colonna/h)<d-1){
            int len=strlen(row);
            row[strlen(row)]=' ';
            row=my_realloc(row,len+s,len);
            if(row==NULL){
                // se la my_realloc non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                printf("errore: non è possibile allocare il blocco di memoria");
                *errore=true;
                pulisci_pagina(p);
                return;
            }
        }

        dim_pagina+=strlen(row);

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
                pulisci_pagina(p);
                return;
            }
            new_riga->val=row;
            new_riga->next=NULL;
            p=push(p,new_riga);
            dim_pagina++;
            
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
                pulisci_pagina(p);
                return;
            }
            memcpy(appo->val+ prev_len, row, strlen(row));

            
        }

        // se il numero di righe lette raggiunge h, una nuova colonna deve essere
        // iniziata quindi il contatore delle righe viene azzerato e quello delle 
        // colonne viene incrementato
        num_righe++;
        if(num_righe==h){
            num_righe=0;
            num_colonna++;
        }

        // se il numero di colonne raggiunge d, la pagina è pronta. A questo punto,
        // la pagina viene trasformata in stringa
        if(num_colonna==d){

            dim_pagina++;
            char * str_pagina=calloc(dim_pagina,sizeof(char));
            if(str_pagina==NULL){
                // se la calloc non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                printf("errore: non è possibile allocare il blocco di memoria");
                *errore=true;
                pulisci_pagina(p);
                return;
            }

            // la pagina viene trasformata in stringa
            pagina_toString(str_pagina,p);

            // la dimensione della stringa rappresentante la pagine viene scritta sulla pipe
            if(write(pipe_proc_write,&dim_pagina, sizeof(int))==-1){
                // se la write non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                printf("errore: non è possibile scrivere sulla pipe");
                *errore=true;
                free(str_pagina);
                pulisci_pagina(p);
                return;
            }

            // la stringa rappresentante la pagine viene scritta sulla pipe
            if(write(pipe_proc_write, str_pagina, dim_pagina)==-1){
                // se la write non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                printf("errore: non è possibile scrivere sulla pipe");
                *errore=true;
                free(str_pagina);
                pulisci_pagina(p);
                return;
            }

            // la variabile che indica la fine del file viene scritta su pipe
            if(write(pipe_proc_write, &flag_eof, sizeof(int))==-1){
                // se la write non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                printf("errore: non è possibile scrivere sulla pipe");
                *errore=true;
                free(str_pagina);
                pulisci_pagina(p);
                return;
            }

            // la pagina corrente viene pulita
            pulisci_pagina(p);

            // una nuova pagina viene creata
            p=malloc(sizeof(pagina));
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
            dim_pagina=0;
            num_colonna=0;
            num_righe=0;

        }
    }

    // se il file è terminato prima di riempire l'ultima pagina, 
    // essa non viene trascritta nel while ma separatamente
    if(num_colonna>0||num_righe>0){
        
        dim_pagina++;
        char * str_pagina=calloc(dim_pagina,sizeof(char));
        if(str_pagina==NULL){
            // se la calloc non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile allocare il blocco di memoria");
            *errore=true;
            pulisci_pagina(p);
            return;
        }

        // la pagina viene trasformata in stringa
        pagina_toString(str_pagina,p);

        // la dimensione della stringa rappresentante la pagine viene scritta sulla pipe
        if(write(pipe_proc_write,&dim_pagina, sizeof(int))==-1){
            // se la write non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            free(str_pagina);
            pulisci_pagina(p);
            return;
        }

        // la stringa rappresentante la pagine viene scritta sulla pipe
        if(write(pipe_proc_write, str_pagina, dim_pagina)==-1){
            // se la write non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            free(str_pagina);
            pulisci_pagina(p);
            return;
        }

        // la variabile che indica la fine del file viene scritta su pipe
        if(write(pipe_proc_write, &flag_eof, sizeof(int))==-1){
            // se la write non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            free(str_pagina);
            pulisci_pagina(p);
            return;
        }

        pulisci_pagina(p);
    }
}

// scrive su file le pagine
void processo_scrivi_su_file(int pipe_proc_write, FILE * fp_output, bool *errore){

    int dim_pagina;

    while(read(pipe_proc_write,&dim_pagina,sizeof(int))){

        // la dimensione della pagina da scrivere viene letta dalla pipe
        char * str_pagina= malloc(dim_pagina);
        if(str_pagina==NULL){
            // se la calloc non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile allocare il blocco di memoria");
            *errore=true;
            return;
        }

        // la pagina da scrivere sul file di output viene letta da pipe
        if(read(pipe_proc_write, str_pagina,dim_pagina)==-1){
            // se la read non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            free(str_pagina);
            return;
        }

        // stampo la pagina sul file di output
        fprintf(fp_output, "%s", str_pagina);
        if(ferror(fp_output)!=0){
            // se la fprintf non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            *errore=true;
            printf("errore: non è stato possibile scrivere sul file di output");
            return;
        }

        free(str_pagina);

        // la variabile che indica la fine del file viene letta dalla pipe
        bool flag_eof=false;
        if(read(pipe_proc_write, &flag_eof,sizeof(int))==-1){
            // se la read non è andata a buon fine, la variabile che indica
            // la presenza di un errore viene impostata a true e la funzione
            // termina 
            printf("errore: non è possibile scrivere sulla pipe");
            *errore=true;
            return;
        }

        // se il file di input non è finito, viene aggiunto il carattere di
        // separazione tra le pagine
        if(flag_eof==0){
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
            fprintf(fp_output,"%s",fine_pagina);
            if(ferror(fp_output)!=0){
                // se la fprintf non è andata a buon fine, la variabile che indica
                // la presenza di un errore viene impostata a true e la funzione
                // termina 
                *errore=true;
                free(fine_pagina);
                return;
            }
            free(fine_pagina);
        }
    }

}

// uccide il processo con pid passato come parametro
bool kill_process(pid_t pid) {
    bool exit = true;

    int is_alive=kill(pid,0);

    if((is_alive==-1 && errno!=ESRCH)||(is_alive!=-1 && kill(pid, SIGTERM) == -1))
        exit=false;

    return exit;
}

int main(int argc, char * argv[]){

    // ordine parametri da linea di comando
    // ./a.out file_input file_output s h w d
    // h =  numero di righe di una colonna
    // w = numero di caratteri di una riga di una colonna
    // s = spazio tra due colonne
    // d = numero di colonne
    if(argc<6){
        printf("troppi pochi parametri\n");
        exit(0);
    }

    // acquisisco parametri da linea di comando
    int s,h,d,w;
    d=new_atoi(argv[6]);
    s=new_atoi(argv[3]);
    h=new_atoi(argv[4]);
    w=new_atoi(argv[5]);

    // creazione pipe tra il processo che legge da file ed il 
    // processo che crea le pagine
    int pipe_read_proc[2];
    if(pipe(pipe_read_proc)<0){
        printf("errore: non è stato possibile aprire la pipe");
        exit(1);
    }

    int pid1, pid2, pid3;

    // creazione del primo figlio
    if((pid1=fork())==-1){
        printf("errore: non è stato possibile creare il figlio");
        exit(1);
    }
    if( pid1 == 0){

        // processo che legge da file

        // apertura file di input
        FILE * fp_input= fopen(argv[1],"r");

        if(fp_input==NULL){
            // se la fopen non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore durante l'apertura del file di input\n");
            close(pipe_read_proc[0]);
            close(pipe_read_proc[1]);
            exit(1);
        }
        // chiusura lettura della prima pipe
        if(close(pipe_read_proc[0])==-1){
            // se la close non è andata a buon fine, tutte le pipe
            // ed il file di input vengono chiusi
            printf("errore: non è stato possibile chiudere la pipe");
            fclose(fp_input);
            close(pipe_read_proc[1]);
            exit(1);
        }

        // variabile che indica se nel processo è avvenuto un errore
        bool errore_pid1=false;

        // lettura caratteri da file
        processo_leggi_riga(pipe_read_proc[1],fp_input,s,w,h,d,&errore_pid1);
        if(errore_pid1){
            close(pipe_read_proc[1]);
            exit(1);
        }

        // chiusura scrittura della prima pipe
        if(close(pipe_read_proc[1])==-1){
            // se la close non è andata a buon fine, 
            // il file di input viene chiuso
            fclose(fp_input);
            printf("errore: non è stato possibile chiudere la pipe");
            exit(1);
        }

        // chiusura file input
        if(fclose(fp_input)){
            // se la fclose non è andata a buon fine, la funzione
            // termina
            printf("errore durante la chiusura del file di input\n");
            exit(1);
        }

        exit(0);
    }

    // creazione pipe tra il processo che crea le pagine ed il processo
    // che scrive su file
    int pipe_proc_write[2];
    if(pipe(pipe_proc_write)<0){
        // se l'apertura della pipe non è andata a buon fine, tutte le pipe
        // vengono chiuse
        kill_process(pid1);
        printf("errore: non è stato possibile aprire la pipe");
        close(pipe_read_proc[0]);
        close(pipe_read_proc[1]);
        exit(1);
    }

    // creazione processo che crea le pagine
    if((pid2=fork())==-1){
        // se la creazione del secondo figlio non è andata a buon fine, tutte le pipe
        // vengono chiuse
        kill_process(pid1);
        printf("errore: non è stato possibile creare il figlio");
        close(pipe_read_proc[0]);
        close(pipe_read_proc[1]);
        close(pipe_proc_write[0]);
        close(pipe_proc_write[1]);
        exit(1);
    }

    if(pid2==0){

        // processo che crea le pagine

        // chiusura scrittura della prima pipe
        if(close(pipe_read_proc[1])==-1){
            // se la chiusura della pipe non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore: non è stato possibile chiudere la pipe");
            close(pipe_read_proc[0]);
            close(pipe_proc_write[1]);
            close(pipe_proc_write[0]);
            exit(1);
        }

        // chiusura lettura della seconda pipe
        if(close(pipe_proc_write[0])==-1){
            // se la chiusura della pipe non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore: non è stato possibile chiudere la pipe");
            close(pipe_read_proc[0]);
            close(pipe_proc_write[1]);
            exit(1);
        }

        // variabile che indica se nel processo è avvenuto un errore
        bool errore_pid2=false;

        // creazione delle pagine
        processo_costruisci_pagina(pipe_read_proc[0],pipe_proc_write[1], s,w,h,d,&errore_pid2);
        if(errore_pid2){
            close(pipe_proc_write[1]);
            close(pipe_read_proc[0]);
            exit(1);
        }

        // chiusura scrittura della seconda pipe
        if(close(pipe_proc_write[1])==-1){
            // se la chiusura della pipe non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore: non è stato possibile chiudere la pipe");
            close(pipe_read_proc[0]);
            exit(1);
        }

        // chiusura lettura della prima pipe
        if(close(pipe_read_proc[0])==-1){
            // se la chiusura della pipe non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore: non è stato possibile chiudere la pipe");
            close(pipe_proc_write[1]);
            exit(1);
        }

        exit(0);
    }

    // creazione del processo che scrive sul file
    if((pid3 = fork())==-1){
        // se la creazione del terzo figlio non è andata a buon fine, tutte le pipe
        // vengono chiuse
        kill_process(pid1);
        kill_process(pid2);
        printf("errore: non è stato possibile creare il figlio");
        close(pipe_read_proc[0]);
        close(pipe_read_proc[1]);
        close(pipe_proc_write[0]);
        close(pipe_proc_write[1]);
        exit(1);
    }

    if( pid3==0){
        // processo che scrive sul file

        // chiusura lettura della prima pipe
        if(close(pipe_read_proc[0])==-1){
            // se la chiusura della pipe non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore: non è stato possibile chiudere la pipe");
            close(pipe_read_proc[1]);
            close(pipe_proc_write[1]);
            close(pipe_proc_write[0]);
            exit(1);
        }

        // chiusura scrittura della prima pipe
        if(close(pipe_read_proc[1])==-1){
            // se la chiusura della pipe non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore: non è stato possibile chiudere la pipe");
            close(pipe_proc_write[1]);
            close(pipe_proc_write[0]);
            exit(1);
        }

        // chiusura scrittura della seconda pipe
        if(close(pipe_proc_write[1])==-1){
            // se la chiusura della pipe non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore: non è stato possibile chiudere la pipe");
            close(pipe_proc_write[0]);
            exit(1);
        }

        // variabile che indica se nel processo è avvenuto un errore
        bool errore_pid3=false;

        // apertura file di output
        FILE * fp_output= fopen(argv[2],"w");
        if(fp_output==NULL){
            // se la fopen non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore durante l'apertura del file di output\n");
            close(pipe_proc_write[0]);
            exit(1);
        }

        // scrittura su file
        processo_scrivi_su_file(pipe_proc_write[0], fp_output, &errore_pid3);
        if(errore_pid3){
            close(pipe_proc_write[0]);
            fclose(fp_output);
            exit(1);
        }

        // chiusura file di output
        if(fclose(fp_output)!=0){
            // se la fclose non è andata a buon fine, tutte le pipe
            // vengono chiuse
            printf("errore durante la chiusura del file di input\n");
            close(pipe_proc_write[0]);
            exit(1);
        }

        // chiusura lettura della seconda pipe
        if(close(pipe_proc_write[0])==-1){
            // se la close non è andata a buon fine, la funzione
            // termina
            printf("errore: non è stato possibile chiudere la pipe");
            exit(1);
        }
        exit(0);
    }

    // chiusura lettura e scrittura da entrambe le pipe
    if(close(pipe_read_proc[0])==-1||close(pipe_read_proc[1])==-1||close(pipe_proc_write[0])==-1||close(pipe_proc_write[1])==-1){
        kill_process(pid1);
        kill_process(pid2);
        kill_process(pid3);
        exit(1);
    }

    // la variabile indica se c'è stato un errore nel corso del programma
    bool errore=false;

    // gestione uccisione processi in caso di errore
    bool pid1_finito = false;
    int pid1_stato = 0;

    bool pid2_finito = false;
    int pid2_stato = 0;

    bool pid3_finito = false;
    int pid3_stato = 0; 

    // il processo main controlla lo stato dei figli fino a quando non sono
    // tutti e tre conclusi
    while (!pid1_finito || !pid2_finito || !pid3_finito) {

        // se il primo filgio non ha già concluso
        if (!pid1_finito) {

            // lo stato del primo figlio viene controllato
            pid_t pid1_wait = waitpid(pid1, &pid1_stato, WNOHANG);
            
            // se lo stato è zero, il figlio non ha ancora fatto exit e la
            // wait del padre verrà ignorata
            if (pid1_wait != 0) {

                // se lo stato è diverso da zero, il processo figlio ha terminato
                pid1_finito = true;

                // se ha terminato con un errore, il processo padre uccide i figli restanti
                if (pid1_wait < 0) {
                    errore=true;
                    pid1_finito = kill_process(pid2);
                    pid1_finito = kill_process(pid3);
                }
            }
        }

        // se il secondo filgio non ha già concluso
        if (!pid2_finito) {

            // lo stato del secondo figlio viene controllato
            pid_t pid2_wait = waitpid(pid2, &pid2_stato, WNOHANG);

            // se lo stato è zero, il figlio non ha ancora fatto exit e la
            // wait del padre verrà ignorata
            if (pid2_wait != 0) {

                // se lo stato è diverso da zero, il processo figlio ha terminato
                pid2_finito = true;

                // se ha terminato con un errore, il processo padre uccide i figli restanti
                if (pid2_wait < 0) {
                    errore=true;
                    pid2_finito = kill_process(pid1);
                    pid2_finito = kill_process(pid3);
                }
            }
        }

        // se il primo filgio non ha già concluso
        if (!pid3_finito) {

            // lo stato del primo figlio viene controllato
            pid_t pid3_wait = waitpid(pid3, &pid3_stato, WNOHANG);

            // se lo stato è zero, il figlio non ha ancora fatto exit e la
            // wait del padre verrà ignorata
            if (pid3_wait != 0) {

                // se lo stato è diverso da zero, il processo figlio ha terminato
                pid3_finito = true;

                // se ha terminato con un errore, il processo padre uccide i figli restanti
                if (pid3_wait < 0) {
                    errore=true;
                    pid3_finito = kill_process(pid1);
                    pid3_finito = kill_process(pid2);
                }
            }
        }
    }

    return errore;
}