# 08 — Shell, test, errori, debug e memoria (Modulo 7)

Fin qui hai scritto firmware che "funzionano". Questo modulo riguarda gli strumenti che
servono per capire **perché** un firmware non funziona, per dimostrare che funziona
(test) e per farlo stare nelle risorse di un microcontrollore.

## 1. La shell

La **shell** di Zephyr è una riga di comando che gira *sul microcontrollore*, sulla stessa
UART della console. È come il terminale di PowerShell, ma i comandi li esegue il firmware.

Si attiva con una riga in `prj.conf`:

```
CONFIG_SHELL=y
```

All'avvio compare il prompt `uart:~$ `. Comandi utili già pronti:

| Comando | Cosa mostra |
|---|---|
| `help` | elenco dei comandi; `<comando> -h` aiuto di un comando |
| `kernel version` | versione di Zephyr |
| `kernel uptime` | millisecondi dall'avvio |
| `kernel thread list` | thread, priorità, stato, uso dello stack (nelle versioni vecchie: `kernel threads`) |
| `kernel thread stacks` | uso dello stack di ogni thread (vecchie versioni: `kernel stacks`) |
| `device list` | dispositivi (driver) e se sono pronti |
| `log status`, `log disable`, `log enable inf` | controllo del logging a runtime |

Il tasto **Tab** completa i comandi, le **frecce** richiamano i comandi precedenti.

Con la shell attiva, scrivi i messaggi con `LOG_*` invece che con `printk`: il logging passa
dalla shell, che ridisegna il prompt dopo ogni messaggio; `printk` scrive direttamente
sulla UART e può "spezzare" la riga che stai digitando.

## 2. Comandi propri

Un comando è una funzione con questa firma:

```c
static int cmd_saluta(const struct shell *sh, size_t argc, char **argv)
{
	/* argv[0] = nome del comando, argv[1] ... = argomenti, argc = quanti sono */
	shell_print(sh, "Ciao, %s!", argv[1]);
	return 0;              /* 0 = ok, negativo (es. -EINVAL) = errore */
}

/* nome, sottocomandi, aiuto, funzione, argomenti obbligatori, facoltativi */
SHELL_CMD_ARG_REGISTER(saluta, NULL, "Saluta qualcuno", cmd_saluta, 2, 0);
```

- `argc` conta anche il nome del comando: `saluta Anna` → `argc = 2`. Per questo i
  "obbligatori" sono 2 (il comando + un argomento). Se il numero non torna, la shell rifiuta
  il comando da sola.
- `shell_print` aggiunge il ritorno a capo; `shell_error` stampa in rosso.
- Gli argomenti sono **stringhe**: per avere un numero serve `strtol` e controllare che la
  conversione sia andata bene (vedi esercizio 2).

**Sottocomandi** (`led on`, `led off`):

```c
SHELL_STATIC_SUBCMD_SET_CREATE(sub_led,
	SHELL_CMD(on,  NULL, "Accende il LED", cmd_led_on),
	SHELL_CMD(off, NULL, "Spegne il LED",  cmd_led_off),
	SHELL_SUBCMD_SET_END
);
SHELL_CMD_REGISTER(led, &sub_led, "Comandi del LED", NULL);
```

Le macro `SHELL_*` registrano il comando **in compilazione** (in una tabella in flash): non
serve chiamare nessuna funzione in `main`.

## 3. Test automatici con ztest

Un **test** è un pezzo di codice che chiama una funzione con valori noti e controlla il
risultato. È l'equivalente degli `assert` del corso di Python, organizzato in modo che si
possano eseguire tutti insieme e avere un riepilogo.

```c
#include <zephyr/ztest.h>

ZTEST(aritmetica, test_somma)              /* suite "aritmetica", test "test_somma" */
{
	zassert_equal(2 + 2, 4, "la somma non torna");
}

ZTEST_SUITE(aritmetica, NULL, NULL, NULL, NULL, NULL);
```

| Asserzione | Passa se |
|---|---|
| `zassert_true(cond, msg)` / `zassert_false` | la condizione è vera / falsa |
| `zassert_equal(a, b, msg)` / `zassert_not_equal` | `a == b` / `a != b` |
| `zassert_is_null(p, msg)` / `zassert_not_null` | puntatore nullo / non nullo |
| `zassert_within(a, b, delta, msg)` | `|a - b| <= delta` |
| `zassert_mem_equal(p1, p2, n, msg)` | i primi `n` byte coincidono |

