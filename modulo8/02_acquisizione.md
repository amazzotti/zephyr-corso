# Passo 2 (Modulo 8) — Acquisizione

Teoria: [teoria/09_progettazione.md](../teoria/09_progettazione.md), sezioni 2 e 4;
ripasso: `modulo6/07_sensore_api`, `modulo4/05_msgq`, `modulo3/01_due_thread`.

**Obiettivo:** il thread di acquisizione legge il sensore ogni secondo e mette ogni misura in
una coda; un thread di elaborazione (per ora semplicissimo) la riceve e la stampa.
Requisiti coperti: **R1, R2**.

## Cosa fare

1. **Il sensore.** È già collegato: il `CMakeLists.txt` del progetto include
   `comune/sensore_sim.cmake`, che aggiunge l'ADT7420 (nodo `sensore`), il driver
   ufficiale (`CONFIG_SENSOR`, `CONFIG_I2C`) e la shell con il comando `sim temp`. Nel
   codice usi la sensor API come nell'esercizio 6.07:
   `DEVICE_DT_GET(DT_NODELABEL(sensore))`, `sensor_sample_fetch`, `sensor_channel_get`.
   Sulla scheda vera cambierebbe solo il devicetree (nodo sotto `&i2c1`): questo codice no.
2. **`src/misura.h`**: la `struct misura` della tua specifica. Almeno:
   - `uint32_t t_ms` (tempo da `k_uptime_get_32()`),
   - `int32_t temp_c100` (temperatura in centesimi di °C),
   - `int err` (0 = lettura riuscita, altrimenti il codice di errore negativo).
3. **`src/sensore.c` / `sensore.h`**:
   - all'avvio del thread controlla `device_is_ready`: se il sensore non è pronto, ogni
     misura avrà `err = -ENODEV`, ma non bloccare il sistema;
   - un thread (`K_THREAD_DEFINE`, priorità e stack dalla specifica) che ogni 1000 ms
     fa `sensor_sample_fetch` + `sensor_channel_get(..., SENSOR_CHAN_AMBIENT_TEMP, ...)`,
     converte `struct sensor_value` in centesimi (`val1 * 100 + val2 / 10000`) e mette
     l'eventuale errore in `m.err`;
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
- La temperatura simulata si cambia dalla **console** (la finestra della UART), non dal
  monitor di Renode: `sim temp 2550` → 25,50 °C (all'avvio vale 22,50). `sim temp` da solo
  mostra il valore. La shell è già attiva per questo comando: le righe della console
  possono iniziare con il prompt `uart:~$ `, è normale.
- Il sensore è usato da un solo thread (l'acquisizione): non serve un mutex. Se un giorno
  lo leggessero due thread, cosa potrebbe succedere se il `fetch` dell'uno si
  intrecciasse con il `get` dell'altro? (Il campione misurato è uno solo, dentro il driver.)
- Se il sensore non è pronto all'avvio, stampa un errore ma **non** bloccare il sistema:
  la gestione completa dei guasti arriva al passo 6.

## Completamento

- `.\verifica.ps1 modulo8\progetto -File modulo8\verifiche\02_acquisizione.txt` → TUTTO OK.
- Cambiando la temperatura con `sim temp`, le misure seguono.

## Domande (rispondi qui)

1. Quanti byte occupa la coda (`N × sizeof(struct misura)`)?
2. Perché l'acquisizione usa `K_NO_WAIT` nella `put` e l'elaborazione `K_FOREVER` nella `get`?
3. Che priorità hai dato ai due thread e perché?
