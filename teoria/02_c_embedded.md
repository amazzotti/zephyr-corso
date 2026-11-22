# 02 — Il linguaggio C per l'embedded (Modulo 1)

Questo file accompagna gli esercizi del Modulo 1. Ogni sezione è breve: si legge prima
dell'esercizio indicato nella consegna e si rilegge quando serve.

## 1. Da Python a C

| Python | C |
|---|---|
| interpretato: `python file.py` | **compilato**: il compilatore traduce tutto in codice macchina prima di eseguire |
| tipi decisi a runtime (`x = 5`, poi `x = "ciao"`) | ogni variabile ha un **tipo fisso**, dichiarato: `int x = 5;` |
| blocchi con l'indentazione | blocchi tra `{ }`, istruzioni che finiscono con `;` |
| `import math` | `#include <math.h>` |
| `def f(a, b):` | `int f(int a, int b) { ... }` (tipo restituito e tipo dei parametri) |
| `print(f"x = {x}")` | `printk("x = %d\n", x);` |
| `# commento` | `/* commento */` oppure `// commento` |
| `True`, `False`, `None` | `true`, `false` (da `<stdbool.h>`), `NULL` |
| gli errori di tipo si scoprono eseguendo | molti errori li trova il **compilatore** |
| la memoria la gestisce Python | la memoria la gestisci **tu**: dove sta ogni variabile, quanto è grande |

Struttura minima di un firmware Zephyr:

```c
#include <zephyr/kernel.h>      /* kernel di Zephyr (k_msleep, ...) */
#include <zephyr/sys/printk.h>  /* printk */

int main(void)                  /* void = nessun parametro */
{
	int x = 5;              /* dichiarazione + inizializzazione */
	printk("x = %d\n", x);
	return 0;
}
```

Specificatori di formato di `printk` usati nel corso:

| Formato | Tipo | Esempio |
|---|---|---|
| `%d` | intero con segno | `-12` |
| `%u` | intero senza segno | `4294967294` |
| `%x`, `%08x` | esadecimale (`%08x`: 8 cifre con zeri davanti) | `0x0000002a` (scrivi tu `0x` prima) |
| `%c` | un carattere | `Z` |
| `%s` | stringa | `ciao` |
| `%p` | indirizzo (puntatore) | `0x20001234` |
| `%-14s` | stringa allineata a sinistra su 14 colonne | `main          ` |

**Niente `%f`**: per risparmiare memoria `printk` di default non stampa i numeri con la
virgola (`float`). Nell'embedded spesso si evitano proprio i float e si lavora in **virgola
fissa**: 21,5 °C si memorizza come `215` decimi e si stampa con
`printk("%d.%d", t / 10, t % 10);`.

## 2. Tipi interi e dimensioni

In Python un intero può essere grande quanto vuoi. In C ogni tipo ha una **dimensione
fissa** in byte, e quindi un intervallo di valori.

I tipi "classici" (`char`, `short`, `int`, `long`) cambiano dimensione da una CPU all'altra:
`int` è 4 byte sul Cortex-M4 ma era 2 byte su molti microcontrollori a 8/16 bit. Per questo
nell'embedded si usano i **tipi a larghezza fissa** di `<stdint.h>`:

| Tipo | Byte | Intervallo |
|---|---|---|
| `int8_t` | 1 | -128 … 127 |
| `uint8_t` | 1 | 0 … 255 |
| `int16_t` | 2 | -32768 … 32767 |
| `uint16_t` | 2 | 0 … 65535 |
| `int32_t` | 4 | circa ±2,1 miliardi |
| `uint32_t` | 4 | 0 … 4294967295 |
| `int64_t` / `uint64_t` | 8 | enormi |

`u` = *unsigned* (senza segno, solo valori ≥ 0). `sizeof(tipo)` dice quanti byte occupa.
Sul Cortex-M4 (architettura a 32 bit) anche gli indirizzi (puntatori) sono di 4 byte.

