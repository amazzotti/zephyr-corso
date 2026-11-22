# Passo 3 (Modulo 8) — Elaborazione

Teoria: [teoria/09_progettazione.md](../teoria/09_progettazione.md), sezioni 5 e 6.

**Obiettivo:** trasformare le misure in decisioni. Media mobile delle ultime 5 misure,
macchina a stati dell'allarme con isteresi, il LED rosso come uscita. La logica va in file **puri**
(senza hardware) che al passo 7 testerai con ztest. Requisiti coperti: **R3, R4, R5**.

## Interfacce (obbligatorie: le usano i test del passo 7)

`src/filtro.h`

```c
#define FILTRO_N 5

struct filtro {
	int32_t valori[FILTRO_N];
	size_t indice;      /* dove scrivere il prossimo valore */
	size_t conteggio;   /* quanti valori validi (max FILTRO_N) */
	int64_t somma;      /* somma dei valori presenti */
};

void filtro_init(struct filtro *f);
/* Aggiunge un valore e restituisce la media (intera) dei valori presenti. */
int32_t filtro_aggiungi(struct filtro *f, int32_t valore);
```

`src/allarme.h`

```c
enum allarme_stato  { ALLARME_NO, ALLARME_ATTIVO, ALLARME_CONFERMATO };
enum allarme_evento { EVENTO_NESSUNO, EVENTO_ATTIVATO, EVENTO_RIENTRATO, EVENTO_CONFERMATO };

struct allarme {
	int32_t soglia;      /* centesimi di grado */
	int32_t isteresi;    /* centesimi di grado */
	enum allarme_stato stato;
};

void allarme_init(struct allarme *a, int32_t soglia, int32_t isteresi);
/* Da chiamare a ogni nuova media: aggiorna lo stato e dice cosa e' cambiato. */
enum allarme_evento allarme_aggiorna(struct allarme *a, int32_t media);
/* Da chiamare quando si preme il pulsante (passo 4). */
enum allarme_evento allarme_conferma(struct allarme *a);
```

Regole:
- `NO` → `ATTIVO` quando `media >= soglia`.
- `ATTIVO` o `CONFERMATO` → `NO` quando `media < soglia - isteresi` (evento `RIENTRATO`).
- `allarme_conferma`: solo da `ATTIVO` porta a `CONFERMATO` (evento `CONFERMATO`);
  negli altri stati non fa nulla (`EVENTO_NESSUNO`).
- `filtro.c` e `allarme.c` **non includono** niente di Zephyr tranne i tipi
  (`<stdint.h>`, `<stddef.h>`, `<stdbool.h>`): niente GPIO, niente log, niente kernel.

## Cosa fare

1. Scrivi `filtro.c` e `allarme.c` secondo le interfacce. La media mobile usa un buffer
   circolare (Modulo 1!): togli dalla somma il valore che esce, aggiungi quello che entra.
2. **`src/uscite.c` / `uscite.h`**: `uscite_init()` prepara il LED rosso (alias
   `led_allarme`, PD14) e `uscite_allarme(bool acceso)`.
3. In `elabora.c`, per ogni misura valida: `media = filtro_aggiungi(...)`,
   `ev = allarme_aggiorna(...)`, aggiorna il LED rosso e stampa. **Formati obbligatori:**

   ```
   MEDIA T=<gradi>.<centesimi> C
   ALLARME ATTIVO (media <xx.yy> >= soglia <xx.yy>)
   ALLARME RIENTRATO (media <xx.yy>)
   ```

   Le misure con errore **non** entrano nel filtro.
4. Soglia e isteresi predefinite: 30.00 °C e 1.00 °C (`3000` e `100`).

## Suggerimenti

- Prova a mano: nella console `sim temp 3500`, aspetta qualche secondo (la media sale
  gradualmente: dopo quante misure scatta l'allarme?), poi `sim temp 2000`.
- Il LED rosso si controlla dal monitor di Renode con `sysbus.gpioPortD.led2 State`.
- Attenzione all'overflow: perché `somma` è `int64_t`? Con quali valori un `int32_t`
  basterebbe?

## Completamento

- `.\verifica.ps1 modulo8\progetto -File modulo8\verifiche\03_elaborazione.txt` → TUTTO OK.
- Anche le verifiche del passo 2 passano ancora.

## Domande (rispondi qui)

1. Con temperatura che salta da 20 a 35 °C, dopo quante misure la media supera 30.00?
   Calcolalo a mano e confronta con quello che vedi.
2. Cosa succederebbe senza isteresi se la temperatura oscillasse tra 29.95 e 30.05?
3. Perché la logica in file "puri" rende il progetto più facile da testare e da portare su
   un'altra scheda?
