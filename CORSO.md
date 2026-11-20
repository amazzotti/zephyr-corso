# Quaderno del corso di Zephyr RTOS

Documento di continuità: raccoglie obiettivo, metodo, piano completo e stato di avanzamento.
Chi lo legge (io stesso, un tutor, un collega) deve poter capire
**cosa è stato fatto, cosa no e qual è il prossimo passo** senza altre informazioni.
Si aggiorna alla fine di ogni sessione (sezione 7).

---

## 1. Obiettivo e punto di partenza

- Imparo a programmare firmware con **Zephyr RTOS**: linguaggio C per l'embedded, kernel
  (thread, sincronizzazione, tempo), build system (CMake, Kconfig, devicetree), driver e
  periferiche, strumenti (shell, log, test, debug).
- **Nessun hardware**: tutto gira su una scheda **nRF52840 DK simulata con Renode**.
  Il firmware è lo stesso che andrebbe sulla scheda vera.
- Arrivare a un **progetto completo** costruito da me: un nodo di monitoraggio della
  temperatura (sensore I2C, thread, code di messaggi, allarme con LED e pulsante, shell,
  configurazione salvata, watchdog, test automatici).
- Tre obiettivi insieme: **basi dei microcontrollori** (memoria, registri, periferiche,
  interrupt), **RTOS** (thread, scheduling, sincronizzazione, tempo) e **C embedded**.
- Livello di partenza: ho già fatto un corso di Python; il C si impara dalle basi, sempre
  dal punto di vista dell'embedded (Modulo 1, completo). Spiegare ogni termine alla prima
  occorrenza.
- Lingua: **italiano**. Spiegazioni brevi, concrete, con esempio di codice.

## 2. Metodo di lavoro (da rispettare)

1. Si introduce **un concetto alla volta**, in poche righe, con un esempio.
2. Segue **un esercizio piccolo**. Il codice lo scrivo **io**, non chi mi guida.
3. Si compila, si esegue in Renode, si leggono insieme gli errori (di compilazione e a
   runtime). Se sono bloccato: prima un **suggerimento**, poi, solo se serve, la soluzione
   commentata.
4. La teoria (scheduling, interrupt, devicetree...) entra **quando serve**, sempre accanto a
   codice che la usa.
5. Ogni esercizio è una **cartella già creata** da chi guida. In `src/main.c` c'è in testa la
   consegna completa (obiettivo, passi, output atteso, concetti nuovi, come verificare, errori
   tipici, bonus); sotto c'è uno scheletro che compila già, e il codice lo scrivo io.
6. Ogni esercizio ha un file `verifica.txt`: con `.\verifica.ps1 <cartella>` il firmware
   viene compilato, eseguito in Renode e controllato **da solo** (come gli `assert` del corso
   di Python).
7. A fine sessione: commit, push, e aggiornamento delle sezioni 7 e 8.

## 3. Ambiente

| Cosa | Dove / come |
|---|---|
| Workspace Zephyr (sorgenti, moduli, ambiente Python) | `C:\zephyrproject` (fuori dal repo: pesa diversi GB) |
| Zephyr SDK (compilatore `arm-zephyr-eabi`) | installato con `west sdk install` |
| Simulatore | Renode, installato da https://github.com/renode/renode/releases |
| Scheda | `nrf52840dk/nrf52840` (nRF52840 DK, Cortex-M4F, 1 MB flash, 256 KB RAM) |
| Scheda simulata | [renode/nrf52840dk.repl](renode/nrf52840dk.repl): CPU + 4 LED + 4 pulsanti + sensore BME280 su I2C |
| Editor | VS Code con estensioni C/C++ e CMake |

Comandi (da PowerShell, nella cartella del repo):

```powershell
. .\attiva.ps1                        # attiva l'ambiente Zephyr (una volta per terminale)
.\esegui.ps1 modulo0\02_hello         # compila e apre in Renode (finestra con la UART)
.\esegui.ps1 modulo0\02_hello -Pulito # ricompila da zero (dopo modifiche a Kconfig/overlay)
.\verifica.ps1 modulo0\02_hello       # compila, esegue senza finestre e controlla l'output
```

