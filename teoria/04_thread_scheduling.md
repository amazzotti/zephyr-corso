# 04 — Thread e scheduling (Modulo 3)

## 1. Perché i thread

Un firmware deve fare più cose con ritmi diversi: leggere un sensore ogni secondo, far
lampeggiare un LED ogni 250 ms, rispondere subito a un pulsante, scrivere sulla UART. In un
unico ciclo infinito questi tempi si intrecciano e il codice diventa un groviglio di
contatori.

Con un RTOS ogni attività diventa un **thread**: una funzione con il suo ciclo infinito,
scritta come se fosse sola al mondo. Il **kernel** decide chi esegue in ogni momento.

La CPU è **una sola** (il Cortex-M4 ha un solo core): i thread non girano davvero in
parallelo, si **alternano** molto velocemente. Il "parallelo" è un'illusione organizzata
dallo **scheduler**.

## 2. Com'è fatto un thread

| Parte | Cos'è |
|---|---|
| **funzione di ingresso** (*entry point*) | il codice che il thread esegue: `void lavoro(void *p1, void *p2, void *p3)` |
| **stack** | un'area di RAM tutta sua per variabili locali, parametri e indirizzi di ritorno |
| **priorità** | un numero: più è **piccolo**, più il thread è importante |
| **stato** | pronto, in esecuzione, in attesa, sospeso, terminato (sezione 5) |
| **struttura `k_thread`** | dove il kernel salva tutto ciò che serve per fermarlo e riprenderlo |

**Cambio di contesto** (*context switch*): quando il kernel passa da un thread A a un thread
B, salva i **registri della CPU** di A (il contatore di programma `PC`, il puntatore allo
stack `SP`, i registri `R0`-`R12`...) e carica quelli di B. B riparte esattamente dove si
era fermato. Sul Cortex-M questo lo fa un'eccezione apposita (*PendSV*); costa pochi
microsecondi.

Creazione statica (il modo più comune):

```c
#define STACK_A 1024
#define PRIO_A  5

void thread_a(void *p1, void *p2, void *p3)
{
	while (1) {
		printk("A\n");
		k_msleep(500);
	}
}

/* nome, stack, funzione, p1, p2, p3, priorita', opzioni, ritardo di partenza (ms) */
K_THREAD_DEFINE(id_a, STACK_A, thread_a, NULL, NULL, NULL, PRIO_A, 0, 0);
```

`K_THREAD_DEFINE` riserva lo stack in RAM in compilazione e fa partire il thread all'avvio.
I parametri `p1, p2, p3` non usati si ignorano con `ARG_UNUSED(p1);`.

Anche `main()` gira in un thread (il **main thread**, priorità 0 di default). Se `main`
ritorna, finisce solo quel thread: gli altri continuano.

## 3. Priorità: cooperative e preemptive

In Zephyr le priorità sono numeri interi:

```
  -16 ...  -1   |   0   1   2  ...  14   |  15
  cooperative   |      preemptive        |  idle
  (più importanti)                         (il meno importante)
```

- **Preemptive** (`>= 0`): il thread può essere **interrotto in qualunque momento** se diventa
  pronto un thread più importante. È il caso normale.
- **Cooperative** (`< 0`): il thread **non viene mai interrotto** da altri thread; tiene la CPU
  finché non la cede lui (dormendo, aspettando, o con `k_yield`). Si usa per codice breve che
  non deve essere spezzato; un thread cooperativo che non cede blocca tutto.
- **Idle** (la priorità più bassa): il thread che gira quando nessun altro è pronto; mette la
  CPU a dormire (istruzione `WFI`) per risparmiare energia.

Gli interrupt (Modulo 5) sono "sopra" a tutti i thread: interrompono anche i cooperativi.

## 4. Le regole dello scheduler

1. Esegue sempre il thread **pronto** con priorità **più alta** (numero più piccolo).
2. Tra thread della stessa priorità: chi è pronto da più tempo (coda FIFO).
3. Un thread preemptive viene interrotto appena un thread più importante diventa pronto
   (per esempio perché è finita la sua `k_msleep`).
