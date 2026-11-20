# 06 — Tempo, interrupt e lavoro differito (Modulo 5)

## 1. Come il kernel misura il tempo

Il kernel conta il tempo in **tick**: unità fisse generate da un timer hardware. Sul nRF52840
Zephyr usa la periferica **RTC1**, che conta a 32768 Hz (un quarzo da orologio): un tick dura
circa 30,5 microsecondi (`CONFIG_SYS_CLOCK_TICKS_PER_SEC=32768`).

Zephyr è **tickless**: non riceve un interrupt a ogni tick, ma programma il timer hardware
perché scatti solo quando serve (la prossima scadenza). Così la CPU dorme di più e consuma
meno.

| Funzione | Restituisce |
|---|---|
| `k_uptime_get()` | millisecondi dall'avvio (`int64_t`) |
| `k_uptime_get_32()` | come sopra, a 32 bit (riparte da 0 dopo ~49 giorni) |
| `k_uptime_delta(&rif)` | ms trascorsi da `rif`, e aggiorna `rif` |
| `k_cycle_get_32()` | contatore ad alta risoluzione (`sys_clock_hw_cycles_per_sec()` al secondo) |

I **timeout** (`K_MSEC(100)`) vengono arrotondati ai tick: 100 ms = 3276,8 tick, quindi
nella realtà 3277 tick. Piccoli errori, che però si **accumulano** (sezione 9).

## 2. Interrupt

Un **interrupt** è un segnale con cui una periferica dice alla CPU "è successo qualcosa,
occupatene ora": il timer è scaduto, è arrivato un byte sulla UART, è stato premuto un
pulsante.

Quando arriva:
1. la CPU **interrompe** quello che stava facendo (qualunque thread), salva i registri;
2. il controller degli interrupt (sul Cortex-M si chiama **NVIC**) sceglie la funzione
   associata a quell'interrupt: la **ISR** (*Interrupt Service Routine*);
3. la ISR viene eseguita;
4. al termine si torna al thread interrotto, oppure lo scheduler passa a un thread più
   importante che la ISR ha reso pronto.

```
thread:  ======interrotto           ===ripresa (o un altro thread)===
ISR:                    [ISR breve]
                        ^ interrupt del timer
```

Gli interrupt hanno priorità tra loro (uno più importante può interrompere un altro), e
**tutti** hanno precedenza su **tutti** i thread.

## 3. Contesto di interrupt contro contesto thread

Il codice di una ISR gira in **contesto di interrupt**: non appartiene a nessun thread. Ne
seguono regole precise:

| Si può | Non si può |
|---|---|
| leggere/scrivere variabili (meglio `atomic_t` o `volatile`) | **bloccarsi**: `k_msleep`, `k_mutex_lock`, `k_sem_take(..., K_FOREVER)` |
| `k_sem_give`, `k_event_post`, `k_msgq_put(..., K_NO_WAIT)` | usare un mutex |
| `k_work_submit` | fare elaborazioni lunghe (blocca tutto il sistema) |
| `printk` (con moderazione), `gpio_pin_toggle_dt` | `LOG_*` va bene, ma non abusarne |

Perché non bloccarsi? Bloccarsi significa "addormenta **questo thread** e passa a un altro".
Ma una ISR non è un thread: non c'è niente da addormentare. Con `CONFIG_ASSERT=y` Zephyr se ne
accorge e si ferma con un messaggio `ASSERTION FAIL`; senza assert il comportamento è
imprevedibile.

`k_is_in_isr()` restituisce vero se il codice sta girando in contesto di interrupt.

Regola d'oro: **la ISR fa il minimo indispensabile** (salva il dato, avvisa qualcuno) e il
lavoro vero lo fa un thread.

## 4. Timer del kernel (`k_timer`)

Un `k_timer` chiama una funzione dopo un certo tempo, una volta (*one-shot*) o
periodicamente. La funzione (*expiry function*) gira **in contesto di interrupt** (è chiamata
dalla ISR del timer di sistema).

```c
static void scaduto(struct k_timer *t)   /* contesto ISR! */
{
	/* breve */
}
static void fermato(struct k_timer *t)   /* chiamata da k_timer_stop (puo' essere NULL) */
{
}

K_TIMER_DEFINE(mio_timer, scaduto, fermato);

k_timer_start(&mio_timer, K_MSEC(500), K_MSEC(500));   /* prima scadenza, poi periodo */
k_timer_start(&mio_timer, K_SECONDS(2), K_NO_WAIT);    /* one-shot: periodo K_NO_WAIT */
k_timer_stop(&mio_timer);
```

- `k_timer_status_get(&t)`: quante volte è scaduto dall'ultima lettura (e azzera il conteggio);
- `k_timer_status_sync(&t)`: blocca il thread fino alla prossima scadenza (utile per cicli
  periodici precisi, sezione 9);
- `k_timer_user_data_set/get`: associa un puntatore al timer, per passare dati alla callback.

## 5. Svegliare un thread da un interrupt

Lo schema più comune (*top half / bottom half*):

```
ISR (top half):       salva il dato, k_sem_give(&sem)        <- brevissima
Thread (bottom half): while (1) { k_sem_take(&sem, K_FOREVER); elabora... }
```

```
timer:   ^              ^              ^
ISR:     [give]         [give]         [give]
thread:  ...[elabora]......[elabora]......[elabora]   (dorme tra uno e l'altro)
```