Dettagli del setup: Modulo 0 e [teoria/01_setup_toolchain.md](teoria/01_setup_toolchain.md).

### Limiti noti della simulazione (da tenere presenti)

- La piattaforma Renode `nrf52840` **non** modella NVMC (controller della flash), SAADC
  (convertitore analogico) e PWM. Per questo il corso non usa ADC e PWM, e per salvare dati
  in flash (Modulo 7) usa il **flash simulator** di Zephyr, che vive in RAM: le API sono
  quelle vere, ma i dati non sopravvivono a un reset completo.
- Il modello I2C di Renode **non gestisce il repeated start**: `i2c_write_read`,
  `i2c_reg_read_byte`, `i2c_burst_read` (e le versioni `_dt`) restituiscono 0 ma con dati
  sbagliati. Funzionano invece due transazioni separate, `i2c_write` + `i2c_read`. Per
  questo il driver ufficiale di Zephyr per il BME280 (sensor API) in Renode non si
  inizializza, e nel corso il driver del sensore lo scrivi tu (Modulo 6, esercizio 7).
  Dettagli: [teoria/07_periferiche_driver.md](teoria/07_periferiche_driver.md), sezione 7.
- Nei pulsanti simulati non c'è il rimbalzo meccanico: lo si simula a mano (Modulo 6).
- Un LED che il firmware **non ha mai configurato** in Renode risulta acceso (`State` =
  `True`): il pin parte da 0 e i LED della DK si accendono con il pin a 0. Sulla scheda vera
  resterebbe spento. Le verifiche controllano solo LED configurati dal firmware.
- I tempi in Renode sono tempi *virtuali*: la misura di prestazioni assolute non ha senso,
  quella di tempi relativi e dell'ordine degli eventi sì.

## 4. Piano completo dei moduli

### Modulo 0 — Setup e primo firmware
- Git, Python, CMake, Ninja, `west`, Zephyr, SDK, Renode. Primo firmware che stampa sulla
  UART. Uso del monitor di Renode. Ciclo infinito di un firmware.
- Concetti: cross-compilazione, toolchain, file ELF, board, UART, console, simulatore.

### Modulo 1 — C per l'embedded
- Tipi a larghezza fissa, operatori sui bit, array e stringhe, puntatori, `struct`, `enum` e
  macchine a stati, `static`/`const`/macro, file multipli e header, `volatile` e registri,
  mappa di memoria (flash, RAM, sezioni, stack), buffer circolare. Tutti gli esercizi girano sulla scheda simulata e stampano con `printk`.

### Modulo 2 — Come si costruisce un'applicazione Zephyr
- `CMakeLists.txt`, `prj.conf` e Kconfig (anche opzioni proprie), logging, **devicetree**
  (lettura di `zephyr.dts`, macro `DT_*`, overlay), primo LED con `gpio_dt_spec`, menuconfig,
  `__ASSERT`.

### Modulo 3 — Thread e scheduling
- Thread statici e dinamici, priorità cooperative e preemptive, `k_yield`, sleep contro
  attesa attiva, time slicing, stati di un thread, stack e suo dimensionamento.

### Modulo 4 — Sincronizzazione e comunicazione
- Race condition, mutex, semafori binari e contatori, code di messaggi (`k_msgq`), FIFO,
  eventi, variabili atomiche, inversione di priorità. Pipeline produttore/elaboratore/consumatore.

### Modulo 5 — Tempo e lavoro differito
- `k_timer`, contesto di interrupt contro contesto thread, work queue di sistema e proprie,
  lavoro ritardato (`k_work_delayable`), timeout, jitter.

### Modulo 6 — Periferiche e driver
- GPIO in ingresso (polling e interrupt), debounce, UART in polling e a interrupt, I2C
  "grezzo" (lettura di registri), un driver del BME280 scritto da me (con la formula di
  compensazione del datasheet; la sensor API di Zephyr come concetto), watchdog.

### Modulo 7 — Strumenti da professionista
- Shell (comandi di sistema e comandi propri), `ztest`, errori fatali e `__ASSERT`, debug con
  **gdb collegato a Renode**, thread analyzer e report di memoria, settings su flash.