4. Un thread più importante che non si ferma mai **affama** (*starvation*) tutti quelli meno
   importanti: non gireranno mai.

Conseguenza pratica: **un thread deve passare la maggior parte del tempo ad aspettare**
(dormire, aspettare un dato, un evento). Un thread che calcola senza sosta è quasi sempre
un errore di progetto.

`k_yield()` cede la CPU **solo** ai thread pronti di priorità **uguale o più alta**: non aiuta
quelli meno importanti. Per lasciarli girare bisogna aspettare davvero (`k_msleep`).

## 5. Gli stati di un thread

```
             crea                 lo scheduler lo sceglie
 (prestart) ──────► PRONTO ◄──────────────────────────────► IN ESECUZIONE
                      ▲        un piu' importante lo           │
                      │        interrompe / k_yield            │ k_msleep, k_sem_take,
                      │                                        │ attesa di un dato...
                      └──────────── IN ATTESA ◄────────────────┘
                       tempo scaduto / dato arrivato     (pending / sleeping)

 SOSPESO: k_thread_suspend() lo toglie dallo scheduling finché k_thread_resume().
 TERMINATO (dead): la funzione e' ritornata o k_thread_abort().
```

Solo i thread **pronti** competono per la CPU. Un thread in attesa non consuma CPU.

## 6. Dormire o aspettare attivamente

| | `k_msleep(100)` | `k_busy_wait(100000)` |
|---|---|---|
| cosa fa | il thread va **in attesa** per 100 ms | la CPU gira a vuoto per 100 000 µs |
| altri thread | **girano** | **non girano** (se meno importanti) |
| consumo | la CPU può dormire | la CPU lavora inutilmente |
| quando si usa | quasi sempre | attese brevissime (microsecondi) richieste dall'hardware |

**Il tempo del kernel.** Sullo STM32F4 il tempo lo misura il **SysTick**, un timer a 24 bit
integrato in ogni core Cortex-M, che conta i cicli del clock della CPU (168 MHz). Funzioni
utili:
- `k_uptime_get()`: millisecondi dall'avvio (`int64_t`);
- `k_cycle_get_32()`: il contatore hardware grezzo (qui 168 000 000 cicli al secondo, dato
  da `sys_clock_hw_cycles_per_sec()`; un contatore a 32 bit riparte da 0 ogni ~25,6 s);
- `k_cyc_to_us_floor32(cicli)`: conversione in microsecondi.