**Overflow.** Se un risultato non ci sta, si "ricomincia dal fondo", come un contachilometri:

```c
uint8_t u = 250;
u = u + 10;        /* 260 non ci sta in un byte: 260 - 256 = 4 */
uint32_t a = 3, b = 5;
a - b;             /* non -2: 4294967294 (= 2^32 - 2) */
```

Per i tipi **senza** segno questo comportamento è garantito. Per quelli **con** segno
l'overflow aritmetico è *comportamento indefinito*: non contarci mai.

**Signed contro unsigned.** Se in un confronto si mescolano `int` e `unsigned int`, il C
converte l'`int` in `unsigned`: `-1` diventa 4294967295, e `-1 < 1u` è **falso**. Il
compilatore lo segnala con un warning (`comparison of integer expressions of different
signedness`): leggilo sempre.

**Costanti**: `10` è `int`, `10u` è `unsigned`, `10UL` è `unsigned long`, `0x1F` è
esadecimale, `0b101` binario (estensione di GCC).

## 3. Operatori sui bit

I registri delle periferiche sono numeri a 32 bit in cui **ogni bit o gruppo di bit ha un
significato** (bit 13 = pin 13, bit 0 = "abilita", bit 8..10 = "velocità"...). Per questo i
bit si manipolano continuamente.

| Operatore | Nome | Esempio (8 bit) |
|---|---|---|
| `a & b` | AND | `1100 & 1010 = 1000` |
| `a \| b` | OR | `1100 \| 1010 = 1110` |
| `a ^ b` | XOR | `1100 ^ 1010 = 0110` |
| `~a` | NOT | `~1100 = 0011` (su tutti i bit del tipo) |
| `a << n` | shift a sinistra | `0001 << 3 = 1000` |
| `a >> n` | shift a destra | `1000 >> 3 = 0001` |

Le quattro operazioni fondamentali sul bit `n`:

```c
reg |=  (1u << n);          /* accendi (set)      */
reg &= ~(1u << n);          /* spegni (clear)     */
reg ^=  (1u << n);          /* inverti (toggle)   */
if (reg & (1u << n)) { }    /* leggi (test)       */
```

`1u << n` è una **maschera**: un numero con un solo bit a 1. Zephyr offre `BIT(n)` che fa
la stessa cosa (in `<zephyr/sys/util.h>`, già incluso da `kernel.h`).

**Campi di più bit.** Per scrivere il valore 5 nei bit 8..10 senza toccare gli altri:

```c
reg = (reg & ~GENMASK(10, 8)) | (5u << 8);   /* GENMASK(10, 8) = bit da 8 a 10 a 1 = 0x700 */
valore = (reg >> 8) & 0x7;                   /* rilettura: sposto giù e tengo 3 bit */
```

Attenzione: `1 << 31` con `1` di tipo `int` (con segno) è comportamento indefinito. Usa
sempre `1u` o `BIT(31)`.

## 4. Array e stringhe

Un **array** è una sequenza di elementi dello **stesso tipo**, di **dimensione fissa**,
consecutivi in memoria:

```c
int voti[5] = {7, 8, 6, 9, 6};
voti[0];                 /* primo elemento; l'ultimo è voti[4] */
sizeof(voti);            /* 5 * 4 = 20 byte */
ARRAY_SIZE(voti);        /* 5 elementi (macro di Zephyr: sizeof(voti) / sizeof(voti[0])) */
```

A differenza delle liste di Python: niente `append`, niente indici negativi, e **nessun
controllo** se esci dai limiti. `voti[7] = 0;` compila e scrive in una zona di memoria che
appartiene a qualcos'altro: è il tipico bug che corrompe dati "a caso".

Una **stringa** in C è un array di `char` che finisce con il carattere `'\0'` (byte 0), il
**terminatore**:

```c
char nome[] = "Zephyr";  /* 7 byte: 'Z','e','p','h','y','r','\0' */
strlen(nome);            /* 6: conta fino al terminatore, escluso (da <string.h>) */
```

