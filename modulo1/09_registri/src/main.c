/*
 * Esercizio 9 (Modulo 1) - volatile e registri: accendere un LED "a mano".
 *
 * Teoria: teoria/02_c_embedded.md, sezioni 3, 10 e 11.
 *
 * OBIETTIVO
 * Toccare con mano come si comanda una periferica: scrivendo in REGISTRI, cioe'
 * indirizzi di memoria speciali. Leggerai l'identificativo del chip e accenderai il
 * LED blu della scheda (pin PD15, led3 in Zephyr e in Renode) scrivendo direttamente
 * nei registri, senza il driver GPIO di Zephyr.
 *
 * I REGISTRI (dal reference manual dell'STM32F407, RM0090)
 *     DBGMCU_IDCODE   0xE0042000   identificativo del chip (sola lettura):
 *                                  bit 11..0 = DEV_ID, bit 31..16 = REV_ID
 *     RCC_AHB1ENR     0x40023830   abilitazione dei clock delle periferiche AHB1:
 *                                  bit 3 = GPIODEN (clock della porta GPIOD)
 *     GPIOD_MODER     0x40020C00   modo di ogni pin, 2 bit per pin:
 *                                  00 ingresso, 01 uscita, 10 funzione alternativa,
 *                                  11 analogico. Il pin 15 usa i bit 31..30.
 *     GPIOD_IDR       0x40020C10   livello letto sui pin, un bit per pin
 *     GPIOD_ODR       0x40020C14   livello delle uscite, un bit per pin
 *     GPIOD_BSRR      0x40020C18   scrivi 1 nel bit n      -> pin n a 1 (Set)
 *                                  scrivi 1 nel bit n + 16 -> pin n a 0 (Reset)
 * I LED della Discovery sono ATTIVI ALTI: il LED si accende quando il pin e' a 1.
 *
 * PASSI
 * 1. Completa la macro REG32 qui sotto e le costanti degli indirizzi.
 *    Nel codice i nomi hanno il suffisso _ADDR (es. GPIOD_BSRR_ADDR): vedi il commento
 *    sopra le #define.
 * 2. Leggi DBGMCU_IDCODE e stampa
 *        IDCODE = 0x%08x
 *        DEV_ID = 0x%03x  REV_ID = 0x%04x
 *    estraendo i campi con maschera e scorrimento (Modulo 1, esercizio 2).
 *    Sullo STM32F407 vero DEV_ID vale 0x413; Renode simula una famiglia vicina e da'
 *    0x411. Annota cosa vedi.
 * 3. Accendi il clock di GPIOD: imposta il bit 3 di RCC_AHB1ENR con |= (qui serve
 *    leggi-modifica-scrivi: gli altri bit accendono altre periferiche e non vanno toccati).
 *    Stampa "Clock GPIOD acceso: %u" con il valore del bit 3 riletto (deve essere 1).
 * 4. Metti il pin 15 in uscita: in GPIOD_MODER azzera i bit 31..30 e poi scrivi 01.
 *        REG32(GPIOD_MODER_ADDR) = (REG32(GPIOD_MODER_ADDR) & ~(3UL << 30)) | (1UL << 30);
 *    Stampa "MODER pin 15 = %u" con i due bit riletti (deve essere 1).
 * 5. Accendi il LED scrivendo BIT(15) in GPIOD_BSRR e stampa "LED acceso".
 *    Rileggi GPIOD_ODR e stampa "ODR bit 15 = %u" con il valore del bit 15 (0 o 1).
 * 6. k_msleep(500); spegni il LED scrivendo BIT(15 + 16) in GPIOD_BSRR, stampa
 *    "LED spento" e di nuovo "ODR bit 15 = %u".
 * 7. k_msleep(500); riaccendilo e stampa "LED acceso di nuovo". Il programma finisce con
 *    il LED acceso (la verifica controlla proprio questo nel simulatore).
 * 8. Con  .\esegui.ps1  scrivi nel monitor di Renode, prima e dopo l'accensione,
 *        sysbus.gpioPortD.led3 State
 *    e anche  sysbus ReadDoubleWord 0x40020C14  : e' lo stesso registro ODR che leggi
 *    dal C!
 *
 * OUTPUT ATTESO
 *     IDCODE = 0x10006411
 *     DEV_ID = 0x411  REV_ID = 0x1000
 *     Clock GPIOD acceso: 1
 *     MODER pin 15 = 1
 *     LED acceso
 *     ODR bit 15 = 1
 *     LED spento
 *     ODR bit 15 = 0
 *     LED acceso di nuovo
 *
 * CONCETTI NUOVI
 * - Memory-mapped I/O: le periferiche si leggono e scrivono come memoria.
 * - (volatile uint32_t *)0x40020C18: trasformare un numero in un puntatore a registro.
 * - volatile: obbliga il compilatore a fare davvero ogni lettura e scrittura.
 * - Clock gating: sugli STM32 ogni periferica ha un "interruttore" del clock in RCC.
 *   Senza clock la periferica e' spenta: le scritture nei suoi registri non hanno
 *   effetto. Per questo il primo passo e' sempre accendere il clock.
 *   (In questo firmware Zephyr l'ha gia' acceso all'avvio per i suoi driver GPIO, e
 *   Renode non simula il clock gating: lo fai lo stesso, come sulla scheda vera.)
 * - Campi di piu' bit (MODER): azzera con & ~maschera, poi imposta con |.
 * - Registro BSRR: scrivi solo i bit che vuoi cambiare, senza leggere prima (atomico:
 *   nessun interrupt puo' "rompere" la modifica a meta').
 * - Suffisso UL (0x40020C00UL): costante unsigned long.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\09_registri
 * Oltre all'output, la verifica chiede a Renode se il LED led3 e' acceso alla fine.
 *
 * ERRORI TIPICI
 * - Dimenticare volatile: con l'ottimizzazione, il compilatore puo' fondere o eliminare
 *   le scritture. Qui forse funziona lo stesso: "forse" non basta in un firmware.
 * - Scrivere  REG32(GPIOD_BSRR_ADDR) |= BIT(15);  : su BSRR si scrive e basta (=). |= fa
 *   prima una lettura inutile (BSRR letto vale sempre 0).
 * - Scrivere  REG32(GPIOD_MODER_ADDR) = 1UL << 30;  : funziona per il pin 15 ma azzera il
 *   modo di TUTTI gli altri pin della porta.
 * - Spegnere con BIT(15) invece di BIT(15 + 16): il LED resta acceso.
 * - Sbagliare l'indirizzo di una cifra: in Renode vedrai un avviso (con logLevel 2) di
 *   accesso a un registro non modellato; su una scheda vera, un comportamento strano o
 *   un fault.
 *
 * BONUS (facoltativo)
 * - Fai lampeggiare i 4 LED (PD12 verde, PD13 arancione, PD14 rosso, PD15 blu) in
 *   sequenza, "a mano", in un ciclo. Ricorda MODER per ciascun pin.
 * - Il pulsante utente e' su PA0 (porta GPIOA, base 0x40020000): GPIOA_IDR e' a
 *   0x40020010 e il bit 0 vale 1 quando il pulsante e' premuto (attivo alto). PA0 e'
 *   gia' un ingresso dopo il reset (MODER = 00). Leggilo in un ciclo e stampa quando
 *   cambia; premilo da Renode con  sysbus.gpioPortA.sw0 Press  e  Release.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>   /* BIT */
#include <stdint.h>

/* TODO passo 1: completa la macro. Deve trasformare un indirizzo in un registro a 32 bit
 * leggibile e scrivibile:  (*(volatile uint32_t *)(indirizzo))  */
#define REG32(indirizzo) /* TODO */

/* TODO passo 1: indirizzi dei registri (completa).
 * Il suffisso _ADDR non e' un vezzo: gli header di ST, che Zephyr include per questo chip,
 * definiscono gia' nomi come GPIOD_BASE. Riusarli darebbe l'avviso "redefined". */
#define DBGMCU_IDCODE_ADDR   0xE0042000UL
#define RCC_AHB1ENR_ADDR     /* TODO */
#define GPIOD_BASE_ADDR      0x40020C00UL
#define GPIOD_MODER_ADDR     (GPIOD_BASE_ADDR + 0x00)
#define GPIOD_IDR_ADDR       /* TODO */
#define GPIOD_ODR_ADDR       /* TODO */
#define GPIOD_BSRR_ADDR      /* TODO */

#define LED_BLU_PIN 15

int main(void)
{
	/* TODO passi 2-7 */

	return 0;
}
