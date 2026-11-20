# 01 — Microcontrollori, Zephyr, toolchain e Renode (Modulo 0)

## 1. Cos'è un microcontrollore

Un **microcontrollore** (MCU, *microcontroller unit*) è un piccolo computer completo in un
solo chip:

| Parte | Nel nRF52840 | A cosa serve |
|---|---|---|
| CPU | ARM Cortex-M4F a 64 MHz | esegue le istruzioni |
| Flash | 1 MB, da indirizzo `0x00000000` | contiene il programma (il **firmware**) e le costanti; non si cancella spegnendo |
| RAM | 256 KB, da indirizzo `0x20000000` | variabili, stack; si perde spegnendo |
| Periferiche | GPIO, UART, I2C, SPI, timer, RTC, watchdog, radio... | parlano con il mondo esterno |

Differenze rispetto al PC su cui hai fatto il corso di Python:
- niente sistema operativo "grande", niente file, niente schermo: l'unico modo per "vedere"
  qualcosa è una **UART** (porta seriale) o un LED;
- memoria piccolissima: 256 KB di RAM contro i GB del PC;
- il programma parte all'accensione e **non finisce mai**;
- le periferiche si comandano scrivendo in **registri**, cioè indirizzi di memoria speciali
  (lo vedrai nel Modulo 1).

## 2. Cos'è un RTOS e cos'è Zephyr

Un firmware semplice è un unico ciclo infinito (*super-loop*): leggi il sensore, aggiorna il
LED, controlla il pulsante, ricomincia. Quando le cose da fare diventano tante, con tempi
diversi, il ciclo diventa difficile da gestire.

Un **RTOS** (*Real-Time Operating System*) è un piccolo sistema operativo che permette di
dividere il programma in **thread** (attività che sembrano girare in parallelo), con
**priorità**, strumenti per farli comunicare e **tempi garantiti** ("real-time" non vuol dire
veloce, vuol dire *prevedibile*).

**Zephyr** è un RTOS open source della Linux Foundation, usato nell'industria. Oltre al
kernel offre: driver per centinaia di schede e periferiche, logging, shell, file system,
stack di rete e Bluetooth, framework di test. Lo stesso codice gira su schede diverse
cambiando solo la "board" in compilazione.

## 3. Cross-compilazione e toolchain

Il tuo PC ha una CPU x86-64; la scheda ha una CPU ARM. Il compilatore deve quindi produrre
istruzioni per una CPU **diversa** da quella su cui gira: è la **cross-compilazione**.

La **toolchain** è l'insieme degli strumenti che lo fanno. Nel Zephyr SDK si chiama
`arm-zephyr-eabi`: compilatore `arm-zephyr-eabi-gcc`, linker, `gdb` per il debug, `objdump`,
`size`...

Dal codice al firmware:

```
main.c --compilatore--> main.o --\
kernel, driver ...  --> *.o -----+--linker--> zephyr.elf --> (su scheda vera: zephyr.hex)
                                  |
                       mappa della memoria (dove va ogni cosa)
```

- **file oggetto** (`.o`): codice macchina di un solo file, non ancora eseguibile;
- **linker**: unisce tutti i `.o` e decide a quale indirizzo di flash e RAM va ogni funzione e
  variabile;
- **ELF** (`zephyr.elf`): il firmware completo, con in più le informazioni per il debug
  (nomi delle funzioni e delle variabili). Renode carica direttamente questo file.

## 4. Gli strumenti

| Strumento | Ruolo |
|---|---|
| **Git** | scaricare i sorgenti di Zephyr e dei moduli; versionare il tuo lavoro |
| **Python** | linguaggio di molti script di Zephyr, e di `west` |
| **CMake** | descrive *come* si compila un progetto (file `CMakeLists.txt`) |
| **Ninja** | esegue la compilazione vera e propria, ricompilando solo ciò che è cambiato |
| **dtc** | compilatore del devicetree (Modulo 2) |
| **west** | lo "strumento tuttofare" di Zephyr: scarica i sorgenti (`west update`), compila (`west build`), lancia menuconfig, programma le schede |
| **Zephyr SDK** | la toolchain `arm-zephyr-eabi` |
| **Renode** | simulatore della scheda |

## 5. Il workspace

Il workspace (`C:\zephyrproject`) contiene:

```
C:\zephyrproject\
  .venv\          ambiente virtuale Python con west (come nel corso ML)
  .west\          configurazione di west
  zephyr\         il kernel, i driver, le board, gli esempi (ZEPHYR_BASE)
  modules\        librerie esterne (es. hal_nordic: i file del costruttore del chip)
  bootloader\, tools\ ...
```

