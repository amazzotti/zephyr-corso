/*
 * Esercizio 6 (Modulo 2) - Overlay
 *
 * Teoria: teoria/03_build_kconfig_devicetree.md, sezioni 6, 7 e 8.
 *
 * OBIETTIVO
 * Modificare il devicetree della scheda SENZA toccare i file di Zephyr, con un file
 * app.overlay: aggiungere un alias "allarme" che punta al LED 2 e dei parametri
 * dell'applicazione nel nodo zephyr,user. Poi leggerli dal codice.
 *
 * PASSI
 * 1. Completa app.overlay (le istruzioni sono nel file).
 * 2. Compila con -Pulito e apri build/zephyr/zephyr.dts: cerca "allarme" e
 *    "zephyr,user". Ci sono? L'overlay e' stato unito al devicetree della scheda.
 * 3. Nel codice leggi i parametri e stampali:
 *        "Nodo: %s"    con DT_PROP(DT_PATH(zephyr_user), nome_nodo)
 *        "Soglia: %d"  con DT_PROP(DT_PATH(zephyr_user), soglia)
 * 4. Crea una gpio_dt_spec per l'alias allarme (come nel blinky, ma con
 *    DT_ALIAS(allarme)), configurala come uscita spenta, accendila e stampa
 *    "Allarme acceso". Poi termina main (il LED resta acceso).
 * 5. In Renode controlla: sysbus.gpio0.led2 State deve valere True.
 * 6. Prova: cambia nell'overlay l'alias in &led3 e ricompila. Senza toccare il .c,
 *    ora si accende il LED 3. Questo e' il punto dei devicetree: il codice non sa
 *    su quale pin e' l'allarme. Rimetti &led2 prima della verifica.
 * 7. Lancia la verifica.
 *
 * OUTPUT ATTESO
 *     Nodo: nodo-01
 *     Soglia: 30
 *     Allarme acceso
 *
 * CONCETTI NUOVI
 * - app.overlay: modifiche al devicetree della board limitate a questa applicazione.
 * - aliases: un nome tuo per un nodo esistente (&led2 = riferimento all'etichetta led2).
 * - zephyr,user: nodo libero per i parametri dell'applicazione.
 * - "nome-nodo" nel devicetree diventa nome_nodo nelle macro; "zephyr,user" diventa
 *   zephyr_user.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo2\06_overlay -Pulito
 *
 * ERRORI TIPICI
 * - Dimenticare il ";" dopo una proprieta' o dopo la "}" di un nodo: errore del
 *   compilatore del devicetree (dtc) con il numero di riga dell'overlay.
 * - Numeri senza < >: soglia = 30; e' sbagliato, va soglia = <30>;
 * - Modifiche all'overlay non viste: ricompila con -Pulito.
 * - Usare DT_ALIAS(allarme) senza aver aggiunto l'alias: errore lungo con
 *   "DT_N_ALIAS_allarme".
 *
 * BONUS (facoltativo)
 * - Nell'overlay cambia la velocita' della console:  &uart0 { current-speed = <9600>; };
 *   e stampala come nell'esercizio 4. (In Renode la UART funziona a qualunque
 *   velocita'; su una scheda vera il terminale del PC andrebbe impostato uguale.)
 * - Aggiungi a zephyr,user una proprieta' con un array:  limiti = <10 20 30>;
 *   e leggine la lunghezza con DT_PROP_LEN e un elemento con DT_PROP_BY_IDX.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/devicetree.h>
#include <zephyr/drivers/gpio.h>

/* TODO: passo 4 - la gpio_dt_spec per l'alias allarme */

int main(void)
{
	/* TODO: passi 3 e 4 */

	return 0;
}
