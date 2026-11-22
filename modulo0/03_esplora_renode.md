# Esercizio 3 (Modulo 0) — Il monitor di Renode

Teoria: [teoria/01_setup_toolchain.md](../teoria/01_setup_toolchain.md), sezioni 1 e 9.

**Obiettivo:** prendere confidenza con il simulatore: guardare dentro la scheda, fermarla,
farla ripartire, leggere la memoria, guardare i LED e premere i pulsanti. Sono le stesse
cose che con una scheda vera faresti con un debugger e con le dita.

Si usa il firmware dell'esercizio precedente (`02_hello` già svolto).

```powershell
.\esegui.ps1 modulo0\02_hello -Pausa
```

Con `-Pausa` la scheda è caricata ma ferma. Si aprono due finestre: il **monitor**
(prompt `(stm32f4)`) e la **UART** della console (`sysbus.usart2`). I comandi vanno scritti
nel monitor.

---

## Passi

1. **Avvio.** Scrivi `start`. Nella finestra della UART compare l'output del firmware.
   Poi `pause`, e di nuovo `start`: cosa cambia? (Il firmware ha già finito di stampare...)
2. **Ripartire da capo.** `machine Reset` e poi `start`. L'output ricompare: il firmware è
   ripartito dall'inizio, come premendo il tasto di reset (nero) della scheda vera.
3. **Le periferiche.** `peripherals` elenca tutto ciò che c'è nella scheda simulata, con
   l'indirizzo di ciascuna periferica. Trova `usart2` (la console), `gpioPortA` e
   `gpioPortD` (con i loro pulsanti e LED "appesi" sotto), `rcc` (i clock), `iwdg` (il
   watchdog), `flash` e `sram`.
4. **Un LED.** `sysbus.gpioPortD.led0 State` → `False`: il LED verde (PD12) è spento,
   perché `02_hello` non lo usa. I LED della Discovery sono *attivi alti*: si accendono
   quando il pin è a 1.
5. **Un pulsante.** `sysbus.gpioPortA.sw0 Press` (il pulsante blu, su PA0), poi
   `sysbus.gpioPortA.sw0 Pressed` → `True` (dice se è premuto). Leggi il registro dei pin
   d'ingresso della porta A: `sysbus ReadDoubleWord 0x40020010` → `0x00000001` (bit 0 = PA0
   a 1: il pulsante è *attivo alto*). Poi `sysbus.gpioPortA.sw0 Release` e rileggi il
   registro. Il firmware non se ne accorge: non legge il pulsante (lo farà nel Modulo 6).
6. **La memoria.** Sullo STM32F407 la flash inizia a `0x08000000`, la RAM a `0x20000000`.
   - `sysbus ReadDoubleWord 0x08000000` → i primi 4 byte della flash: il valore iniziale
     dello stack (teoria, sezione 7). Deve essere un indirizzo di RAM, del tipo
     `0x2000....` (per `02_hello` circa `0x20001100`).
   - `sysbus ReadDoubleWord 0x08000004` → l'indirizzo del *reset handler*, la prima
     istruzione eseguita all'accensione (del tipo `0x08000...`).
   - `sysbus ReadDoubleWord 0x0` → in Renode vale `0x00000000`. Sul chip vero, all'avvio,
     l'indirizzo 0 è un "alias" della flash (la CPU Cortex-M legge la tabella dei vettori
     da lì); il simulatore invece fa partire la CPU direttamente da `0x08000000` e non
     mappa l'alias.
7. **La CPU.** `pause`, poi `cpu PC`: l'indirizzo dell'istruzione in esecuzione, del tipo
   `0x8002c8c`. Cercalo nel file `modulo0\02_hello\build\zephyr\zephyr.map` o con questo
   comando:
   ```powershell
   & "$env:USERPROFILE\zephyr-sdk-1.0.1\gnu\arm-zephyr-eabi\bin\arm-zephyr-eabi-addr2line.exe" -f -e modulo0\02_hello\build\zephyr\zephyr.elf 0x8002c8c
   ```
   (usa il tuo valore di `cpu PC`). In che funzione è la CPU? Probabilmente nel ciclo
   "idle" del kernel: quando nessun thread ha da fare, la CPU dorme lì.
8. **Un registro di sistema.** `sysbus ReadDoubleWord 0xE0042000` legge `DBGMCU_IDCODE`,
   l'identificativo del chip: in Renode `0x10006411`. Lo rileggerai dal C nel Modulo 1.
9. **Aiuto.** `help` e poi `sysbus.gpioPortD.led0` da solo: elenca i comandi disponibili
   per il LED.
10. **Uscita.** `quit`.

---

## Le mie risposte (scrivile qui)

1. Dopo `pause` e `start` l'output si ripete? Perché?
2. Indirizzi di `usart2`, `gpioPortA`, `gpioPortD`, `rcc` secondo `peripherals`:
3. Valore letto a `0x08000000` (stack iniziale) e a `0x08000004` (reset handler):
4. Il valore a `0x08000004` è dispari: le istruzioni ARM Thumb hanno l'ultimo bit
   dell'indirizzo a 1. Togliendo 1, in che funzione cade? (cercalo nel `zephyr.map`)
5. Valore di `cpu PC` dopo `pause` e funzione corrispondente:
6. Valore di `0x40020010` con il pulsante premuto e rilasciato:
7. Cosa elenca `help` per il LED?