Il thread ha una sua priorità, può bloccarsi, usare mutex, dormire: tutto ciò che la ISR non
può fare.

## 6. Work queue di sistema

Creare un thread per ogni piccolo lavoro differito costa memoria (ogni thread ha il suo
stack). Zephyr ha già un thread pronto, la **work queue di sistema** (nome `sysworkq`), a cui
si consegnano "lavori" da eseguire:

```c
static void mio_handler(struct k_work *w)   /* gira nel thread sysworkq */
{
	/* qui si puo' fare di piu' che in una ISR */
}

K_WORK_DEFINE(mio_lavoro, mio_handler);

k_work_submit(&mio_lavoro);   /* da ISR o da thread: "eseguilo appena puoi" */
```

- `k_work_submit` restituisce 1 se il lavoro è stato messo in coda, 0 se **era già in coda**
  (non viene duplicato: due submit ravvicinati = una sola esecuzione), 2 se era in esecuzione
  ed è stato rimesso in coda;
- i lavori della coda vengono eseguiti **uno alla volta, in ordine**: se un handler si blocca
  a lungo, tutti gli altri aspettano (sezione 8);
- per passare dati all'handler si mette la `struct k_work` dentro una struct propria e si
  risale alla struct con `CONTAINER_OF(w, struct mia, lavoro)`.

## 7. Lavoro ritardato (`k_work_delayable`)

Un lavoro da eseguire **tra un po'**:

```c
K_WORK_DELAYABLE_DEFINE(timeout_lavoro, handler);

k_work_schedule(&timeout_lavoro, K_SECONDS(3));    /* se gia' programmato, NON cambia la scadenza */
k_work_reschedule(&timeout_lavoro, K_SECONDS(3));  /* sposta sempre la scadenza a "tra 3 s da ora" */
k_work_cancel_delayable(&timeout_lavoro);          /* annulla */
```

Lo schema **timeout** (o *watchdog software*): ogni volta che arriva un dato si chiama
`k_work_reschedule(..., K_SECONDS(3))`. Finché i dati arrivano, la scadenza viene spostata in
avanti e l'handler non parte mai. Se i dati smettono per 3 secondi, l'handler parte: "allarme".

```
dati:      d    d    d    d                    (stop)
scadenza:  -->3s -->3s -->3s -->3s ........... ALLARME
```

Altro uso: il **debounce** di un pulsante (Modulo 6).

## 8. Una work queue propria

La work queue di sistema è condivisa da tutto Zephyr (anche da driver e sottosistemi). Un
lavoro lento (una lettura I2C che dura 50 ms, una scrittura in flash) li ritarda tutti.
Soluzione: una coda dedicata, con il suo thread, il suo stack e la priorità che scegli tu.

```c
K_THREAD_STACK_DEFINE(stack_coda, 1024);
static struct k_work_q coda_lenta;

struct k_work_queue_config cfg = { .name = "coda_lenta" };
k_work_queue_init(&coda_lenta);
k_work_queue_start(&coda_lenta, stack_coda, K_THREAD_STACK_SIZEOF(stack_coda), 10, &cfg);

k_work_submit_to_queue(&coda_lenta, &lavoro_lento);
k_work_schedule_for_queue(&coda_lenta, &ritardato, K_MSEC(100));
```

## 9. Precisione: deriva e jitter

Ciclo "ingenuo":

```c
while (1) {
	lavora();          /* dura 7 ms */
	k_msleep(100);     /* poi aspetta 100 ms */
}
```

Il periodo reale è 107 ms (più gli arrotondamenti dei tick): dopo 50 giri si è in ritardo di
350 ms, dopo un'ora di quasi 2 minuti. È la **deriva** (*drift*): l'errore si accumula.

Con un timer periodico le scadenze sono fissate fin dall'inizio (0, 100, 200, ... ms), e il
tempo di lavoro non le sposta:

```c
k_timer_start(&t, K_MSEC(100), K_MSEC(100));
while (1) {
	k_timer_status_sync(&t);   /* aspetta la prossima scadenza */
	lavora();
}
```

Ogni singolo risveglio può arrivare con un piccolo ritardo variabile (**jitter**: il thread
deve aspettare che finiscano interrupt o thread più importanti), ma l'errore non si
accumula. In alternativa si può dormire fino a un istante **assoluto**:
`k_sleep(K_TIMEOUT_ABS_MS(prossimo))`, con `prossimo += 100` a ogni giro.

## 10. Il tempo in Renode

Renode simula il timer RTC1 della scheda, quindi Zephyr "vede" il tempo come su una scheda
vera: le misure fatte con `k_uptime_get` sono coerenti tra loro e le differenze (deriva,
ritardi, ordine degli eventi) si osservano davvero.

Però è **tempo virtuale**: quando la CPU dorme, Renode può far scorrere il tempo più veloce
dell'orologio di casa; quando simula molto codice, più lento. Quindi:
- confronta tempi **tra loro** (k_msleep contro k_timer), non con il cronometro del telefono;
- `k_busy_wait` e i cicli di calcolo consumano tempo virtuale come sulla scheda vera,
  ma a velocità reale diversa;
- le verifiche automatiche controllano ordine e contenuto delle righe, non i secondi esatti.