`'Z'` (virgolette singole) è **un carattere**, cioè un numero (il codice ASCII 90);
`"Z"` (doppie) è una stringa di 2 byte. Le lettere minuscole stanno 32 posizioni dopo le
maiuscole: `'a' - 'A' == 32`.

Per comporre stringhe si usa `snprintk(buffer, dimensione, formato, ...)`: scrive al massimo
`dimensione - 1` caratteri più il terminatore, e **restituisce la lunghezza che avrebbe
voluto scrivere**. Se il valore restituito è ≥ `dimensione`, il testo è stato troncato.
Mai usare funzioni che non conoscono la dimensione del buffer (come `sprintf` o `strcpy`):
sono la causa classica dei *buffer overflow*.

## 5. Puntatori

Ogni variabile sta in memoria a un certo **indirizzo**. Un **puntatore** è una variabile
che contiene un indirizzo.

```c
int a = 10;
int *p = &a;     /* p "punta ad" a: contiene l'indirizzo di a      */
*p = 20;         /* scrivo nella variabile puntata: ora a vale 20  */
```

- `&a` = indirizzo di `a`;
- `int *p` = "p è un puntatore a int";
- `*p` = "la cosa a cui punta p" (*dereferenza*).

**Perché servono.** In C gli argomenti si passano **per valore** (la funzione riceve una
copia). Per far modificare una variabile a una funzione le si passa il suo indirizzo:

```c
void azzera(int *x) { *x = 0; }
azzera(&a);
```

Così una funzione può anche "restituire" più risultati (es. `min` e `max`) scrivendoli in
variabili del chiamante.

**Puntatori e array.** Il nome di un array, usato in un'espressione, diventa il puntatore al
suo primo elemento. `p + 1` non aggiunge 1 byte ma **un elemento**: con `int *p`, avanza di
4 byte. Quindi `*(p + 2)` è lo stesso di `p[2]`. La differenza tra due puntatori allo stesso
array dà il numero di elementi tra loro.

Quando passi un array a una funzione passi solo il puntatore: la funzione **non** conosce la
lunghezza, quindi la si passa sempre a parte: `int somma(const int *v, size_t n)`.
`const int *v` promette che la funzione non modificherà gli elementi.

**`NULL`** è il puntatore "che non punta a niente". Si usa per dire "non trovato" o
"nessun dato". Dereferenziare `NULL` è un errore grave (sulla scheda: un *fault*).

## 6. Struct

Una `struct` raggruppa dati di tipo diverso sotto un unico nome, come un piccolo oggetto
senza metodi:

```c
struct misura {
	uint32_t timestamp_ms;
	int16_t  temperatura_decimi;
	uint8_t  sensore_id;
};

struct misura m = { .timestamp_ms = 1000, .temperatura_decimi = 215, .sensore_id = 1 };
m.temperatura_decimi;        /* accesso con il punto */

struct misura *pm = &m;
pm->temperatura_decimi;      /* con un puntatore: freccia (= (*pm).temperatura_decimi) */
```

Le struct si passano quasi sempre **per puntatore** (`const struct misura *m` se la funzione
non la modifica): passarle per valore copia tutti i byte, e le modifiche fatte dalla
funzione si perdono.

**Padding.** La CPU legge più velocemente un dato di 4 byte se il suo indirizzo è multiplo
di 4 (*allineamento*). Il compilatore quindi inserisce byte vuoti (*padding*) tra i campi e
in fondo. La struct sopra contiene 4 + 2 + 1 = 7 byte di dati ma `sizeof` dà 8. Mettendo i
campi in ordine diverso (`uint8_t`, poi `uint32_t`, poi `int16_t`) servono 12 byte. Regola
pratica: campi dal più grande al più piccolo.

## 7. Enum, switch e macchine a stati

Un `enum` dà un nome a una serie di costanti intere:

```c
enum stato { STATO_ROSSO, STATO_VERDE, STATO_GIALLO };   /* valgono 0, 1, 2 */
enum stato s = STATO_ROSSO;
```

