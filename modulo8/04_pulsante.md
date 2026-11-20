# Passo 4 (Modulo 8) — Tacitare l'allarme (e battito)

Teoria: [teoria/09_progettazione.md](../teoria/09_progettazione.md), sezioni 2 e 4;
ripasso: `modulo6/02_pulsante_campionato`, `modulo6/03_debounce`, `modulo5/01_k_timer`.

**Obiettivo:** l'operatore conferma l'allarme premendo SW0; LED1 lampeggia come "battito"
per dire che il nodo è vivo. Requisiti coperti: **R6, R7**.

## Cosa fare

1. **`src/pulsante.c` / `pulsante.h`**: SW0 (alias `pulsante_conferma`) **campionato**
   con un `k_timer` ogni 10 ms e debounce per campionamento (esercizio 6.03): la
   pressione è valida quando il pin resta premuto per 5 campioni consecutivi (50 ms).
   Nella funzione del timer (contesto di interrupt) **non** confermare l'allarme:
   segnala soltanto la pressione valida (punto 2). Se nel Modulo 6 hai scritto il
   debounce come libreria (`debounce.c/.h`), riusala.

   > Limite di Renode: gli interrupt GPIO non scattano nel simulatore, per questo il
   > pulsante si campiona. Sulla scheda vera potresti usare l'interrupt sul fronte +
   > un `k_work_delayable` di 50 ms (teoria 07, sezione 5): l'interfaccia di
   > `pulsante.h` resterebbe identica, cambierebbe solo `pulsante.c`.
2. La pressione valida deve arrivare all'**elaborazione**, che possiede lo stato
   dell'allarme. Scegli il meccanismo (e scrivilo nella specifica, sezione 4):
   - un `k_sem` / `k_event` che l'elaborazione controlla;
   - oppure un messaggio "speciale" nella stessa coda delle misure (es. un campo `tipo`
     in `struct misura`): vantaggio, l'elaborazione aspetta una cosa sola.
   Qual è più semplice? Quale evita che l'elaborazione debba aspettare due cose insieme?
3. L'elaborazione chiama `allarme_conferma()`; se l'evento è `EVENTO_CONFERMATO`, spegne
   LED0 e stampa (formato obbligatorio):

   ```
   ALLARME CONFERMATO
   ```

4. **Battito su LED1** (alias `led_battito`): un `k_timer` periodico di 500 ms che inverte
   il LED. Si può fare nella callback del timer? (`gpio_pin_toggle_dt` è veloce e non
   blocca: sì; ma ricordati che sei in contesto di interrupt.) Mettilo in `uscite.c`.

## Suggerimenti

- Dal monitor di Renode: `sysbus.gpio0.sw0 Press`, poi `sysbus.gpio0.sw0 Release`.
  Per simulare un rimbalzo: `Press`, `Release`, `Press` in rapida successione (macro
  `rimbalzo` della teoria 07, sezione 11).
- Il timer dei pulsanti e quello del battito possono essere lo stesso? Si', ma tenerli
  separati rende `pulsante.c` e `uscite.c` indipendenti.
- La pressione con allarme non attivo non deve fare nulla (e non deve stampare
  `ALLARME CONFERMATO`).
- Il battito sarà più utile al passo 6: se il sistema si pianta, il LED smette di
  lampeggiare? (Dipende da *dove* lo fai lampeggiare: pensaci.)

## Completamento

- `.\verifica.ps1 modulo8\progetto -File modulo8\verifiche\04_pulsante.txt` → TUTTO OK.
- Le verifiche dei passi 2 e 3 passano ancora.

## Domande (rispondi qui)

1. Quale meccanismo hai scelto per far arrivare la pressione all'elaborazione, e perché?
2. Se il battito è fatto da un `k_timer`, continua a lampeggiare quando il thread di
   elaborazione è bloccato? È un buon indicatore di "sistema vivo"? Come lo cambieresti?