### Modulo 8 — Progetto finale: nodo di monitoraggio
- Specifica e architettura scritte da me, poi costruzione a passi di un'unica applicazione
  (`modulo8/progetto/`): acquisizione, elaborazione con soglie e isteresi, allarme, shell,
  configurazione, watchdog, test e relazione finale.

## 5. Struttura del repository

```
zephyr-corso/
  CORSO.md            questo quaderno
  README.md           descrizione breve
  attiva.ps1          attiva l'ambiente Zephyr nel terminale
  esegui.ps1          compila un esercizio e lo apre in Renode
  verifica.ps1        compila, esegue e controlla l'output di un esercizio
  strumenti/          funzioni comuni degli script
  renode/             scheda simulata (.repl) e script di avvio (.resc)
  teoria/             appunti, un file per modulo
  modulo0/ ... modulo8/
     NN_nome/         un esercizio = una applicazione Zephyr
        CMakeLists.txt
        prj.conf
        src/main.c    consegna in testa + il mio codice
        verifica.txt  controlli automatici
        (app.overlay, Kconfig, altri .c/.h quando servono)
```

## 6. Regole per commit, file e convenzioni

- Messaggi di commit brevi, in italiano, in prima persona, ad esempio
  "Svolgo esercizio sui puntatori".
- Prima di ogni push: `git status`, controllare che nessuna cartella `build/` sia inclusa.
- Prima di iniziare: `git pull`. A fine sessione: `git add`, `git commit`, `git push`.

### Convenzioni degli esercizi (per chi li prepara)

- **Consegna** in testa a `src/main.c`, in un commento `/* ... */` con queste sezioni, nello
  stesso stile del corso di Python:
  `Esercizio N (Modulo M) - Titolo`, `Teoria: teoria/XX.md, sezione Y`, `OBIETTIVO`, `PASSI`,
  `OUTPUT ATTESO`, `CONCETTI NUOVI`, `COME VERIFICARE DA SOLO`, `ERRORI TIPICI`,
  `BONUS (facoltativo)`, e in fondo `Scrivi il tuo codice qui sotto.`
- Sotto il commento c'è uno **scheletro che compila** (include e `main` vuoto o con i soli
  punti da completare segnati `/* TODO: ... */`). Mai la soluzione. Gli avvisi
  `defined but not used` che compaiono compilando lo scheletro riguardano variabili già
  preparate per l'esercizio: spariscono quando le usi.
- Collaudo del 20/11/2026: tutti gli scheletri compilano con Zephyr v4.5.0 e SDK 1.0.1.
- `prj.conf` contiene solo le opzioni necessarie, ciascuna con un commento. Se l'esercizio
  chiede di aggiungere opzioni, lo dice la consegna.
- Esercizi senza codice (installazione, esplorazione) sono file `.md` nella cartella del
  modulo, con domande a cui rispondere scrivendo nel file stesso.
