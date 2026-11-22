# Passo 5 (Modulo 8) — Shell e configurazione

Teoria: [teoria/08_strumenti_debug_test.md](../teoria/08_strumenti_debug_test.md), sezioni 1,
2 e 7; [teoria/09_progettazione.md](../teoria/09_progettazione.md), sezione 4.

**Obiettivo:** l'operatore guarda lo stato e cambia la soglia dalla shell; la soglia viene
salvata e sopravvive al "riavvio". Requisiti coperti: **R8, R9**.

## Cosa fare

1. **Configurazione**: la shell è già attiva dal passo 2 (la porta `comune/sensore_sim.conf`
   per il comando `sim`). Aggiungi in `prj.conf` i settings su flash simulata (copia le
   righe da `modulo7/08_riepilogo/prj.conf`). `app.overlay` ha già la flash simulata.
2. **Niente più printk per le misure**: `MISURA` e `MEDIA` ogni secondo rendono la shell
   inutilizzabile (prova a scrivere un comando mentre arrivano!). Trasformale in `LOG_DBG` (spariscono, ma le
   riaccendi con `log enable dbg <modulo>`). Gli eventi dell'allarme diventano `LOG_INF`
   con lo stesso testo. Da questo passo le verifiche 02 e 03 non valgono più (è previsto).
3. **`src/config.c` / `config.h`**: soglia e isteresi correnti, protette da `k_mutex`
   (le scrive la shell, le legge l'elaborazione).
   - `config_init()`: `settings_subsys_init`, `settings_load` (handler per il prefisso
     `"nodo"`, chiave `"nodo/soglia"`, `int32_t` in centesimi);
   - `int config_set_soglia(int32_t c100)`: controlla l'intervallo **-40.00 .. 85.00 °C**
     (il campo di lavoro del nodo; l'ADT7420 misura da -40 a +150 °C), aggiorna, salva con `settings_save_one`, ritorna 0 o `-EINVAL`;
   - `int32_t config_get_soglia(void)`.
   L'elaborazione, a ogni misura, copia la soglia corrente in `struct allarme`.
4. **Storico**: l'elaborazione tiene le ultime 10 misure valide in un buffer circolare
   (protetto: chi lo legge?).
5. **`src/comandi.c`**: comandi della shell. **Formati obbligatori:**

   ```
   uart:~$ stato
   Temperatura media: 22.50 C
   Soglia: 30.00 C
   Isteresi: 1.00 C
   Allarme: NO                  (oppure ATTIVO, CONFERMATO)
   Misure perse: 0
   uart:~$ soglia get
   Soglia: 30.00 C
   uart:~$ soglia set 28.5
   Soglia impostata: 28.50 C (salvata)
   uart:~$ soglia set abc
   valore non valido: abc       (con shell_error)
   uart:~$ storico
   #12 t=12003 ms T=22.50 C
   #11 t=11003 ms T=22.50 C
   ...
   ```

   Per `soglia set` scrivi un parser di numeri con al massimo due decimali **senza float**:
   `"28.5"` → `2850`, `"-3.25"` → `-325`, `"30"` → `3000`; rifiuta `"abc"`, `"1.234"`,
   `"12,5"`. Mettilo in un file puro (`formato.c`): al passo 7 lo testerai.

## Suggerimenti

- Prova il "riavvio" senza perdere la flash simulata: un comando di prova `riavvia_config`
  che rimette la soglia al valore predefinito e richiama `settings_load()`. La soglia salvata
  torna? (Con `machine Reset` invece si perde: perché? Vedi `modulo7/07_settings`.)
- `stato` legge dati di più thread: prendi i mutex per il tempo minimo, copia, rilascia,
  poi stampa.

## Completamento

- `.\verifica.ps1 modulo8\progetto -File modulo8\verifiche\05_shell_config.txt` → TUTTO OK.
- La verifica 04 passa ancora (gli eventi dell'allarme hanno lo stesso testo).

## Domande (rispondi qui)

1. Perché la soglia si salva in centesimi interi e non come stringa o float?
2. Chi può bloccarsi in attesa del mutex della configurazione, e per quanto al massimo?
3. Quanta RAM in più hanno aggiunto shell e settings? (Tabella "Memory region" prima/dopo.)
