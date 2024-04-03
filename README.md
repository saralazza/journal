# Newspaper


## Descrizione del progetto

Lo scopo del progetto è quello di sviluppare un software che prende in ingresso un file di testo e lo trasforma in una pagina di giornale. Dato un file di testo formattato su una colonna, dunque, il programma lo trasforma in un testo formattato su più pagine divise in più colonne. In particolare, il testo viene diviso su più colonne in base alle preferenze dell'utente che specifica il numero di caratteri per ogni riga ed il numero di righe di ogni colonna, il numero di colonne ed il numero di spazi tra una colonna e l'altra.

Per esempio, se il testo di input è il seguente:
```
Sistemi Operativi II Modulo. Progetto (Homework).

L’obiettivo è implementare un programma C che trasformi un testo in italiano da una colonna a più colonne su più pagine (come ad es. per una pagina di quotidiano). Sono richieste due versioni del programma: una a singolo processo e una multi-processo con almeno tre processi concorrenti.

Dati in ingresso e requisiti: un file di testo in codifica Unicode (UTF-8) contenente un testo in italiano strutturato in paragrafi separati da una o più linee vuote (stringa contenente solo il carattere ‘\n’). Ogni paragrafo è  costituito da una o più stringhe terminate da ‘\n’ (il testo in ingresso è tutto su una sola colonna); il numero di colonne per pagina su cui distribuire il testo; il numero di linee per ogni colonna; larghezza di ogni colonna (numero di caratteri per colonna); la distanza in caratteri tra una colonna di testo e l’altra.
```
I parametri sono:
```
- numero di righe = 10
- numero di colonne = 3
- numero di caratteri per riga = 40
- numero di spazi tra colonne = 10
```
Il testo di output è
```
Sistemi  Operativi II          su  più  pagine (come          Dati  in  ingresso  e
Modulo.      Progetto          ad es. per una pagina          requisiti: un file di
(Homework).                    di  quotidiano). Sono          testo   in   codifica
                               richieste         due          Unicode       (UTF-8)
L’obiettivo         è          versioni          del          contenente  un  testo
implementare       un          programma:    una   a          in           italiano
programma    C    che          singolo   processo  e          strutturato        in
trasformi un testo in          una    multi-processo          paragrafi separati da
italiano    da    una          con     almeno    tre          una o più linee vuote
colonna a più colonne          processi concorrenti.          (stringa   contenente

%%%

solo   il   carattere          testo;  il  numero di
‘\n’). Ogni paragrafo          linee     per    ogni
è costituito da una o          colonna; larghezza di
più          stringhe          ogni  colonna (numero
terminate da ‘\n’ (il          di    caratteri   per
testo  in  ingresso è          colonna); la distanza
tutto   su  una  sola          in  caratteri tra una
colonna);  il  numero          colonna  di  testo  e
di colonne per pagina          l’altra.
su cui distribuire il

```
Il progetto è consituito sia da una versione monoprocesso che una multiprocesso del programma.


## Istruzioni d'uso del programma

Per testare il programma è necessario aprire il terminale e spostarsi all'interno della directory del progetto.

Per compilare sia la versione monoprocesso che multiprocesso del programma è sufficiente runnare su terminale il seguente comando:
```
make
```
In questo modo, vengono generati due file eseguibili differenti, uno per ogni versione del programma. Per eseguire la versione monoprocesso del programma è sufficiente runnare il seguente comando da terminale:
```
./monoprocesso finput foutput s h w d
```
Per eseguire la versione multiprocesso è sufficiente runnare il seguente comando da terminale:
```
./multiprocesso finput foutput s h w d
```
dove i parametri indicano:

- finput, il file di input da cui viene letto il testo da trasformare in giornale
- foutput, il file sul quale scrivere il testo formattato 
- s, il numero di spazi tra una colonna e l'altra
- h, il numero di righe del testo formattato
- w, il numero di caratteri massimi per ogni colonna del testo formattato
- d, il numero massimo di colonne del testo formattato


## Architettura del programma

Nella versione monoprocesso, è presente un singolo processo che si occupa di leggere i dati dal file di input, creare le righe formattate inserendole correttamente nella pagina e scrivere la pagina formattata sul file di output.

La pagina da scrivere sul file di output viene salvata in una struttura denominata 'pagina'. Tale struttura contiene una queue che rappresenta le righe della pagina. Quindi ogni volta che viene letta una riga di w caratteri dal file, essa viene formattata ed inserita all'interno del nodo rappresentante la riga di output in questione.

Nella versione multiprocesso, i compiti vengono svolti da tre processi concorrenti che comunicano tra loro attraverso due pipe. I tre processi, fratelli tra loro, sono tutti figli del processo main. La prima pipe permette la comunicazione tra il primo ed il secondo processo; mentre la seconda pipe mette in comunicazione il secondo ed il terzo processo.

I compiti, tra i vari processi, vengono così suddivisi:

- il primo processo si occupa di leggere i caratteri dal file di input e creare le righe giustificate, scrivendole sulla prima pipe
- il secondo processo si occupa di leggere le righe dalla prima pipe, inserendole nella struttura rappresentante la pagina, e di scrivere la stringa rappresenante la pagina completa nella seconda pipe
- il terzo processo si occupa di leggere la stringa rappresentante la pagina dalla seconda pipe e scriverla su pipe