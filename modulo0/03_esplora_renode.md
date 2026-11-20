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
(prompt `(nrf52840dk)`) e la **UART** (`sysbus.uart0`). I comandi vanno scritti nel monitor.

---

## Passi

1. **Avvio.** Scrivi `start`. Nella finestra della UART compare l'output del firmware.
   Poi `pause`, e di nuovo `start`: cosa cambia? (Il firmware ha già finito di stampare...)
2. **Ripartire da capo.** `machine Reset` e poi `start`. L'output ricompare: il firmware è
   ripartito dall'inizio, come premendo il tasto di reset della scheda vera.
3. **Le periferiche.** `peripherals` elenca tutto ciò che c'è nella scheda simulata, con
   l'indirizzo di ciascuna periferica. Trova `uart0`, `gpio0`, `twi0`, `wdt`.
4. **Un LED.** `sysbus.gpio0.led0 State` → `False` (spento: nessuno l'ha acceso).
5. **Un pulsante.** `sysbus.gpio0.sw0 Press`, poi `sysbus.gpio0.sw0 Pressed` (dice se è
   premuto; se il comando non esiste nella tua versione, scrivi `sysbus.gpio0.sw0` da solo
   per vedere quelli disponibili), poi `sysbus.gpio0.sw0 Release`. Il firmware non se ne accorge: non legge il
   pulsante (lo farà nel Modulo 6).
6. **La memoria.** La RAM inizia a `0x20000000`, la flash a `0x00000000`.
   - `sysbus ReadDoubleWord 0x0` → i primi 4 byte della flash: il valore iniziale dello
     stack (teoria, sezione 7). Deve essere un indirizzo di RAM, del tipo `0x2000....`.
   - `sysbus ReadDoubleWord 0x4` → l'indirizzo del *reset handler*, la prima istruzione
     eseguita all'accensione.
7. **La CPU.** `pause`, poi `cpu PC`: l'indirizzo dell'istruzione in esecuzione.
   Cerca quell'indirizzo (senza `0x` e con zeri davanti, es. `00001a2c`) nel file
   `modulo0\02_hello\build\zephyr\zephyr.map` o con questo comando:
   ```powershell
   arm-zephyr-eabi-addr2line -f -e modulo0\02_hello\build\zephyr\zephyr.elf 0x1a2c
   ```
   (se `arm-zephyr-eabi-addr2line` non si trova, lo trovi nella cartella dell'SDK, in
   `arm-zephyr-eabi\bin`). In che funzione è la CPU? Probabilmente nel ciclo "idle" del kernel:
   quando nessun thread ha da fare, la CPU dorme lì.
8. **Aiuto.** `help` e poi `sysbus.gpio0.led0` da solo: elenca i comandi disponibili per il LED.
9. **Uscita.** `quit`.

---

## Le mie risposte (scrivile qui)

1. Dopo `pause` e `start` l'output si ripete? Perché?
2. Indirizzi di `uart0`, `gpio0`, `twi0` secondo `peripherals`:
3. Valore letto a `0x0` (stack iniziale) e a `0x4` (reset handler):
4. Il valore a `0x4` è dispari: le istruzioni ARM Thumb hanno l'ultimo bit dell'indirizzo a 1.
   Togliendo 1, in che funzione cade? (cercalo nel `zephyr.map`)
5. Valore di `cpu PC` dopo `pause` e funzione corrispondente:
6. Cosa elenca `help` per il LED?
