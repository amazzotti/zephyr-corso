/*
 * Esercizio 4 (Modulo 7) - Errori fatali
 *
 * Teoria: teoria/08_strumenti_debug_test.md, sezione 4.
 *
 * OBIETTIVO
 * Provocare apposta un errore fatale in un thread, leggere il rapporto del kernel e
 * scrivere un handler che termina solo il thread colpevole, lasciando vivo il resto
 * del sistema.
 *
 * PASSI
 * 1. Completa il thread "fragile" (gia' dichiarato con K_THREAD_DEFINE): stampa
 *        fragile: conto 1     (poi 2, poi 3, uno al secondo)
 *        fragile: ora provoco un errore
 *    e poi, secondo la macro PROVA:
 *        PROVA 1:  k_oops();
 *        PROVA 2:  __ASSERT(1 + 1 == 3, "aritmetica rotta");   (serve CONFIG_ASSERT=y)
 *        PROVA 3:  __builtin_trap();    (istruzione non valida -> eccezione della CPU)
 *    Usa #if PROVA == 1 ... #elif ... #endif.
 * 2. In main: ciclo infinito che ogni secondo stampa "main ancora vivo (N)".
 * 3. Esegui con PROVA 1 SENZA handler: leggi il rapporto. Cosa succede a main?
 *    (Suggerimento: l'handler predefinito ferma tutto il sistema.)
 * 4. Scrivi il tuo handler:
 *        void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
 *    (con #include <zephyr/fatal.h>; nelle versioni di Zephyr prima della 3.7 il tipo e'
 *    z_arch_esf_t). Deve stampare con printk
 *        HANDLER: errore <reason> nel thread <nome>, lo termino
 *    (nome con k_thread_name_get(k_current_get())) e poi RITORNARE. Ora main deve
 *    continuare a stampare "main ancora vivo".
 * 5. Prova PROVA 2 e PROVA 3 e annota i codici di "reason" (domande in fondo).
 * 6. Rimetti PROVA 1 e lancia la verifica.
 *
 * OUTPUT ATTESO (PROVA 1, con l'handler)
 *     fragile: conto 1
 *     main ancora vivo (1)
 *     ...
 *     fragile: ora provoco un errore
 *     [..] <err> os: >>> ZEPHYR FATAL ERROR 3: Kernel oops on CPU 0
 *     [..] <err> os: Current thread: 0x2000.... (fragile)
 *     HANDLER: errore 3 nel thread fragile, lo termino
 *     main ancora vivo (4)
 *     main ancora vivo (5)
 *
 * CONCETTI NUOVI
 * - Errore fatale e "reason" (tabella nella teoria).
 * - Fault della CPU Cortex-M: HardFault, UsageFault, BusFault, MemManage.
 * - Registri nel rapporto: pc = istruzione colpevole, lr = da dove era stata chiamata.
 * - Handler fatale: la "politica" di cosa fare in caso di errore la decidi tu.
 * - Thread essenziali: se muore main (o un thread del kernel) il sistema si ferma comunque.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo7\04_errori_fatali
 * (con PROVA 1) controlla il rapporto, il messaggio dell'handler e che main continui.
 *
 * ERRORI TIPICI
 * - Provocare l'errore in main invece che nel thread fragile: main e' essenziale, il
 *   sistema si ferma anche con il tuo handler.
 * - Firma dell'handler diversa: il linker usa quello predefinito e il tuo viene ignorato
 *   (o errore "conflicting types").
 * - Fare cose complicate nell'handler (sleep, mutex): siamo in una situazione di errore,
 *   bisogna fare il minimo.
 * - PROVA 2 senza CONFIG_ASSERT=y: l'assert non viene neanche compilato, non succede niente.
 *
 * BONUS (facoltativo)
 * - PROVA 4: scrivi un valore all'indirizzo 0xFFFFFFF0:
 *       *(volatile uint32_t *)0xFFFFFFF0 = 1;
 *   Sulla scheda vera e' un BusFault. In Renode cosa succede? (logLevel 2 nel monitor
 *   per vedere gli avvisi.) Che cosa ti dice questo sui limiti di un simulatore?
 * - Nel rapporto della PROVA 3 trova il valore di pc; cerca quell'indirizzo in
 *   build/zephyr/zephyr.map (o con arm-zephyr-eabi-addr2line -e build/zephyr/zephyr.elf
 *   <indirizzo>): e' la riga di __builtin_trap?
 * - Nell'handler accendi led0 come "spia di errore".
 *
 * DOMANDE (rispondi qui)
 * 1. Codice di reason per PROVA 1, 2 e 3:
 * 2. Senza handler, main continua? Perche'?
 * 3. Con PROVA 3, che tipo di fault riporta la CPU?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/fatal.h>
#include <zephyr/sys/printk.h>

#define PROVA 1    /* 1 = k_oops, 2 = __ASSERT, 3 = istruzione non valida */

static void fragile_fn(void *p1, void *p2, void *p3)
{
	/* TODO: conto fino a 3, poi l'errore scelto da PROVA. */
}

/* Thread "fragile": stack 1024, priorita' 5, parte subito. */
K_THREAD_DEFINE(fragile, 1024, fragile_fn, NULL, NULL, NULL, 5, 0, 0);

/* TODO: k_sys_fatal_error_handler. */

int main(void)
{
	/* TODO: ciclo infinito "main ancora vivo (N)" ogni secondo. */
	return 0;
}