- `verifica.txt` usa i comandi letti da `verifica.ps1` (una istruzione per riga):

  | Riga | Significato |
  |---|---|
  | `# testo` | commento |
  | `attendi: REGEX` | aspetta (max `timeout`) una riga della UART che contiene il testo/regex, dopo l'ultima trovata |
  | `attendi 30: REGEX` | come sopra con timeout di 30 secondi |
  | `mai: REGEX` | alla fine, controlla che nessuna riga ricevuta corrisponda |
  | `scrivi: testo` | scrive una riga sulla UART (come se la digitassi) |
  | `monitor: comando` | esegue un comando nel monitor di Renode (es. `sysbus.gpio0.sw0 Press`) |
  | `controlla: comando => testo` | esegue il comando nel monitor e controlla che la risposta contenga il testo (es. `sysbus.gpio0.led0 State => True`) |
  | `pausa: 500` | aspetta 500 ms (tempo reale) |

  Le regex sono quelle di .NET e non distinguono maiuscole e minuscole. Per i caratteri
  speciali (`( ) [ ] . * + ?`) usare `\`.

- Nomi nella scheda simulata: UART della console `sysbus.uart0`, LED `sysbus.gpio0.led0..led3`
  (pin 13-16, `State` vale `True` se acceso), pulsanti `sysbus.gpio0.sw0..sw3`
  (pin 11, 12, 24, 25; comandi `Press` e `Release`), sensore `sysbus.twi0.bme280`
  (indirizzo I2C `0x76`, nel devicetree Zephyr il bus è `i2c0`; proprietà `Temperature`,
  `Humidity`, `Pressure`, per esempio `sysbus.twi0.bme280 Temperature 35.5`; all'avvio
  22.5 °C, impostati da `renode/esercizio.resc`). Sul bus I2C usare solo `i2c_write` +
  `i2c_read` (niente repeated start, vedi i limiti nella sezione 3).

## 6b. Teoria

Un file per modulo nella cartella `teoria/`, da leggere prima degli esercizi del modulo.

- [ ] `teoria/01_setup_toolchain.md` — toolchain, cross-compilazione, west, Renode (Modulo 0).
- [ ] `teoria/02_c_embedded.md` — tipi, bit, puntatori, struct, memoria, volatile (Modulo 1).
- [ ] `teoria/03_build_kconfig_devicetree.md` — CMake, Kconfig, devicetree, logging (Modulo 2).
- [ ] `teoria/04_thread_scheduling.md` — thread, priorità, scheduler, stack (Modulo 3).
- [ ] `teoria/05_sincronizzazione.md` — race condition, mutex, semafori, code (Modulo 4).
- [ ] `teoria/06_tempo_lavoro_differito.md` — timer, ISR, work queue (Modulo 5).
- [ ] `teoria/07_periferiche_driver.md` — modello dei driver, GPIO, UART, I2C, sensori (Modulo 6).
- [ ] `teoria/08_strumenti_debug_test.md` — shell, ztest, gdb, analisi memoria (Modulo 7).
- [ ] `teoria/09_progettazione.md` — dalla specifica all'architettura di un firmware (Modulo 8).

## 7. Stato di avanzamento

Legenda: [x] fatto, [ ] da fare. Il dettaglio di ogni esercizio è nella sezione 8.

- [ ] Modulo 0 — da iniziare
- [ ] Modulo 1
- [ ] Modulo 2
- [ ] Modulo 3
- [ ] Modulo 4
- [ ] Modulo 5
- [ ] Modulo 6
- [ ] Modulo 7
- [ ] Modulo 8

**Ultima sessione: —.** **Prossimo passo:** leggere `teoria/01_setup_toolchain.md` e svolgere
`modulo0/01_installazione.md`.

## 8. Catalogo degli esercizi

Per ogni esercizio: cartella, obiettivo, concetti nuovi ed eventuali note emerse durante lo
svolgimento. **Tutti gli esercizi sono già creati**: si apre `src/main.c` in VS Code, si legge
la consegna e si scrive il codice sotto.

### Modulo 0 — Setup e primo firmware

- [ ] `modulo0/01_installazione.md` — **Installare gli strumenti.** Git, Python, CMake, Ninja,
  west, workspace Zephyr, SDK, Renode, VS Code. Ogni passo ha un comando di controllo.
- [ ] `modulo0/02_hello` — **Ciao, Zephyr!** Primo firmware: stampare un saluto con `printk`.
  Concetti: `main`, `printk`, compilare con west, aprire in Renode, banner di avvio.
- [ ] `modulo0/03_esplora_renode.md` — **Il monitor di Renode.** Comandi `peripherals`,
  `pause`/`start`, leggere lo stato dei LED, premere un pulsante, `sysbus ReadDoubleWord`.
- [ ] `modulo0/04_formati` — **printk e i formati.** `%d %u %x %s %c %p`, versione del kernel,
  tempo dall'avvio. Concetti: specificatori di formato, `k_uptime_get`.
- [ ] `modulo0/05_ciclo_infinito` — **Un firmware non finisce mai.** Contatore stampato ogni
  secondo con `k_msleep`. Concetti: super-loop, sleep, perché `main` non deve "finire".

### Modulo 1 — C per l'embedded

- [ ] `modulo1/01_tipi` — **Tipi e dimensioni.** `sizeof`, `int8_t`...`uint64_t`, overflow di un
  `uint8_t`, signed contro unsigned, conversioni implicite.
- [ ] `modulo1/02_bit` — **Operatori sui bit.** `& | ^ ~ << >>`, accendere/spegnere/invertire/
  leggere un bit, maschere, macro `BIT()` e `GENMASK()` di Zephyr.
- [ ] `modulo1/03_array_stringhe` — **Array e stringhe.** Array, terminatore `'\0'`, `strlen`,
  `snprintk`, perché un buffer troppo piccolo è pericoloso.
- [ ] `modulo1/04_puntatori` — **Puntatori.** Indirizzo `&`, dereferenza `*`, `swap`, funzioni
  che restituiscono più valori, aritmetica dei puntatori sugli array, `NULL`.
- [ ] `modulo1/05_struct` — **Struct.** `struct misura`, array di struct, `->`, `sizeof` e padding,
  passare una struct per puntatore.
- [ ] `modulo1/06_enum_stati` — **Enum e macchina a stati.** Semaforo stradale con `enum` e
  `switch`, transizioni temporizzate.
- [ ] `modulo1/07_static_const_macro` — **static, const e macro.** Variabili `static` locali e di
  file, `const`, `#define` e le sue trappole, `ARRAY_SIZE`.
