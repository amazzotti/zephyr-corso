# 09 — Progettare un firmware (Modulo 8)

Negli esercizi la struttura te la davo io. Nel progetto la decidi tu. Questa teoria è un
metodo per passare da "cosa deve fare" a "come è fatto", prima di scrivere codice.

## 1. Dalla richiesta ai requisiti

Un **requisito** è una frase verificabile. "Deve controllare la temperatura" non lo è;
"deve leggere la temperatura ogni secondo e accendere LED0 entro 10 s da quando la media
supera la soglia" sì: si può scrivere un test che lo controlla.

- **Funzionali**: cosa fa (letture, allarmi, comandi).
- **Non funzionali**: con quali vincoli (tempi, memoria, robustezza, consumo).

Numerali (`R1`, `R2`...): li userai nei test e nella relazione per dire "R3 è verificato da...".

## 2. Scomporre in attività

Per ogni cosa da fare chiediti: **quando** deve succedere e **quanto dura**?

| Tipo di attività | Strumento Zephyr |
|---|---|
| reazione immediata e brevissima a un evento hardware | **ISR** / callback (es. UART, timer; il pulsante sulla scheda vera): solo registrare l'evento |
| controllo periodico di un ingresso | **k_timer** che campiona (es. pulsante con debounce, come in questo corso) |
| lavoro breve dopo un evento o un ritardo | **work** / **delayable work** |
| attività periodica regolare, breve | **k_timer** + work, oppure thread con sleep |
| attività che si blocca (attende I2C, attende una coda) | **thread** proprio |
| comandi dall'utente | shell (gira nel suo thread) |

Regola d'oro: nell'ISR non si aspetta mai e non si fa nulla di lungo.

## 3. Priorità

In Zephyr numero **più basso = priorità più alta**. Criterio pratico: più è stretto il vincolo
di tempo, più alta la priorità.

- Acquisizione dal sensore (tempi regolari): alta.
- Elaborazione (deve stare al passo con l'acquisizione, ma può ritardare un po'): media.
- Battito, statistiche, shell: bassa.

Attenzione all'**inversione di priorità** (Modulo 4): se un thread a bassa priorità tiene un
mutex che serve a uno ad alta, usa `k_mutex` (che ha l'ereditarietà di priorità), non un
semaforo.

## 4. Comunicazione tra attività

| Situazione | Scelta |
|---|---|
| dati che scorrono da un thread a un altro (misure) | `k_msgq`: copia dei dati, nessuna memoria condivisa |
| "è successo qualcosa" senza dati | `k_sem` o `k_event` |
| stato letto da molti, scritto da uno (soglia, ultima media) | variabile protetta da `k_mutex`, o `atomic_t` se è un intero |
| da ISR a thread | `k_msgq_put` / `k_sem_give` con `K_NO_WAIT` |

Dimensiona le code: se il produttore produce 1 misura/s e il consumatore può fermarsi al
massimo 3 s, servono almeno 3-4 posti. Decidi cosa fare **se la coda è piena** (scartare la
più vecchia? la nuova? contare le perse?).

## 5. Macchine a stati

Un allarme non è "acceso/spento": ha una storia. Disegna gli **stati** e le **transizioni**:

```
            media >= soglia                 pulsante
   [NO] ----------------------> [ATTIVO] -------------> [CONFERMATO]
     ^                             |                          |
     |   media < soglia-isteresi   |                          |
     +-----------------------------+--------------------------+
```

L'**isteresi** evita lo "sfarfallio": con soglia 30.00 °C e isteresi 1.00 °C l'allarme entra
a 30.00 e rientra solo sotto 29.00. Senza isteresi, una temperatura che oscilla tra 29.99 e
30.01 accenderebbe e spegnerebbe l'allarme a ogni misura.

Implementa la macchina a stati come in Modulo 1: `enum` per gli stati, una funzione che
riceve l'ingresso e restituisce l'evento (cambio di stato).

## 6. Separare la logica dall'hardware

Dividi il codice in due tipi di file:

- **logica pura** (`filtro.c`, `allarme.c`): solo calcoli su numeri e struct. Niente GPIO,
  niente sensori, niente `k_sleep`. Si testa con ztest, anche senza scheda.
- **adattatori** (`sensore.c`, `uscite.c`, `comandi.c`): parlano con driver e kernel e
  chiamano la logica.

Così se cambi sensore cambi un file solo, e se un test fallisce sai che il problema è nella
logica e non nella simulazione.

**Numeri a virgola fissa.** Il nRF52840 ha una FPU, ma nel progetto usiamo interi in
**centesimi di grado** (`int32_t`, 2345 = 23.45 °C): confronti esatti, test semplici,
stampa con `%d.%02d`. Attenzione ai negativi: -0.50 °C è `-50`, e `-50 / 100 = 0` (si perde
il segno); stampa il segno a parte.

## 7. Errori e robustezza

Chiediti per ogni componente: **cosa succede se si guasta?**

- Sensore che non risponde: riprova, conta gli errori consecutivi, dopo N segnala
  `ERRORE SENSORE` e non usare valori vecchi come se fossero nuovi.
- Thread bloccato: il **watchdog** è un timer hardware che riavvia il chip se nessuno lo
  "nutre" (`wdt_feed`) entro un tempo. Va nutrito dal punto che dimostra che **il lavoro
  importante procede** (es. l'elaborazione che riceve misure), non da un thread che non fa
  altro che nutrirlo.
- Valori fuori range (comando `soglia set 9999`): rifiutarli.
- **Iniezione di guasti**: un comando di test che simula il guasto (es. `guasto on`) è il modo
  più semplice per verificare che la gestione degli errori funzioni davvero.

## 8. Dimensionare

- **Stack**: parti da 1024 byte per thread, misura con il thread analyzer, poi metti
  uso + 50%.
- **Code**: in base ai tempi (sezione 4).
- **Storico**: N misure × `sizeof(struct misura)` byte di RAM: fai il conto.

Riporta i numeri nella relazione: dimensionare "a occhio" non è un metodo.

## 9. Verificare

Tre livelli, dal più veloce al più completo:
1. **ztest** sulla logica pura (filtro, isteresi): tanti casi limite in un secondo.
2. **verifica.ps1** sul firmware completo in Renode: comportamento visto da fuori (UART, LED,
   pulsanti, temperatura cambiata dal monitor).
3. **prova manuale** con `esegui.ps1` e la shell, per le cose difficili da automatizzare.

Ogni requisito deve essere coperto da almeno un controllo: fai una tabella requisito → test.

## 10. La relazione

Breve e concreta (il modello è in `modulo8/07_test_relazione.md`):
1. obiettivo e requisiti;
2. architettura: thread, priorità, code, diagramma;
3. scelte e alternative scartate (perché una coda e non una variabile globale?);
4. risultati: test superati, memoria usata, stack misurati;
5. limiti (cosa non funziona o non è stato verificato, cosa cambia sulla scheda vera);
6. cosa ho imparato.