Zephyr è *tickless*: non c'è un interrupt periodico fisso; il kernel riprogramma il SysTick
per svegliarsi alla prossima scadenza (il tick, `CONFIG_SYS_CLOCK_TICKS_PER_SEC` = 10 000
qui, resta l'unità di misura dei tempi). Meno interrupt, meno consumo.

## 7. Thread creati a runtime e parametri

Quando il numero di thread o i loro parametri si decidono durante l'esecuzione:

```c
#define STACK 1024
K_THREAD_STACK_DEFINE(stack_w, STACK);   /* lo stack resta statico: va riservato */
static struct k_thread thr_w;            /* il "descrittore" del thread */

k_tid_t tid = k_thread_create(&thr_w, stack_w, K_THREAD_STACK_SIZEOF(stack_w),
			      lavoro, (void *)(intptr_t)42, NULL, NULL,
			      5, 0, K_NO_WAIT);
k_thread_join(&thr_w, K_FOREVER);        /* aspetta che termini */
```

- per più thread uguali: `K_THREAD_STACK_ARRAY_DEFINE(stacks, N, STACK)` e un array di
  `struct k_thread`;
- i parametri sono `void *` (puntatori "generici"). Per passare un numero si fa la
  conversione `(void *)(intptr_t)n` e nel thread `int n = (intptr_t)p1;`. Per passare più
  dati si passa il puntatore a una `struct`, che deve **restare valida** finché il thread la
  usa (non una variabile locale di una funzione già terminata!);
- la struttura `k_thread` e lo stack non si possono riusare finché il thread non è terminato.

## 8. Time slicing

Con più thread **della stessa priorità** sempre pronti (che calcolano senza dormire), senza
time slicing il primo tiene la CPU finché non si ferma. Con il **time slicing** il kernel li
fa girare a turno, una "fetta" di tempo ciascuno:

```
CONFIG_TIMESLICING=y        # meccanismo disponibile (di solito gia' y)
CONFIG_TIMESLICE_SIZE=20    # durata della fetta in ms (0 = disattivato)
CONFIG_TIMESLICE_PRIORITY=0 # vale per i thread di priorita' >= di questa
```

Il time slicing non cambia le regole delle priorità: un thread più importante vince sempre.
Divide solo il tempo tra pari.

## 9. Lo stack

Lo stack di un thread contiene: le **variabili locali** delle funzioni in corso, i parametri,
gli indirizzi di ritorno, i registri salvati durante un cambio di contesto o un interrupt.
Ogni chiamata di funzione aggiunge un "piano" (*stack frame*); al ritorno lo toglie.
Sul Cortex-M lo stack **cresce verso il basso** (verso indirizzi più piccoli).

```
indirizzi alti   ┌──────────────────┐ inizio dello stack
                 │ frame di thread()│
                 │ frame di f()     │
                 │ frame di g()     │ ◄── SP (punta qui adesso)
                 │   (libero)       │
indirizzi bassi  └──────────────────┘ fine: se lo superi -> STACK OVERFLOW
                   altra memoria (stack di un altro thread, variabili...)
```

In un microcontrollore non c'è memoria virtuale che "si allarga": ogni stack ha una
dimensione **fissa** decisa in compilazione. Troppo piccolo → **stack overflow**: il thread
scrive sulla memoria di qualcun altro, con effetti casuali e difficili da capire.
Troppo grande → RAM sprecata (sono solo 128 KB di RAM principale per tutto).

Come si dimensiona: si parte largo, si **misura** l'uso reale con il **thread analyzer**
(`CONFIG_THREAD_ANALYZER=y`, che riempie lo stack di un valore noto all'avvio e poi conta
quanto ne è stato sovrascritto), si lascia un margine (es. 25-30%).

Come si scopre un overflow:
- **stack sentinel** (`CONFIG_STACK_SENTINEL=y`): il kernel scrive un valore "sentinella"
  in fondo a ogni stack e lo ricontrolla a ogni cambio di contesto; se è stato sovrascritto,
  errore fatale "stack overflow". Solo software: funziona ovunque, ma se ne accorge *dopo*;
- **protezione hardware** (`CONFIG_HW_STACK_PROTECTION`): la **MPU** (*Memory Protection
  Unit*) del Cortex-M4 vieta la scrittura appena sotto lo stack, e l'overflow genera subito
  un'eccezione. Più affidabile sulla scheda vera; in Renode il modello della MPU non è
  garantito, quindi nel corso usiamo la sentinella.

Le funzioni **ricorsive** e gli **array locali grandi** sono i nemici dello stack: in embedded
si evitano o si controllano con cura.

## 10. I thread che ci sono già

Anche un'applicazione "vuota" ha alcuni thread di sistema:

| Thread | Priorità | Ruolo |
|---|---|---|
| `main` | 0 | esegue `main()` |
| `idle` | la più bassa | gira quando nessuno è pronto, fa dormire la CPU |
| `sysworkq` | -1 (cooperativo) | la *work queue* di sistema (Modulo 5) |
| `logging` | bassa | stampa i messaggi del logging deferred (se `CONFIG_LOG=y`) |

Con `CONFIG_THREAD_NAME=y` ogni thread ha un nome (per `K_THREAD_DEFINE` è il nome dato
come primo argomento) e gli strumenti di debug lo mostrano.