- [ ] `modulo1/08_file_multipli` — **Più file.** `statistiche.c/.h`, header guard, `extern`,
  aggiungere sorgenti in `CMakeLists.txt`.
- [ ] `modulo1/09_registri` — **volatile e registri.** Leggere l'ID del chip dal registro FICR e
  accendere un LED scrivendo direttamente nei registri GPIO. Concetti: memory-mapped I/O.
- [ ] `modulo1/10_memoria` — **Dove vivono codice e variabili.** Stampare gli indirizzi di
  funzioni, costanti, globali, `static`, locali: flash contro RAM, sezioni `.text`, `.rodata`,
  `.data`, `.bss`, stack; confronto con `build/zephyr/zephyr.map`.
- [ ] `modulo1/11_buffer_circolare` — **Riepilogo: buffer circolare.** Struct + funzioni +
  puntatori: `rb_push`, `rb_pop`, pieno/vuoto, con test fatti a mano.

### Modulo 2 — Come si costruisce un'applicazione Zephyr

- [ ] `modulo2/01_prj_conf` — **prj.conf e Kconfig.** Togliere il banner di avvio, cambiare
  opzioni, leggere `build/zephyr/.config`, dimensione del firmware.
- [ ] `modulo2/02_kconfig_proprio` — **Opzioni Kconfig proprie.** File `Kconfig` con un intero, un
  booleano e una stringa usati nel codice.
- [ ] `modulo2/03_logging` — **Logging.** `LOG_MODULE_REGISTER`, `LOG_INF/WRN/ERR/DBG`, livelli,
  `LOG_HEXDUMP_INF`, differenza tra `printk` e log.
- [ ] `modulo2/04_devicetree_lettura` — **Leggere il devicetree.** Trovare nodi in
  `build/zephyr/zephyr.dts`, macro `DT_ALIAS`, `DT_CHOSEN`, `DT_PROP`, `DT_REG_ADDR`.
- [ ] `modulo2/05_blinky` — **Il primo LED.** `GPIO_DT_SPEC_GET`, `gpio_is_ready_dt`,
  `gpio_pin_configure_dt`, `gpio_pin_toggle_dt`.
- [ ] `modulo2/06_overlay` — **Overlay.** `app.overlay` con un alias proprio e il nodo
  `zephyr,user` con proprietà lette dal codice.
- [ ] `modulo2/07_menuconfig_assert` — **menuconfig e assert.** Esplorare le opzioni con
  `west build -t menuconfig`, attivare `CONFIG_ASSERT` e far fallire un `__ASSERT`.
- [ ] `modulo2/08_riepilogo` — **Riepilogo.** Lampeggio con periodo da Kconfig, LED scelto da
  overlay, messaggi con il logging.

### Modulo 3 — Thread e scheduling

- [ ] `modulo3/01_due_thread` — **Due thread.** `K_THREAD_DEFINE`, due thread che stampano a
  ritmi diversi.
- [ ] `modulo3/02_priorita` — **Priorità.** Priorità cooperative e preemptive, un thread che non
  dorme mai e "affama" gli altri, `k_yield`.
- [ ] `modulo3/03_sleep_e_attesa_attiva` — **Dormire o aspettare?** `k_msleep` contro
  `k_busy_wait`, misura con `k_uptime_get` e `k_cycle_get_32`.
