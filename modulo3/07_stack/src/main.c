/*
 * Esercizio 7 (Modulo 3) - Lo stack
 *
 * Teoria: teoria/04_thread_scheduling.md, sezione 9.
 *
 * OBIETTIVO
 * Vedere lo stack "riempirsi": una funzione ricorsiva con un array locale consuma stack
 * a ogni chiamata. Misurare l'uso con il thread analyzer, poi esagerare e provocare
 * uno stack overflow, riconoscendolo dal messaggio di errore.
 *
 * PASSI
 * 1. Scrivi la funzione ricorsiva
 *        static int scendi(int livello)
 *    che:
 *      - dichiara un array locale  volatile uint8_t buf[64];  e scrive buf[0] = livello;
 *      - se livello == 0: stampa "profondita raggiunta" e restituisce 0;
 *      - altrimenti restituisce  scendi(livello - 1) + buf[0];
 *    (volatile e l'uso di buf dopo la chiamata impediscono al compilatore di togliere
 *    l'array o di trasformare la ricorsione in un ciclo.)
 * 2. Crea con K_THREAD_DEFINE il thread "ricorsione" con stack di 1024 byte e
 *    priorita' 5, che:
 *      - stampa "ricorsione: scendo di %d livelli" (usa la macro PROFONDITA);
 *      - chiama scendi(PROFONDITA) e stampa "ricorsione: risultato %d";
 *      - poi resta vivo dormendo: while (1) { k_msleep(1000); }
 *        (se terminasse, il thread analyzer non lo vedrebbe piu').
 * 3. In main(): k_msleep(500), poi  thread_analyzer_print(0);  e stampa
 *    "main: analisi fatta".
 * 4. Esegui con PROFONDITA 5. Nel report trova la riga "ricorsione": quanti byte usa
 *    su 1024? Prova PROFONDITA 10: di quanto cresce? Quanti byte costa UN livello?
 *    (Torna a 5 per la verifica normale.)
 * 5. Stima con il numero trovato a quale profondita' lo stack finisce. Poi metti
 *    PROFONDITA 40 ed esegui: errore fatale. Leggi il messaggio: deve parlare di stack
 *    overflow (o almeno di un errore fatale nel thread "ricorsione").
 *    Verifica questo caso con il file verifica_overflow.txt (vedi sotto).
 * 6. Rimetti PROFONDITA 5 e lancia la verifica normale.
 *
 * OUTPUT ATTESO (con PROFONDITA 5, numeri indicativi)
 *     ricorsione: scendo di 5 livelli
 *     profondita raggiunta
 *     ricorsione: risultato 15
 *     Thread analyze:
 *      ricorsione          : STACK: unused 520 usage 504 / 1024 (49 %); CPU: 0 %
 *      main                : STACK: ...
 *     main: analisi fatta
 *
 * OUTPUT ATTESO (con PROFONDITA 40)
 *     ricorsione: scendo di 40 livelli
 *     (forse ancora "profondita raggiunta" e "risultato": la sentinella si controlla
 *      solo al cambio di contesto, cioe' quando il thread va a dormire!)
 *     ... >>> ZEPHYR FATAL ERROR 2: Stack overflow on CPU 0
 *     (oppure un altro errore fatale, se la memoria sovrascritta ha rotto altro prima)
 *
 * CONCETTI NUOVI
 * - Stack frame: ogni chiamata di funzione aggiunge sullo stack le sue variabili
 *   locali, i registri salvati e l'indirizzo di ritorno.
 * - Lo stack di ogni thread ha dimensione FISSA: non c'e' memoria virtuale.
 * - Thread analyzer: all'avvio riempie gli stack con un valore noto (0xAA) e poi conta
 *   quanti byte sono stati sovrascritti = uso massimo ("high water mark").
 * - Stack sentinel: valore magico in fondo allo stack; se viene sovrascritto, al
 *   prossimo cambio di contesto il kernel ferma tutto con "Stack overflow".
 * - Alternativa hardware: la MPU del Cortex-M4 (qui spenta, vedi prj.conf).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo3\07_stack                                   (PROFONDITA 5)
 *     .\verifica.ps1 modulo3\07_stack -File modulo3\07_stack\verifica_overflow.txt
 *                                                                       (PROFONDITA 40)
 *
 * ERRORI TIPICI
 * - Il compilatore "ottimizza" la ricorsione e lo stack non cresce: assicurati di
 *   usare buf[0] DOPO la chiamata ricorsiva, e che buf sia volatile.
 * - thread_analyzer_print con argomento sbagliato: nelle versioni recenti vuole il
 *   numero della CPU (0); se la tua versione da' errore sugli argomenti, leggi la
 *   firma in zephyr/include/zephyr/debug/thread_analyzer.h.
 * - Pensare che 1024 byte siano "tanti": printk da sola ne usa qualche centinaio.
 *
 * BONUS (facoltativo)
 * - Stampa l'indirizzo di buf a ogni livello (printk("%p\n", (void *)buf)): gli
 *   indirizzi DIMINUISCONO. Lo stack del Cortex-M cresce verso il basso.
 * - Con CONFIG_THREAD_ANALYZER_AUTO=y e CONFIG_THREAD_ANALYZER_AUTO_INTERVAL=5 il
 *   report si stampa da solo ogni 5 secondi.
 * - Riduci lo stack del thread al minimo che regge PROFONDITA 5 con un margine del
 *   25%: e' cosi' che si dimensionano gli stack nei progetti veri.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/debug/thread_analyzer.h>

#define PROFONDITA 5

/* TODO: passi 1 e 2 */

int main(void)
{
	/* TODO: passo 3 */

	return 0;
}
