# Passo 7 (Modulo 8) — Test e relazione

Teoria: [teoria/08_strumenti_debug_test.md](../teoria/08_strumenti_debug_test.md), sezione 3;
[teoria/09_progettazione.md](../teoria/09_progettazione.md), sezioni 9 e 10.

**Obiettivo:** dimostrare che il nodo rispetta i requisiti e raccontare il progetto.
Requisito coperto: **R12**, più la verifica di tutti gli altri.

## Parte A — Test della logica (ztest)

L'applicazione di test è già pronta in `modulo8/test_logica/`: compila `filtro.c`,
`allarme.c` e (se esiste) `formato.c` **direttamente dalla cartella del progetto**.
Scrivi i test in `modulo8/test_logica/src/main.c` (le istruzioni sono lì). Minimo:

| Suite | Test |
|---|---|
| `filtro` | media con 1 valore; con 5; con 7 (i primi 2 sono usciti); valori negativi; init azzera |
| `allarme` | resta NO sotto soglia; scatta esattamente a soglia; non rientra dentro l'isteresi; rientra sotto soglia − isteresi; conferma solo da ATTIVO; da CONFERMATO rientra |
| `formato` (se hai `formato.c`) | `"28.5"`, `"-3.25"`, `"30"`, rifiuti `"abc"`, `"1.234"`, `"12,5"` |

```powershell
.\verifica.ps1 modulo8\test_logica
```

## Parte B — Verifica completa del firmware

```powershell
.\verifica.ps1 modulo8\progetto -File modulo8\verifiche\07_completa.txt
```

Percorre in un colpo solo: soglia cambiata dalla shell, allarme, conferma, rientro, guasto
del sensore. Deve dare TUTTO OK insieme alle verifiche 04, 05, 06.

## Parte C — Tabella requisiti → verifiche

Completa (nella relazione):

| Requisito | Verificato da | Esito |
|---|---|---|
| R1 | ... | ... |
| ... | ... | ... |

## Parte D — Relazione

Crea `modulo8/relazione.md` partendo da questo modello. Breve, concreta, con numeri.

```markdown
# Nodo di monitoraggio della temperatura — Relazione

## 1. Obiettivo
(Due o tre frasi: cosa fa il nodo, su quale piattaforma, simulata con Renode.)

## 2. Requisiti
(Tabella R1-R12 e N1-N4, rimanda a 01_specifica.md per il dettaglio.)

## 3. Architettura
- Diagramma di thread, code e periferiche.
- Tabella delle attività: tipo, priorità, periodo, stack misurato / assegnato.
- Macchina a stati dell'allarme.

## 4. Scelte progettuali
(Almeno tre scelte, ciascuna con l'alternativa scartata e il perché. Esempi: coda contro
variabile globale; interi in centesimi contro float; dove nutrire il watchdog; come arriva
la pressione del pulsante all'elaborazione.)

## 5. Verifica
- Test ztest: quanti, cosa coprono, risultato.
- Verifiche in Renode: elenco e risultato.
- Tabella requisiti → verifiche.

## 6. Risorse
- FLASH e RAM (tabella Memory region), le 3 voci più grandi da ram_report.
- Uso degli stack (thread analyzer).

## 7. Limiti
(Cosa non funziona o non è verificato. Cosa cambierebbe sulla scheda vera: flash simulata,
rimbalzi reali del pulsante, tempi reali, sensore vero.)

## 8. Cosa ho imparato
(Su C embedded, RTOS, microcontrollori: le tre cose più importanti e la più difficile.)
```

## Completamento

- `test_logica` → TUTTO OK; `07_completa.txt` → TUTTO OK.
- `modulo8/relazione.md` scritta e senza parti vuote.
- Commit finale. Aggiorna `CORSO.md` (sezioni 7 e 8): il corso è finito.

## Domande finali (rispondi nella relazione)

1. Quale bug hai trovato grazie ai test e che altrimenti sarebbe arrivato "sulla scheda"?
2. Se dovessi portare il nodo su una scheda vera con un altro sensore di temperatura, quali
   file cambieresti? Quali restano identici?