- [ ] `modulo3/04_thread_dinamici` — **Thread creati a runtime.** `K_THREAD_STACK_DEFINE`,
  `k_thread_create` con parametri, `k_thread_join`.
- [ ] `modulo3/05_stati_thread` — **Stati di un thread.** Nomi dei thread, `k_thread_suspend`/
  `resume`, elenco con `k_thread_foreach`.
- [ ] `modulo3/06_time_slicing` — **Time slicing.** Thread di uguale priorità con e senza
  `CONFIG_TIMESLICING`.
- [ ] `modulo3/07_stack` — **Lo stack.** Uso dello stack con il thread analyzer, provocare uno
  stack overflow e riconoscerlo.
- [ ] `modulo3/08_riepilogo` — **Riepilogo: quattro LED.** Un thread per LED a periodi diversi e
  un thread di controllo che stampa lo stato.

### Modulo 4 — Sincronizzazione e comunicazione

- [ ] `modulo4/01_race_condition` — **Race condition.** Due thread incrementano lo stesso
  contatore: il totale è sbagliato. Capire perché.
- [ ] `modulo4/02_mutex` — **Mutex.** Correggere l'esercizio precedente con `k_mutex`.
- [ ] `modulo4/03_semaforo_segnale` — **Semaforo come segnale.** Un thread ne sveglia un altro;
  timeout di `k_sem_take`.
- [ ] `modulo4/04_semaforo_contatore` — **Semaforo contatore.** Un pool di 3 risorse condiviso
  da 5 thread.
- [ ] `modulo4/05_msgq` — **Coda di messaggi.** Produttore e consumatore di `struct misura` con
  `k_msgq`; coda piena e coda vuota.
- [ ] `modulo4/06_fifo` — **FIFO.** `k_fifo` con elementi da un pool statico; confronto con
  `k_msgq`.
- [ ] `modulo4/07_eventi` — **Eventi.** `k_event`: aspettare più condizioni insieme.
- [ ] `modulo4/08_atomic_inversione` — **Atomic e inversione di priorità.** `atomic_t`;
  osservare l'inversione di priorità e l'ereditarietà di priorità del mutex.
- [ ] `modulo4/09_riepilogo` — **Riepilogo: pipeline.** Generatore → filtro (media mobile) →
  stampa, con due code e statistiche protette da mutex.

### Modulo 5 — Tempo e lavoro differito

- [ ] `modulo5/01_k_timer` — **Timer del kernel.** `k_timer` periodico e one-shot, callback,
  `k_timer_status_get`.
- [ ] `modulo5/02_contesto_isr` — **Contesto di interrupt.** Cosa non si può fare in una
  callback del timer; `k_is_in_isr`; svegliare un thread con un semaforo.
- [ ] `modulo5/03_workqueue` — **Work queue di sistema.** `k_work_submit` dal timer: il lavoro
  pesante esce dall'interrupt.
- [ ] `modulo5/04_lavoro_ritardato` — **Lavoro ritardato e timeout.** `k_work_delayable`,
  `k_work_reschedule`: "allarme se nessun dato da 3 secondi".
- [ ] `modulo5/05_workqueue_propria` — **Una work queue propria.** `k_work_queue_start` con
  priorità scelta; perché non bloccare la coda di sistema.
- [ ] `modulo5/06_jitter` — **Precisione e jitter.** Ciclo con `k_msleep` contro `k_timer`:
  misurare la deriva nel tempo.
- [ ] `modulo5/07_riepilogo` — **Riepilogo: battito e supervisore.** LED di "battito" con timer
  e un supervisore che segnala i thread bloccati.

### Modulo 6 — Periferiche e driver

- [ ] `modulo6/01_pulsante_polling` — **Pulsante in polling.** Leggere `sw0` e accendere `led0`.
- [ ] `modulo6/02_pulsante_interrupt` — **Pulsante a interrupt.** `gpio_pin_interrupt_configure_dt`,
  callback, contatore di pressioni.
- [ ] `modulo6/03_debounce` — **Debounce.** Interrupt + lavoro ritardato per ignorare i rimbalzi
  (simulati con pressioni rapide da Renode).