Lo `switch` sceglie tra più casi:

```c
switch (s) {
case STATO_ROSSO:
	/* ... */
	break;          /* senza break si "cade" nel caso successivo! */
case STATO_VERDE:
	/* ... */
	break;
default:
	break;
}
```

Una **macchina a stati** è il modo più comune di organizzare la logica di un firmware: il
sistema è sempre in **uno** stato; ogni stato ha un comportamento e delle regole per passare
al successivo (transizioni). Un semaforo, un protocollo di comunicazione, la gestione di un
pulsante sono macchine a stati.

## 8. static, const e il preprocessore

**`static` su una variabile locale**: la variabile non sta sullo stack ma in memoria fissa;
mantiene il valore tra una chiamata e l'altra della funzione, ed è inizializzata una sola
volta.

**`static` su una variabile o funzione globale**: visibile **solo in quel file `.c`**.
È il modo del C per avere qualcosa di "privato".

**`const`**: il valore non si può modificare dopo l'inizializzazione; il compilatore dà
errore se ci provi. Le costanti globali `const` finiscono in flash, non in RAM.

**Il preprocessore** lavora *prima* del compilatore, sul testo: le righe che iniziano con
`#` sono sue.

- `#include <file.h>`: copia lì il contenuto del file;
- `#define NOME valore`: sostituisce `NOME` con `valore` ovunque, come un "trova e sostituisci";
- `#define QUADRATO(x) ((x) * (x))`: macro con parametri.

Le macro sono sostituzioni di testo, non funzioni. Con `#define QUADRATO(x) x * x`,
`QUADRATO(2 + 3)` diventa `2 + 3 * 2 + 3`, cioè 11. Regola: **parentesi intorno a ogni
parametro e a tutta l'espressione**. E mai passare a una macro argomenti con effetti
collaterali come `i++`: potrebbero essere valutati due volte.

Macro utili di Zephyr (`<zephyr/sys/util.h>`): `ARRAY_SIZE(a)`, `MIN(a, b)`, `MAX(a, b)`,
`CLAMP(v, min, max)`, `BIT(n)`, `GENMASK(alto, basso)`.

## 9. Programmi su più file

Un progetto vero è diviso in moduli. Per ogni modulo:

- `modulo.h` (**header**): le **dichiarazioni**, cioè cosa offre il modulo (firme delle
  funzioni, tipi, costanti). È l'"interfaccia".
- `modulo.c`: le **definizioni**, cioè il codice delle funzioni.

Chi usa il modulo fa `#include "modulo.h"` (virgolette = file del progetto; `< >` = librerie
di sistema).

```c
/* statistiche.h */
#ifndef STATISTICHE_H          /* header guard: evita di includere il file due volte */
#define STATISTICHE_H
#include <stdint.h>
#include <stddef.h>
int32_t stat_somma(const int32_t *v, size_t n);    /* dichiarazione */
extern int stat_chiamate;                          /* variabile definita in statistiche.c */
#endif
```

`extern` dice: "questa variabile esiste, ma è **definita** in un altro file". La definizione
(`int stat_chiamate = 0;`) deve esserci in **un solo** `.c`.

Ogni `.c` è compilato separatamente in un `.o`; il **linker** poi collega le chiamate alle
funzioni. Se una funzione è dichiarata ma il suo `.c` non è nella compilazione, l'errore
arriva dal linker: `undefined reference to 'stat_somma'`. In Zephyr i file da compilare si
elencano in `CMakeLists.txt` con `target_sources(app PRIVATE src/main.c src/statistiche.c)`.

## 10. La memoria del microcontrollore

Lo STM32F407 vede un unico spazio di indirizzi a 32 bit (*mappa di memoria*):

