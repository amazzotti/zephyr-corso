/*
 * Esercizio 2 (Modulo 7) - Comandi della shell propri
 *
 * Teoria: teoria/08_strumenti_debug_test.md, sezione 2.
 *
 * OBIETTIVO
 * Aggiungere alla shell tre comandi scritti da te:
 *     led on | off | stato    accende, spegne, mostra lo stato di led0
 *     conta                   incrementa un contatore e lo stampa
 *     somma A B               somma due numeri interi, con controllo degli errori
 *
 * PASSI
 * 1. In main prepara led0 come nel Modulo 2 (GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios),
 *    gpio_is_ready_dt, gpio_pin_configure_dt con GPIO_OUTPUT_INACTIVE). La variabile
 *    gpio_dt_spec deve essere globale (static, fuori dalle funzioni): la usano i comandi.
 * 2. Scrivi tre funzioni con la firma dei comandi:
 *        static int cmd_led_on(const struct shell *sh, size_t argc, char **argv)
 *    (e cmd_led_off, cmd_led_stato). Stampano con shell_print:
 *        "LED acceso"   "LED spento"   "LED: acceso" oppure "LED: spento"
 *    Per lo stato usa gpio_pin_get_dt (restituisce 1 se attivo, 0 se no).
 *    Registra i sottocomandi con SHELL_STATIC_SUBCMD_SET_CREATE e il comando "led" con
 *    SHELL_CMD_REGISTER (esempio nella teoria).
 * 3. Comando "conta" (SHELL_CMD_REGISTER, nessun sottocomando): una variabile static
 *    che parte da 0; a ogni chiamata la incrementa e stampa "conta = N".
 * 4. Comando "somma" con esattamente due argomenti: SHELL_CMD_ARG_REGISTER(somma, NULL,
 *    "Somma due interi", cmd_somma, 3, 0). Converti gli argomenti con strtol:
 *        char *fine;
 *        long a = strtol(argv[1], &fine, 10);
 *        if (*fine != '\0') { ... non era un numero ... }
 *    Se un argomento non e' un numero: shell_error(sh, "argomento non valido: %s", ...)
 *    e return -EINVAL. Se va tutto bene stampa "2 + 3 = 5".
 * 5. Prova con .\esegui.ps1 modulo7\02_comandi_propri e guarda il LED nel monitor di
 *    Renode con: sysbus.gpioPortD.led0 State
 *
 * OUTPUT ATTESO
 *     uart:~$ led on
 *     LED acceso
 *     uart:~$ conta
 *     conta = 1
 *     uart:~$ somma 2 3
 *     2 + 3 = 5
 *     uart:~$ somma 2 x
 *     argomento non valido: x
 *     uart:~$ somma 2
 *     somma: wrong parameter count      (lo scrive la shell da sola)
 *
 * CONCETTI NUOVI
 * - argc/argv: come in C "classico" (e come sys.argv in Python). argv[0] e' il nome.
 * - Le macro SHELL_* registrano i comandi in compilazione: niente da chiamare in main.
 * - strtol e il puntatore "fine": l'unico modo affidabile per sapere se una stringa era
 *   tutta un numero (atoi("12abc") restituisce 12 senza dire niente).
 * - Valore di ritorno: 0 = ok, -EINVAL = argomento sbagliato (codici errno, <errno.h>).
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo7\02_comandi_propri
 * scrive i comandi e controlla risposte e stato del LED.
 *
 * ERRORI TIPICI
 * - Numero di argomenti: per "somma A B" gli obbligatori sono 3, non 2 (conta il nome).
 * - gpio_dt_spec locale a main: i comandi non la vedono (errore di compilazione) o, se
 *   passata in giro per puntatore, punta a memoria che non esiste piu'.
 * - Dimenticare SHELL_SUBCMD_SET_END in fondo alla lista dei sottocomandi.
 * - Usare printk nei comandi: va usato shell_print(sh, ...).
 *
 * BONUS (facoltativo)
 * - "led blink N": lampeggia N volte (argomento numerico, controlla che sia 1-20).
 * - Comando "somma" con un numero qualsiasi di argomenti (obbligatori 2, facoltativi 8).
 * - Prova Tab dopo "led ": la shell propone on, off, stato. Da dove li prende?
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <stdlib.h>
#include <errno.h>
#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(app, LOG_LEVEL_INF);

/* TODO: gpio_dt_spec globale per led0. */

/* TODO: funzioni dei comandi e registrazione con le macro SHELL_*. */

int main(void)
{
	/* TODO: prepara led0. */

	LOG_INF("Comandi pronti: led, conta, somma");
	return 0;
}
