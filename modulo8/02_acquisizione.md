# Passo 2 (Modulo 8) — Acquisizione

Teoria: [teoria/09_progettazione.md](../teoria/09_progettazione.md), sezioni 2 e 4;
ripasso: `modulo6/07_driver_bme280`, `modulo4/05_msgq`, `modulo3/01_due_thread`.

**Obiettivo:** il thread di acquisizione legge il BME280 ogni secondo e mette ogni misura in
una coda; un thread di elaborazione (per ora semplicissimo) la riceve e la stampa.
Requisiti coperti: **R1, R2**.

## Cosa fare

1. **Il driver del sensore.** Copia `bme280_corso.c` e `bme280_corso.h` da
   `modulo6/07_driver_bme280/src/` in `modulo8/progetto/src/` (il `CMakeLists.txt` del
   progetto compila da solo ogni `.c` in `src/`). Qui lo copi, invece di compilarlo dalla
   cartella del Modulo 6 come nell'esercizio 9: il progetto deve stare in piedi da solo, e
   d'ora in poi il driver è suo (potrai aggiungere, per esempio, l'umidità).
   In `prj.conf` aggiungi il driver del bus: `CONFIG_I2C=y`. Niente `CONFIG_SENSOR` e niente
   nodo `bosch,bme280` nel devicetree: il driver ufficiale di Zephyr in Renode non funziona
   (teoria 07, sezione 7). Ricompila con `-Pulito`.
2. **`src/misura.h`**: la `struct misura` della tua specifica. Almeno:
   - `uint32_t t_ms` (tempo da `k_uptime_get_32()`),
   - `int32_t temp_c100` (temperatura in centesimi di °C),
   - `int err` (0 = lettura riuscita, altrimenti il codice di errore negativo).
3. **`src/sensore.c` / `sensore.h`**:
   - all'avvio del thread `bme280c_init()`: se fallisce, ricorda l'errore (ogni misura
     avrà `err` diverso da 0) ma non bloccare il sistema;
   - un thread (`K_THREAD_DEFINE`, priorità e stack dalla specifica) che ogni 1000 ms
     chiama `bme280c_leggi_centesimi(&m.temp_c100)` e mette il valore restituito in `m.err`;
     il driver dà già i centesimi di grado, niente conversioni;
   - `k_msgq_put(&coda_misure, &m, K_NO_WAIT)`: se la coda è piena, conta la misura persa
     (non bloccare l'acquisizione!).
   - Per far partire il ciclo a intervalli regolari (non "1 s + tempo della lettura") usa
     un tempo di risveglio assoluto: `k_sleep(K_TIMEOUT_ABS_MS(prossimo))` con
     `prossimo += 1000`. Perché è meglio di `k_msleep(1000)`? (Modulo 5, jitter.)
4. **La coda**: `K_MSGQ_DEFINE(coda_misure, sizeof(struct misura), N, 4)` con N dalla
   specifica. Dove la definisci? In un file e la dichiari `extern` nell'header di chi la usa
   (Modulo 1, file multipli).
5. **`src/elabora.c`**: un thread che fa `k_msgq_get(&coda_misure, &m, K_FOREVER)` e per ora
   stampa soltanto. **Formato obbligatorio** (lo controlla la verifica):

   ```
   MISURA #<n> T=<gradi>.<centesimi> C
   ```

   con `n` che parte da 1, per esempio `MISURA #1 T=22.50 C`. Attenzione ai negativi:
   `-0.50` deve stampare `T=-0.50 C` (teoria sezione 6). Se `m.err != 0` stampa invece
   `MISURA #<n> ERRORE <err>`.

## Suggerimenti

- Scrivi una funzione `static void stampa_c100(int32_t v, char *buf, size_t len)` che
  formatta i centesimi con segno: ti servirà ovunque (meglio in un file `formato.c`).
- La temperatura simulata si legge e si cambia dal monitor di Renode, per esempio
  `sysbus.twi0.bme280 Temperature 25.5` (all'avvio vale 22.5).
- Il driver è usato da un solo thread (l'acquisizione): non serve un mutex. Se un giorno
  lo chiamassero due thread, quale problema potrebbe nascere sul bus?
- Se il sensore non è pronto all'avvio, stampa un errore ma **non** bloccare il sistema:
  la gestione completa dei guasti arriva al passo 6.

## Completamento

- `.\verifica.ps1 modulo8\progetto -File modulo8\verifiche\02_acquisizione.txt` → TUTTO OK.
- Cambiando la temperatura dal monitor di Renode, le misure seguono.

## Domande (rispondi qui)

1. Quanti byte occupa la coda (`N × sizeof(struct misura)`)?
2. Perché l'acquisizione usa `K_NO_WAIT` nella `put` e l'elaborazione `K_FOREVER` nella `get`?
3. Che priorità hai dato ai due thread e perché?