```
0x00000000 - ...          alias della flash all'avvio (la CPU legge qui i vettori)
0x08000000 - 0x080FFFFF   FLASH (1 MB)       codice e costanti
0x10000000 - 0x1000FFFF   RAM CCM (64 KB)    RAM veloce usata solo dalla CPU (Zephyr qui non la usa)
0x1FFF7A10                UID                identificativo unico del chip (96 bit)
0x20000000 - 0x2001FFFF   RAM (128 KB)       variabili, stack
0x40000000 - ...          PERIFERICHE APB    registri di USART, timer, I2C, watchdog, ...
0x40020000 - ...          PERIFERICHE AHB1   GPIOA (0x40020000), GPIOB, ... GPIOD (0x40020C00),
                                             RCC (0x40023800, i clock)
0xE0000000 - ...          CORE               registri del Cortex-M (NVIC, SysTick, DBGMCU)
```

(In Renode la RAM simulata è più grande, fino a `0x2003FFFF`, e l'alias a `0x00000000` e la
CCM non sono mappati: il firmware non se ne accorge.)

Il linker divide il programma in **sezioni**:

| Sezione | Contiene | Dove |
|---|---|---|
| `.text` | il codice (le funzioni) | flash |
| `.rodata` | costanti `const`, stringhe letterali `"..."` | flash |
| `.data` | variabili globali/`static` **con** valore iniziale diverso da 0 | RAM (il valore iniziale è conservato in flash e copiato in RAM all'avvio) |
| `.bss` | variabili globali/`static` senza valore iniziale (o = 0) | RAM (azzerata all'avvio) |
| stack | variabili locali delle funzioni, indirizzi di ritorno | RAM (uno stack per ogni thread) |
| heap | memoria chiesta a runtime (`k_malloc`) | RAM (nell'embedded si usa poco) |

Per questo, sul microcontrollore, la RAM è la risorsa più preziosa: ogni variabile globale
e ogni stack la consumano per tutto il tempo.

Il file `build/zephyr/zephyr.map`, generato dal linker, dice a che indirizzo è finito ogni
simbolo e in che sezione. La tabella `Memory region` alla fine di `west build` riassume
quanta flash e quanta RAM sono usate.

Le variabili **locali** (dentro una funzione, non `static`) esistono solo mentre la funzione
è in esecuzione: restituire l'indirizzo di una variabile locale è un errore grave, perché
quella zona di stack verrà riusata.

## 11. Registri memory-mapped e volatile

Le periferiche si comandano tramite **registri**: celle a 32 bit che si trovano a indirizzi
fissi della mappa di memoria. Leggere o scrivere a quell'indirizzo non tocca la RAM ma la
periferica (*memory-mapped I/O*). Gli indirizzi sono nel manuale del chip: per lo STM32F407
è il *reference manual* RM0090 (il *datasheet* contiene invece i dati elettrici e i pin).

**Prima regola degli STM32: il clock.** Ogni periferica ha un "interruttore" del clock nel
blocco **RCC** (*Reset and Clock Control*). Dopo il reset quasi tutte sono spente per
risparmiare energia: finché non accendi il clock, la periferica non risponde e le
scritture nei suoi registri non hanno effetto. Per la porta GPIOD l'interruttore è il bit 3
(`GPIODEN`) di `RCC_AHB1ENR`, a `0x40023830`. (Zephyr accende da solo i clock delle
periferiche che usa; Renode non simula questo "spegnimento", ma sulla scheda vera dimenticarlo
è l'errore numero uno.)

Esempio, porta GPIOD dello STM32F407 (base `0x40020C00`; ogni porta occupa `0x400` byte):

| Registro | Indirizzo | Effetto |
|---|---|---|
| `MODER` | `0x40020C00` | modo di ogni pin, **2 bit per pin**: `00` ingresso, `01` uscita, `10` funzione alternativa (es. USART), `11` analogico. Il pin *n* usa i bit `2n+1..2n` |
| `IDR` | `0x40020C10` | livello letto sui pin (un bit per pin), sola lettura |
| `ODR` | `0x40020C14` | livello delle uscite (un bit per pin) |
| `BSRR` | `0x40020C18` | scrivendo 1 nel bit *n* porta a 1 il pin *n* (*Set*); scrivendo 1 nel bit *n+16* lo porta a 0 (*Reset*). Gli altri pin non cambiano |

`BSRR` evita la sequenza leggi-modifica-scrivi su `ODR`: un'unica scrittura, senza rischio
di toccare gli altri bit e senza che un interrupt possa "intromettersi" a metà.
`MODER` invece ha campi di 2 bit: per cambiare un pin si azzera il suo campo e poi lo si
imposta (leggi-modifica-scrivi).

In C si accede a un registro con un puntatore a un indirizzo fisso:

```c
#define REG32(indirizzo) (*(volatile uint32_t *)(indirizzo))

REG32(0x40023830) |= BIT(3);                                    /* clock di GPIOD   */
REG32(0x40020C00) = (REG32(0x40020C00) & ~(3UL << 30)) | (1UL << 30); /* PD15 uscita */
REG32(0x40020C18) = BIT(15);                                    /* PD15 a 1          */
REG32(0x40020C18) = BIT(15 + 16);                               /* PD15 a 0          */
uint32_t odr = REG32(0x40020C14);
```

**`volatile`** dice al compilatore: "questo valore può cambiare, o la sua scrittura può avere
effetti, in modi che tu non vedi; non ottimizzare, leggi e scrivi davvero ogni volta". Senza
`volatile` il compilatore potrebbe, per esempio, eliminare due scritture consecutive sullo
stesso indirizzo tenendo solo l'ultima, o leggere un registro una volta sola fuori da un
ciclo. Serve per i registri delle periferiche e, come vedrai, per variabili modificate da
un interrupt.

Nella pratica con Zephyr **non** si scrive nei registri a mano: si usano i driver (dal
Modulo 2), che sono portabili su chip diversi. Ma i driver fanno esattamente questo, e
saperlo ti permette di capire cosa succede e di fare debug.

I LED della Discovery (PD12 verde, PD13 arancione, PD14 rosso, PD15 blu) si accendono
quando il pin è a **1** (*attivi alti*): `BSRR` bit *n* accende, bit *n+16* spegne. Su altre
schede i LED possono essere *attivi bassi* (acceso = pin a 0): il devicetree di Zephyr lo
indica con `GPIO_ACTIVE_LOW` e il driver ne tiene conto per te.

Un altro registro utile: `DBGMCU_IDCODE` a `0xE0042000` contiene l'identificativo del
modello di chip (bit 11..0 `DEV_ID`, bit 31..16 `REV_ID`). Sullo STM32F407 `DEV_ID` vale
`0x413`; in Renode l'intero registro vale `0x10006411`.

## 12. Il buffer circolare

Un **buffer circolare** (*ring buffer*) è una coda FIFO (il primo che entra è il primo che
esce) di capacità fissa, realizzata con un array e due indici:

```
 dati:  [ . | A | B | C | . | . | . | . ]
              ^coda         ^testa
 coda  = dove si legge il prossimo elemento
 testa = dove si scrive il prossimo elemento
```

- `push`: scrive in `dati[testa]`, poi `testa = (testa + 1) % CAPACITA`;
- `pop`: legge da `dati[coda]`, poi `coda = (coda + 1) % CAPACITA`;
- quando un indice arriva in fondo, il `%` (resto) lo riporta a 0: per questo è "circolare";
- per distinguere "pieno" da "vuoto" (in entrambi i casi `testa == coda`) si tiene un
  contatore degli elementi presenti.

È la struttura con cui si passano dati tra chi li produce e chi li consuma, per esempio tra
l'interrupt della UART che riceve i byte e il codice che li elabora. Non usa memoria
dinamica: tutto è allocato una volta, con dimensione nota, come si preferisce
nell'embedded.

Una funzione che può fallire (buffer pieno o vuoto) restituisce `bool` e scrive il
risultato tramite un puntatore: `bool rb_pop(struct ringbuf *rb, uint8_t *v)`.
