# 05 — Sincronizzazione e comunicazione tra thread (Modulo 4)

Nel Modulo 3 i thread lavoravano ognuno per conto suo. Nei firmware veri invece **collaborano**:
uno legge il sensore, uno elabora, uno comunica. Appena due thread toccano gli stessi dati, o
uno deve aspettare l'altro, servono gli strumenti di questo modulo.

## 1. Il problema: dati condivisi

Una variabile globale è visibile da tutti i thread. Finché un solo thread la scrive, nessun
problema. Quando la scrivono in due, il risultato può dipendere da **come lo scheduler
alterna i thread**: a volte giusto, a volte no. Gli errori di questo tipo sono i più
difficili da trovare, perché non si ripetono sempre uguali.

## 2. Race condition (corsa critica)

`contatore++` sembra una sola operazione, ma per la CPU sono tre:

```
1. leggi   contatore dalla RAM in un registro     (LDR)
2. somma   1 al registro                          (ADDS)
3. scrivi  il registro in RAM                     (STR)
```

Se tra 1 e 3 lo scheduler passa a un altro thread che fa lo stesso, un incremento si perde:

```
contatore = 5
Thread A: legge 5 ................................ scrive 6
Thread B:            legge 5 ... scrive 6
risultato: 6   (dovrebbe essere 7)
```

Si chiama **race condition**: due thread "corrono" sulla stessa risorsa e vince chi arriva
ultimo. Su un microcontrollore a un solo core (come il nRF52840) succede quando un thread viene
interrotto (*preemption*, interrupt, time slicing) proprio in mezzo alla sequenza. È raro,
quindi negli esercizi lo **forziamo** con un `k_yield()` tra lettura e scrittura.

La parte di codice che legge e modifica dati condivisi si chiama **sezione critica**: deve
essere eseguita da un thread alla volta.

## 3. Mutex

Un **mutex** (*mutual exclusion*) è un lucchetto con un solo proprietario:

```c
K_MUTEX_DEFINE(lucchetto);               /* creato e pronto all'uso */

k_mutex_lock(&lucchetto, K_FOREVER);     /* se e' gia' preso, il thread si blocca e aspetta */
/* ... sezione critica ... */
k_mutex_unlock(&lucchetto);              /* lo libera: un thread in attesa puo' entrare */
```

Regole:
- solo il thread che l'ha preso può liberarlo;
- lo stesso thread può prenderlo più volte (è *ricorsivo*), ma deve liberarlo altrettante volte;
- **non si usa negli interrupt** (un interrupt non può bloccarsi ad aspettare);
- la sezione critica deve essere **breve**: chi aspetta è fermo;
- `k_mutex_lock` restituisce 0 se l'ha preso, un errore se scade il timeout
  (`K_MSEC(100)` invece di `K_FOREVER`).

I **timeout** in Zephyr sono di tipo `k_timeout_t`: `K_FOREVER` (aspetta per sempre),
`K_NO_WAIT` (non aspettare: o subito o errore), `K_MSEC(n)`, `K_SECONDS(n)`.

## 4. Semafori

Un **semaforo** è un contatore di "permessi":

```c
K_SEM_DEFINE(sem, 0, 1);       /* valore iniziale 0, massimo 1 */

k_sem_give(&sem);              /* +1 (senza superare il massimo) */
k_sem_take(&sem, K_FOREVER);   /* se > 0: -1 e prosegue; se 0: si blocca finche' qualcuno da' */
```

Due usi tipici:

**Segnale** (semaforo binario, iniziale 0, massimo 1): un thread avvisa un altro che è
successo qualcosa.

```
Produttore:  ...lavora... give ..............lavora... give
Consumatore: take (bloccato) -> sveglio, elabora -> take (bloccato) -> sveglio
```

**Contatore di risorse** (iniziale N, massimo N): ci sono N risorse uguali (buffer, canali);
`take` ne prende una, `give` la restituisce. Il thread N+1 aspetta.

