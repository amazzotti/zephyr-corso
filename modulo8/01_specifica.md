# Passo 1 (Modulo 8) — Specifica e architettura

Teoria: [teoria/09_progettazione.md](../teoria/09_progettazione.md), sezioni 1-5 e 8.

**Obiettivo:** prima di scrivere una riga di codice, decidere *cosa* fa il nodo e *come* è
organizzato. Questo file è un **modello da compilare**: riempi le parti segnate con `...`.
Le scelte possono cambiare durante il progetto: quando succede, aggiorna questo file e
scrivi perché (servirà nella relazione).

---

## 1. Descrizione in una frase

Il nodo legge la temperatura da un sensore ADT7420 (I2C), ne calcola la media mobile e
segnala con un LED quando supera una soglia configurabile; l'operatore conferma l'allarme con un
pulsante e configura il nodo dalla shell.

**Hardware** (STM32F4 Discovery simulata in Renode, `progetto/app.overlay`):

| Ruolo | Componente | Alias nel devicetree |
|---|---|---|
| Sensore di temperatura | ADT7420 su I2C, indirizzo `0x48` (emulato da Zephyr, `comune/`) | nodo `sensore` |
| LED di allarme | LED rosso, PD14 | `led_allarme` |
| LED di battito | LED verde, PD12 | `led_battito` |
| Pulsante di conferma | pulsante blu, PA0 (attivo alto, interrupt EXTI0) | `pulsante_conferma` |
| Watchdog | IWDG | `watchdog0` |
| Console e shell | USART2 | `zephyr,console` |

## 2. Requisiti

### Funzionali (già fissati: sono quelli che le verifiche controllano)

| Id | Requisito | Passo |
|---|---|---|
| R1 | Il nodo legge la temperatura dal sensore (ADT7420, sensor API) ogni 1 s. | 2 |
| R2 | Ogni misura (tempo, temperatura in centesimi di °C, esito) passa all'elaborazione tramite una coda. | 2 |
| R3 | L'elaborazione calcola la media mobile delle ultime 5 misure valide. | 3 |
| R4 | Se la media è ≥ soglia (predefinita 30.00 °C) l'allarme si attiva e il LED rosso (allarme) si accende. | 3 |
| R5 | L'allarme rientra quando la media scende sotto soglia − isteresi (predefinita 1.00 °C); il LED rosso si spegne. | 3 |
| R6 | Premendo il pulsante blu (SW0) con allarme attivo l'allarme passa a "confermato" e il LED rosso si spegne; il pulsante è gestito a interrupt con un debounce di 50 ms. | 4 |
| R7 | Il LED verde lampeggia (battito, 500 ms acceso / 500 ms spento) finché il sistema è vivo. | 4 |
| R8 | Dalla shell: `stato`, `soglia get`, `soglia set <°C>`, `storico`. | 5 |
| R9 | La soglia impostata viene salvata e ricaricata all'avvio. | 5 |
| R10 | Dopo 3 letture fallite consecutive il nodo segnala `ERRORE SENSORE`; alla prima lettura buona `SENSORE OK`. | 6 |
| R11 | Se l'elaborazione si blocca, il watchdog riavvia il nodo entro 5 s. | 6 |
| R12 | Filtro e allarme sono coperti da test automatici (ztest). | 7 |

### Non funzionali (completa)

| Id | Requisito |
|---|---|
| N1 | Temperature come interi in centesimi di grado (`int32_t`): niente float. Perché: ... |
| N2 | Nessuno stack usato oltre il ...% (misurato con il thread analyzer). |
| N3 | RAM totale usata sotto ... KB. |
| N4 | ... |

## 3. Attività (thread, work, ISR)

Completa la tabella (teoria, sezioni 2 e 3). Una riga per attività.

| Attività | Tipo (thread / work / ISR / timer) | Priorità | Periodo o evento | Stack (iniziale) | Perché questa scelta |
|---|---|---|---|---|---|
| Acquisizione | thread | ... | 1 s | 1024 | ... |
| Elaborazione | ... | ... | arrivo di una misura | ... | ... |
| Pulsante | ISR (EXTI) + ... | – | pressione | – | ... |
| Battito | ... | ... | 500 ms | ... | ... |
| Shell | thread (di Zephyr) | – | comando | – | – |
| Watchdog | ... | ... | ... | ... | ... |

## 4. Comunicazione

| Da → A | Cosa passa | Meccanismo | Dimensione | Se pieno / vuoto |
|---|---|---|---|---|
| Acquisizione → Elaborazione | `struct misura` | `k_msgq` | ... posti | ... |
| Pulsante → Elaborazione | "premuto" | ... | – | – |
| Shell ↔ Elaborazione | soglia, stato, storico | ... (mutex?) | – | – |

`struct misura` (scrivi i campi e i tipi):

```c
struct misura {
	...
};
```

Quanti byte occupa? ...

## 5. Diagramma

Disegna (ASCII, o un'immagine nella cartella) thread, code e periferiche, con le frecce dei
dati. Esempio di partenza da completare:

```
 ADT7420 --I2C--> [Acquisizione] --msgq--> [Elaborazione] --> LED rosso (allarme)
                                               ^      |
                          SW0 --ISR--> ... -----+      +--> storico
                                                       
 [Shell] <-- comandi --> ...                LED verde (battito) <-- ...
```

## 6. Macchina a stati dell'allarme

Completa stati e transizioni (teoria, sezione 5):

| Stato attuale | Evento | Nuovo stato | Azione |
|---|---|---|---|
| NO | media ≥ soglia | ATTIVO | LED rosso acceso, stampa `ALLARME ATTIVO ...` |
| ATTIVO | pulsante | ... | ... |
| ATTIVO | media < soglia − isteresi | ... | ... |
| CONFERMATO | ... | ... | ... |

Domanda: se in stato CONFERMATO la temperatura scende sotto la soglia − isteresi e poi
risale sopra la soglia, cosa deve succedere? ...

## 7. Rischi e domande aperte

- Cosa succede se il sensore non risponde all'avvio? ...
- Cosa succede se la coda delle misure si riempie? ...
- ...

---

**Completamento:** tutte le tabelle compilate, nessun `...` rimasto. Fai commit di questo file
prima di passare al passo 2.