- [ ] `modulo6/04_uart_polling` — **UART in polling.** `uart_poll_in`/`uart_poll_out`: eco dei
  caratteri digitati.
- [ ] `modulo6/05_uart_interrupt` — **UART a interrupt.** Ricezione a interrupt, buffer di riga,
  comandi `on`/`off` per il LED.
- [ ] `modulo6/06_i2c_registri` — **I2C a basso livello.** Leggere il registro ID del BME280
  e i coefficienti di calibrazione con `i2c_write` + `i2c_read`; repeated start e perché in
  Renode non si usa.
- [ ] `modulo6/07_driver_bme280` — **Un driver fatto da me: BME280.** `bme280_corso.c/.h`:
  init, calibrazione, modo normal, formula di compensazione intera, temperatura in
  centesimi. La sensor API di Zephyr come concetto. Riusato in 09 e nel progetto.
- [ ] `modulo6/08_watchdog` — **Watchdog.** `wdt_install_timeout`, `wdt_feed` da un thread; cosa
  succede quando il thread si blocca.
- [ ] `modulo6/09_riepilogo` — **Riepilogo: termostato.** Lettura periodica del sensore con il
  driver dell'esercizio 7, soglia, LED di allarme, pulsanti per cambiare modalità e soglia.

### Modulo 7 — Strumenti da professionista

- [ ] `modulo7/01_shell` — **La shell.** Attivare la shell, comandi `kernel`, `device`, `log`.
- [ ] `modulo7/02_comandi_propri` — **Comandi della shell propri.** `SHELL_CMD_REGISTER`,
  sottocomandi e argomenti: `led on|off`, `conta`.
- [ ] `modulo7/03_ztest` — **Test con ztest.** Test automatici del buffer circolare del Modulo 1
  (`modulo1/11_buffer_circolare`).
- [ ] `modulo7/04_errori_fatali` — **Errori fatali.** `k_oops`, accesso a indirizzo non valido,
  handler `k_sys_fatal_error_handler`, leggere un fault dump.
- [ ] `modulo7/05_debug_gdb` — **Debug con gdb.** Server gdb di Renode, breakpoint, passo passo,
  osservare variabili: trovare un bug nascosto.
- [ ] `modulo7/06_memoria` — **Memoria.** `rom_report`/`ram_report`, thread analyzer, ridurre lo
  stack in base alle misure.
- [ ] `modulo7/07_settings` — **Salvare la configurazione.** Sottosistema `settings` su flash
  simulata: salvare e ricaricare un valore.
- [ ] `modulo7/08_riepilogo` — **Riepilogo.** Lampeggio configurabile da shell, salvato con
  `settings`, con log.

### Modulo 8 — Progetto finale: nodo di monitoraggio

L'applicazione è una sola, in `modulo8/progetto/`, e cresce passo dopo passo. Le consegne dei
passi sono nei file `.md`; le verifiche in `modulo8/verifiche/`.

- [ ] `modulo8/01_specifica.md` — **Specifica e architettura.** Requisiti, thread, code,
  priorità e diagramma scritti da me prima di programmare.
- [ ] `modulo8/02_acquisizione.md` — **Acquisizione.** Thread del sensore e coda delle misure.
- [ ] `modulo8/03_elaborazione.md` — **Elaborazione.** Media mobile, soglie con isteresi, LED di
  allarme.
- [ ] `modulo8/04_pulsante.md` — **Tacitare l'allarme e battito.** Pulsante con debounce che
  conferma l'allarme; LED1 di "battito".
- [ ] `modulo8/05_shell_config.md` — **Shell e configurazione.** Comandi `stato`, `soglia`,
  `storico`; soglie salvate con `settings`; `formato.c` per leggere numeri senza float.
- [ ] `modulo8/06_robustezza.md` — **Robustezza.** Watchdog, gestione degli errori del sensore,
  dimensionamento di stack e code.
- [ ] `modulo8/07_test_relazione.md` — **Test e relazione.** Test della logica con ztest
  (`modulo8/test_logica/`, che compila `filtro.c`, `allarme.c`, `formato.c` del progetto),
  verifica completa in Renode (`modulo8/verifiche/07_completa.txt`), relazione finale.