`k_sem_take` con timeout restituisce `-EAGAIN` se il tempo scade, `-EBUSY` con `K_NO_WAIT`
se non c'è nulla. `k_sem_count_get(&sem)` legge il valore attuale.

A differenza del mutex, **`k_sem_give` si può chiamare da un interrupt** (Modulo 5).

## 5. Mutex o semaforo?

| | Mutex | Semaforo |
|---|---|---|
| Scopo | proteggere una sezione critica | segnalare un evento / contare risorse |
| Proprietario | sì (chi lo prende lo libera) | no (uno dà, un altro prende) |
| Da interrupt | no | `give` sì |
| Ereditarietà di priorità | sì (sezione 10) | no |

Regola pratica: **dati condivisi → mutex; "è successo qualcosa" → semaforo**.

## 6. Code di messaggi (`k_msgq`)

Spesso un thread non deve solo avvisare, ma **passare dati**. La coda di messaggi è un buffer
circolare (come quello del Modulo 1) già protetto dal kernel:

```c
struct misura { uint32_t n; int32_t valore; };

K_MSGQ_DEFINE(coda, sizeof(struct misura), 8, 4);   /* elementi da sizeof(...), max 8, allineamento 4 */

struct misura m = { .n = 1, .valore = 215 };
k_msgq_put(&coda, &m, K_NO_WAIT);    /* COPIA m nella coda; -ENOMSG se piena */

struct misura r;
k_msgq_get(&coda, &r, K_FOREVER);    /* COPIA il piu' vecchio in r; si blocca se vuota */
```

- i messaggi vengono **copiati**: dopo `put` puoi riusare `m`;
- l'ordine è FIFO (primo entrato, primo uscito);
- **coda piena**: con `K_NO_WAIT` il messaggio viene rifiutato (devi decidere tu cosa fare:
  scartarlo e contarlo, oppure aspettare); con un timeout il produttore si blocca;
- `k_msgq_num_used_get` e `k_msgq_num_free_get` dicono quanto è piena;
- `put` con `K_NO_WAIT` si può fare da un interrupt.

```
Produttore --put--> [ m5 | m4 | m3 | __ | __ ] --get--> Consumatore
```

## 7. FIFO (`k_fifo`)

La FIFO passa **puntatori**, non copie: è più veloce per dati grandi, ma i dati devono restare
validi finché il consumatore non ha finito. Il primo campo della struct è **riservato al
kernel** (lo usa per collegare gli elementi in lista):

```c
struct elemento {
	void *fifo_reserved;   /* deve essere il PRIMO campo, non toccarlo */
	uint32_t n;
	char testo[16];
};

K_FIFO_DEFINE(pieni);
k_fifo_put(&pieni, &elem);                          /* inserisce l'indirizzo */
struct elemento *e = k_fifo_get(&pieni, K_FOREVER); /* restituisce l'indirizzo */
```

Da dove vengono gli elementi? Niente `malloc` (in un firmware si evita la memoria dinamica):
si usa un **pool** statico, cioè un array di elementi più una seconda FIFO dei "liberi".
Il produttore prende un elemento libero, lo riempie, lo mette in `pieni`; il consumatore lo
usa e lo rimette in `liberi`. Se i liberi finiscono, il produttore aspetta: il sistema si
autoregola.

| | `k_msgq` | `k_fifo` |
|---|---|---|
| Passa | copie | puntatori |
| Dimensione massima | fissata alla creazione | illimitata (dipende da quanti elementi esistono) |
| Memoria | la coda contiene i dati | i dati sono altrove (pool) |
| Adatta a | messaggi piccoli | blocchi grandi, buffer |

## 8. Eventi (`k_event`)