Le tue applicazioni stanno **fuori** (in questo repo). `attiva.ps1` attiva l'ambiente e
imposta `ZEPHYR_BASE`, così CMake sa dove trovare Zephyr.

## 6. Com'è fatta un'applicazione Zephyr

```
02_hello/
  CMakeLists.txt     dice a CMake: "questa è un'app Zephyr, i sorgenti sono questi"
  prj.conf           opzioni di configurazione (Kconfig): quali funzioni del kernel attivare
  src/main.c         il tuo codice
```

`CMakeLists.txt` minimo:

```cmake
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})   # carica tutto Zephyr
project(hello)
target_sources(app PRIVATE src/main.c)                   # i miei sorgenti
```

Compilazione (lo fa `esegui.ps1` per te):

```powershell
west build -b nrf52840dk/nrf52840 -d modulo0\02_hello\build modulo0\02_hello --no-sysbuild
```

- `-b` sceglie la **board**: `nrf52840dk` è la scheda, `nrf52840` il chip (SoC);
- `-d` la cartella dove finiscono i file generati;
- il risultato è `build/zephyr/zephyr.elf`.

La prima compilazione è lenta (compila anche il kernel); le successive ricompilano solo ciò
che hai cambiato.

## 7. Cosa succede all'accensione

1. La CPU legge dalla flash, all'indirizzo 0, la **tabella dei vettori**: il valore iniziale
   dello stack e l'indirizzo della prima istruzione (*reset handler*).
2. Il codice di avvio copia le variabili inizializzate da flash a RAM, azzera le altre
   (vedrai il perché nel Modulo 1).
3. Zephyr inizializza il kernel e i driver, nell'ordine stabilito dal devicetree.
4. Stampa il **banner** `*** Booting Zephyr OS build ... ***`.
5. Crea il thread principale, che chiama la tua `main()`.

Se `main()` ritorna, il thread principale finisce ma **il sistema resta acceso**: gli altri
thread (se ci sono) continuano. Per questo un firmware "finisce" solo se lo spegni.

## 8. UART, console e printk

La **UART** è la periferica più semplice per comunicare: due fili (TX e RX) su cui i byte
viaggiano uno alla volta a una velocità prefissata (115200 bit/s). Sulla scheda vera la UART
arriva al PC tramite USB; in Renode si vede in una finestra (*analyzer*).

Zephyr usa una UART come **console**: `printk("x = %d\n", x);` stampa lì. `printk` è come il
`print` di Python con le f-string scritte alla vecchia maniera del C (formato + valori).

## 9. Renode

Renode simula la scheda **a livello di istruzioni**: esegue lo stesso `zephyr.elf` che
andrebbe sulla scheda vera, e simula CPU, memoria e periferiche (UART, GPIO, timer, I2C...).

Ha un **monitor**: una console dove dai comandi al simulatore (avviare, fermare, leggere la
memoria, premere un pulsante, guardare un LED). La scheda del corso è descritta in
`renode/nrf52840dk.repl`; lo script `renode/esercizio.resc` la crea e carica il firmware.

Comandi base del monitor:

| Comando | Effetto |
|---|---|
| `start` / `pause` | avvia / ferma l'emulazione |
| `machine Reset` | riparte da capo (ricarica il firmware) |
| `peripherals` | elenca le periferiche della scheda |
| `sysbus.gpio0.led0 State` | stato del LED 0 |
| `sysbus.gpio0.sw0 Press` / `Release` | preme / rilascia il pulsante 0 |
| `sysbus ReadDoubleWord 0x20000000` | legge 4 byte di memoria |
| `cpu PC` | indirizzo dell'istruzione corrente |
| `help` | aiuto; `help <periferica>` o il solo nome mostra i comandi |
| `quit` | chiude Renode |

**Tempo virtuale.** Renode ha un suo orologio: un secondo "della scheda" non è per forza un
secondo dell'orologio di casa. Quando la CPU non ha niente da fare, il tempo virtuale può
scorrere più veloce del reale. L'ordine degli eventi e i tempi *relativi* sono corretti;
le misure di prestazioni assolute no.

## 10. Leggere gli errori

- **Errore di compilazione**: leggi sempre il **primo** errore, gli altri spesso sono
  conseguenze. Il formato è `file:riga:colonna: error: messaggio`.
- **Warning**: il codice compila ma probabilmente c'è un problema. Trattali come errori.
- **Errore a runtime**: il firmware stampa un messaggio di errore fatale (*fault*) con i
  registri della CPU. Si impara a leggerli nei Moduli 3 e 7.
- **Nessun output**: il firmware è bloccato prima della stampa, o la stampa non ha `\n`
  finale (l'uscita a volte arriva solo a fine riga).