- Un'applicazione di test ha `CONFIG_ZTEST=y` e **non ha `main`**: lo fornisce ztest, che
  esegue tutte le suite.
- Ogni test deve essere **indipendente**: prepara da sé i dati che gli servono (o lo fa la
  funzione `before` della suite, quarto argomento di `ZTEST_SUITE`).
- Si testa la **logica pura** (calcoli, buffer, macchine a stati) separata dall'hardware: per
  questo nel progetto finale filtro e allarme stanno in file propri, senza GPIO né sensori.

Uscita tipica:

```
START - test_somma
 PASS - test_somma in 0.001 seconds
...
SUITE PASS - 100.00% [aritmetica]: pass = 1, fail = 0, skip = 0, total = 1 ...
PROJECT EXECUTION SUCCESSFUL
```

## 4. Errori fatali

Quando qualcosa va storto in modo irrecuperabile, il kernel chiama la gestione degli
**errori fatali** e stampa un rapporto:

```
>>> ZEPHYR FATAL ERROR 3: Kernel oops on CPU 0
Current thread: 0x20000a18 (fragile)
```

Il numero è il **motivo** (`reason`):

| Codice | Nome | Quando |
|---|---|---|
| 0 | `K_ERR_CPU_EXCEPTION` | la CPU ha generato un'eccezione (istruzione non valida, accesso vietato, divisione per zero con trap...) |
| 1 | `K_ERR_SPURIOUS_IRQ` | interrupt senza gestore |
| 2 | `K_ERR_STACK_CHK_FAIL` | stack overflow rilevato |
| 3 | `K_ERR_KERNEL_OOPS` | `k_oops()`: errore grave in un thread |
| 4 | `K_ERR_KERNEL_PANIC` | `k_panic()` o assert fallito: errore di tutto il sistema |

Sui Cortex-M le eccezioni della CPU si chiamano **fault**: *HardFault*, *MemManage* (violazione
della MPU), *BusFault* (accesso a un indirizzo senza periferica), *UsageFault* (istruzione non
definita, stato non valido, divisione per zero se attivata). Il rapporto stampa i registri
`r0-r3`, `r12`, `lr` (da dove era stata chiamata la funzione) e `pc` (l'istruzione che ha
causato il fault). Con `pc` e il file `zephyr.map` (o gdb) trovi la riga colpevole.

**L'handler.** Il kernel chiama `k_sys_fatal_error_handler(reason, esf)`. Quello predefinito
ferma il sistema. Puoi scriverne uno tuo:

```c
#include <zephyr/fatal.h>

void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
{
	/* registrare l'errore, accendere un LED rosso, riavviare... */
}
```

Se il tuo handler **ritorna**, il kernel termina solo il thread colpevole e il resto del
sistema continua; se il thread è **essenziale** (come quello di `main`) il sistema si ferma
comunque. (Nelle versioni di Zephyr precedenti alla 3.7 il tipo si chiamava `z_arch_esf_t`.)

**Limite di Renode:** sulla scheda vera leggere o scrivere un indirizzo dove non c'è niente
provoca un *BusFault*; Renode invece di solito scrive un avviso nel log e restituisce 0.
Istruzioni non valide, `k_oops`, `k_panic` e gli assert si comportano come sull'hardware.

## 5. Debug con gdb

**gdb** è il debugger della toolchain (`arm-zephyr-eabi-gdb`). Si collega al bersaglio
attraverso un **server gdb**: sulla scheda vera lo fornisce la sonda JTAG/SWD, in Renode lo
apre il comando `machine StartGdbServer 3333`.

Flusso di lavoro:

```
Renode (monitor):   machine StartGdbServer 3333
Terminale:          arm-zephyr-eabi-gdb build\zephyr\zephyr.elf
(gdb)               target remote :3333
(gdb)               break main
(gdb)               monitor start          <- avvia l'emulazione da gdb
(gdb)               continue
```

| Comando gdb | Effetto |
|---|---|
| `break funzione` / `break file.c:42` (`b`) | punto di arresto |
| `continue` (`c`) | riprendi fino al prossimo arresto |
| `next` (`n`) | riga successiva, senza entrare nelle funzioni |
| `step` (`s`) | riga successiva, entrando nelle funzioni |
| `finish` | esegui fino all'uscita dalla funzione corrente |
| `print espr` (`p`) | valore di una variabile o espressione: `p stato.soglia`, `p &x`, `p/x reg` |
| `x/6dw indirizzo` | mostra 6 parole (w = 4 byte) in decimale a partire dall'indirizzo |
| `watch espr` | ferma quando il valore **cambia** (watchpoint) |
| `info locals` / `info breakpoints` | variabili locali / punti di arresto |
| `bt` | *backtrace*: catena delle chiamate |
| `delete N` | toglie il punto di arresto N |
| `quit` | esce |