Un oggetto evento contiene 32 bit; ogni bit è una condizione ("sensore pronto", "rete
pronta"...). Un thread può aspettare **più condizioni insieme**.

```c
#define EV_SENSORE BIT(0)
#define EV_RETE    BIT(1)

K_EVENT_DEFINE(eventi);                 /* serve CONFIG_EVENTS=y */

k_event_post(&eventi, EV_SENSORE);      /* accende il bit (gli altri restano) */

uint32_t ev = k_event_wait_all(&eventi, EV_SENSORE | EV_RETE, false, K_SECONDS(5));
/* ritorna quando TUTTI i bit sono accesi (o 0 se scade il timeout) */

ev = k_event_wait(&eventi, EV_SENSORE | EV_RETE, false, K_FOREVER);
/* ritorna quando ALMENO UNO e' acceso */
```

- il terzo argomento (`reset`) a `true` azzera gli eventi *prima* di aspettare;
- `k_event_set` sostituisce tutti i bit, `k_event_clear` spegne quelli indicati;
- con un semaforo si aspetta una cosa sola; con gli eventi una combinazione.

## 9. Variabili atomiche

Per un semplice contatore o un flag, un mutex è eccessivo. Le operazioni **atomiche**
fanno lettura-modifica-scrittura in un colpo solo, senza possibilità di interruzione
(il Cortex-M4 ha istruzioni apposite, `LDREX`/`STREX`):

```c
#include <zephyr/sys/atomic.h>

atomic_t contatore = ATOMIC_INIT(0);
atomic_inc(&contatore);              /* +1 atomico */
atomic_add(&contatore, 5);
atomic_val_t v = atomic_get(&contatore);
atomic_set_bit(flags, 3);            /* anche bit singoli */
```

Bastano quando la sezione critica è **una sola operazione su una sola variabile**. Se devi
aggiornare due variabili insieme (min e max, per esempio), serve il mutex.

## 10. Inversione di priorità

Tre thread: **A**lta, **M**edia, **B**assa priorità. A e B usano la stessa risorsa.

```
tempo ->
B: [prende lucchetto]==lavora==                         ==lavora==[libera]
A:            [vuole lucchetto: BLOCCATA..........................................] [esegue]
M:                         [=========== lavora a lungo, non c'entra nulla ========]
```

A è bloccata da B (normale: B ha il lucchetto). Ma M, che ha priorità più alta di B, la
interrompe e lavora a lungo: **A aspetta M**, un thread meno importante che non usa nemmeno la
risorsa. È l'**inversione di priorità** (famosa per aver fatto riavviare in continuazione la
sonda Mars Pathfinder nel 1997).

Soluzione: **ereditarietà di priorità**. Quando A aspetta un mutex tenuto da B, il kernel
alza temporaneamente la priorità di B a quella di A; così M non può interromperla, B finisce
presto e libera.

```
B: [prende]==lavora==[sale alla priorita' di A]==lavora==[libera, torna bassa]
A:            [vuole: bloccata.....................]     [esegue]
M:                         [aspetta: B ora e' piu' importante]    [lavora]
```

Il `k_mutex` di Zephyr lo fa automaticamente; il semaforo no (non ha proprietario, quindi il
kernel non sa chi "alzare"). È un motivo in più per proteggere i dati con un mutex.

## 11. Deadlock (stallo)

Thread 1 prende il lucchetto X e aspetta Y; thread 2 ha preso Y e aspetta X: si aspettano a
vicenda per sempre. Regole per evitarlo:
- prendi i lucchetti **sempre nello stesso ordine** in tutto il programma;
- tieni un lucchetto il meno possibile e **non chiamare funzioni bloccanti** (sleep, attese su
  code) mentre lo tieni;
- usa un timeout invece di `K_FOREVER` quando un'attesa infinita sarebbe un errore.

## 12. Riepilogo: quale strumento?

| Ho bisogno di... | Uso |
|---|---|
| proteggere dati condivisi (più variabili, strutture) | `k_mutex` |
| incrementare un contatore / un flag | `atomic_t` |
| avvisare un thread che è successo qualcosa | `k_sem` (binario) |
| limitare l'uso di N risorse uguali | `k_sem` (contatore) |
| passare piccoli dati in ordine | `k_msgq` |
| passare blocchi grandi senza copiarli | `k_fifo` + pool |
| aspettare una combinazione di condizioni | `k_event` |
