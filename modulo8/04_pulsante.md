# Passo 4 (Modulo 8) — Tacitare l'allarme (e battito)

Teoria: [teoria/09_progettazione.md](../teoria/09_progettazione.md), sezioni 2 e 4;
ripasso: `modulo6/02_pulsante_interrupt`, `modulo6/03_debounce`, `modulo5/01_k_timer`.

**Obiettivo:** l'operatore conferma l'allarme premendo il pulsante blu (SW0); il LED verde
lampeggia come "battito" per dire che il nodo è vivo. Requisiti coperti: **R6, R7**.

## Cosa fare

1. **`src/pulsante.c` / `pulsante.h`**: SW0 (alias `pulsante_conferma`, PA0) **a
   interrupt** con debounce (esercizio 6.03): interrupt su entrambi i fronti
   (`GPIO_INT_EDGE_BOTH`); la callback riprogramma un `k_work_delayable` di 50 ms con
   `k_work_reschedule`; quando il lavoro scatta legge il pin stabile, e solo il passaggio
   da rilasciato a premuto è una pressione valida.
   Nella callback (contesto di interrupt) **non** confermare l'allarme: al massimo
   riprogramma il lavoro. E neanche il lavoro conferma l'allarme: segnala soltanto la
   pressione valida (punto 2).
2. La pressione valida deve arrivare all'**elaborazione**, che possiede lo stato
   dell'allarme. Scegli il meccanismo (e scrivilo nella specifica, sezione 4):
   - un `k_sem` / `k_event` che l'elaborazione controlla;
   - oppure un messaggio "speciale" nella stessa coda delle misure (es. un campo `tipo`
     in `struct misura`): vantaggio, l'elaborazione aspetta una cosa sola.
   Qual è più semplice? Quale evita che l'elaborazione debba aspettare due cose insieme?
3. L'elaborazione chiama `allarme_conferma()`; se l'evento è `EVENTO_CONFERMATO`, spegne
   il LED rosso e stampa (formato obbligatorio):

   ```
   ALLARME CONFERMATO
   ```

4. **Battito sul LED verde** (alias `led_battito`, PD12): un `k_timer` periodico di 500 ms
   che inverte il LED. Si può fare nella callback del timer? (`gpio_pin_toggle_dt` è veloce
   e non blocca: sì; ma ricordati che sei in contesto di interrupt.) Mettilo in `uscite.c`.

## Suggerimenti

- Dal monitor di Renode: `sysbus.gpioPortA.sw0 Press`, poi `sysbus.gpioPortA.sw0 Release`.
  Per simulare un rimbalzo: `pause`, poi `Press`, `Release`, `Press`, poi `start` (macro
  `rimbalzo` della teoria 07, sezione 11).
- `pulsante.h` non deve dire *come* il pulsante è letto: sulla scheda vera potresti passare
  al campionamento con un `k_timer` (teoria 07, sezione 4.1) cambiando solo `pulsante.c`.
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
3. Il pulsante è su PA0, cioè sulla linea EXTI 0. Se aggiungessi un secondo pulsante su
   PB0 potresti usare l'interrupt per entrambi? Perché? (Teoria 07, sezione 4.)