Il compilatore, per risparmiare spazio, riordina e toglie codice: alcune variabili risultano
`<optimized out>` e `next` "salta" in modo strano. Con `CONFIG_DEBUG_OPTIMIZATIONS=y` usa
`-Og`, un livello di ottimizzazione pensato per il debug.

## 6. Memoria: quanto occupo?

Ricorda il Modulo 1: il codice e le costanti vanno in **flash** (`.text`, `.rodata`), le
variabili in **RAM** (`.data`, `.bss`, stack dei thread, heap). Alla fine di ogni
compilazione west stampa:

```
Memory region   Used Size  Region Size  %age Used
        FLASH:     18264 B         1 MB      1.74%
          RAM:      6544 B       256 KB      2.50%
```

Per sapere **chi** occupa cosa:

```powershell
west build -d modulo7\06_memoria\build -t rom_report    # flash, per file e funzione
west build -d modulo7\06_memoria\build -t ram_report    # RAM, per file e variabile
```

**Gli stack** sono spesso la voce più grande della RAM. Si dimensionano così:
1. si parte larghi;
2. si misura l'uso *reale* nel caso peggiore con il **thread analyzer**
   (`CONFIG_THREAD_ANALYZER=y`): Zephyr riempie lo stack di un valore noto all'avvio
   (`CONFIG_INIT_STACKS`) e poi conta quanto ne è stato sovrascritto;
3. si imposta la dimensione = uso misurato + **margine** (tipicamente 25-50%), arrotondando.

Uno stack troppo piccolo non dà errori di compilazione: sfora in silenzio nella memoria vicina
o, se la protezione è attiva, provoca un errore fatale (motivo 2).

## 7. Flash e settings

La **flash** è diversa dalla RAM:
- si **legge** come la RAM;
- si **scrive** solo trasformando bit da 1 a 0;
- per riportare i bit a 1 si **cancella** un'intera pagina (sul nRF52840: 4 KB) alla volta;
- ogni pagina sopporta un numero limitato di cancellazioni (10.000 circa).

Per questo non si salva "una variabile in flash" direttamente: si usa un file system a
chiave-valore come **NVS** (o il più recente **ZMS**), che scrive i dati in coda, distribuisce
l'usura e sopravvive a uno spegnimento a metà scrittura.

Sopra NVS c'è il sottosistema **settings**: valori identificati da un nome tipo
`"nodo/soglia"`.

```c
settings_save_one("nodo/soglia", &soglia, sizeof(soglia));    /* salva */
settings_load();      /* rilegge tutto e chiama gli handler registrati */
```

Gli **handler** si registrano per un prefisso (`"nodo"`) e ricevono ogni valore trovato:

```c
static int nodo_set(const char *nome, size_t len, settings_read_cb leggi, void *arg)
{
	if (strcmp(nome, "soglia") == 0 && len == sizeof(soglia)) {
		return leggi(arg, &soglia, sizeof(soglia)) == sizeof(soglia) ? 0 : -EIO;
	}
	return -ENOENT;
}
SETTINGS_STATIC_HANDLER_DEFINE(nodo, "nodo", NULL, nodo_set, NULL, NULL);
```

**In Renode:** la piattaforma nRF52840 non simula il controller della flash (NVMC), quindi
la flash vera non si può cancellare né scrivere come sulla scheda. Usiamo il **flash
simulator** di Zephyr: un "finto chip di flash" che vive in RAM, dichiarato nel devicetree
(`app.overlay`). Il codice dell'applicazione è **identico** a quello per la flash vera (cambia
solo l'overlay); il limite è che i dati si perdono con `machine Reset`, perché la RAM viene
reinizializzata.

## 8. Riassunto: quale strumento per quale problema

| Sintomo | Strumento |
|---|---|
| "voglio provare una funzione a mano" | comando della shell |
| "voglio essere sicuro che la logica sia giusta, anche dopo modifiche" | ztest |
| "il firmware si pianta con un errore" | rapporto dell'errore fatale + `pc` + gdb |
| "una variabile cambia valore e non so chi la tocca" | gdb `watch` |
| "si comporta in modo strano dopo un po'" | thread analyzer (stack), log |
| "non ci sta in memoria" | `rom_report`, `ram_report`, analyzer |
