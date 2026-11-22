# Passo 6 (Modulo 8) — Robustezza

Teoria: [teoria/09_progettazione.md](../teoria/09_progettazione.md), sezioni 7 e 8;
ripasso: `modulo6/08_watchdog`, `modulo7/06_memoria`.

**Obiettivo:** il nodo deve comportarsi in modo sensato quando qualcosa va storto: sensore
guasto, thread bloccato. E le risorse vanno dimensionate su misure reali.
Requisiti coperti: **R10, R11**, N2, N3.

## Cosa fare

1. **Iniezione di guasti** (comando di test): `guasto on|off`. Con `guasto on` il thread di
   acquisizione si comporta come se il sensore non rispondesse (misura con `err = -EIO`
   invece di leggere). È codice di test che resta nel firmware: perché è utile anche sul
   prodotto vero?
2. **Gestione del sensore guasto** (nell'elaborazione): conta le misure con errore
   consecutive.
   - alla 3a consecutiva: `LOG_ERR("ERRORE SENSORE")`, una sola volta;
   - alla prima misura buona dopo l'errore: `LOG_INF("SENSORE OK")`;
   - durante il guasto il filtro non riceve valori e lo stato dell'allarme resta
     com'è (oppure decidi tu: scrivilo nella specifica e motivalo);
   - `stato` mostra una riga in più: `Sensore: OK` oppure `Sensore: GUASTO`.
3. **Watchdog** (`CONFIG_WATCHDOG=y`, dispositivo `DEVICE_DT_GET(DT_ALIAS(watchdog0))`:
   l'IWDG, già attivato in `app.overlay`):
   timeout 5000 ms, opzione `WDT_FLAG_RESET_SOC`. Va nutrito (`wdt_feed`) **dall'elaborazione**
   a ogni messaggio ricevuto, perché è il lavoro che dimostra che il nodo funziona.
   Attenzione: la `k_msgq_get` con `K_FOREVER` non ritorna se l'acquisizione si ferma...
   usa un timeout (es. 2 s) e nutri il watchdog anche allo scadere, *se* sai che il sistema
   è comunque sano.
4. **Comando di test `blocca`**: fa entrare l'elaborazione in un ciclo infinito
   (`while (1) { k_busy_wait(1000); }`). Entro 5 s il watchdog deve riavviare il nodo:
   rivedi il banner di avvio e `NODO AVVIATO`.
5. **Dimensionamento**: attiva il thread analyzer (Modulo 7, esercizio 6), fai girare il
   nodo con tutti i comandi e un allarme completo, annota l'uso degli stack e ridimensionali
   (uso + 50%). Riporta i numeri nella specifica (N2, N3).

## Suggerimenti

- Quando il nodo si riavvia per il watchdog, Renode ricarica il firmware: la soglia salvata
  nella flash *simulata* si perde (la flash vera la conserverebbe). Annotalo tra i limiti.
- L'IWDG dell'STM32, una volta avviato, **non si può fermare**: in debug può essere
  scomodo. Cerca `WDT_OPT_PAUSE_HALTED_BY_DBG` (e non usare `WDT_OPT_PAUSE_IN_SLEEP`: il
  driver la rifiuta con `-ENOTSUP`, teoria 07 sezione 10).
- Le stringhe `ERRORE SENSORE` / `SENSORE OK` devono comparire esattamente così: la verifica
  le cerca.

## Completamento

- `.\verifica.ps1 modulo8\progetto -File modulo8\verifiche\06_robustezza.txt` → TUTTO OK.
- Le verifiche 04 e 05 passano ancora.

## Domande (rispondi qui)

1. Perché il watchdog si nutre dall'elaborazione e non da un thread dedicato?
2. Uso misurato e dimensione scelta per ogni stack:
3. Il dimensionamento della coda delle misure regge 3 s di elaborazione bloccata? Come lo
   hai verificato?
