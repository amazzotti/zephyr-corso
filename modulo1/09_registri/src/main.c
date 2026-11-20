/*
 * Esercizio 9 (Modulo 1) - volatile e registri: accendere un LED "a mano".
 *
 * Teoria: teoria/02_c_embedded.md, sezioni 3, 10 e 11.
 *
 * OBIETTIVO
 * Toccare con mano come si comanda una periferica: scrivendo in REGISTRI, cioe'
 * indirizzi di memoria speciali. Leggerai l'identificativo del chip e accenderai il
 * LED 1 della scheda (pin P0.13) scrivendo direttamente nei registri GPIO, senza driver.
 *
 * I REGISTRI (dal manuale del nRF52840)
 *     FICR->DEVICEID[0]   0x10000060   identificativo del chip, parte bassa (sola lettura)
 *     FICR->DEVICEID[1]   0x10000064   identificativo del chip, parte alta
 *     P0.OUT              0x50000504   stato delle uscite, un bit per pin
 *     P0.OUTSET           0x50000508   scrivi 1 nel bit n -> pin n a livello alto (1)
 *     P0.OUTCLR           0x5000050C   scrivi 1 nel bit n -> pin n a livello basso (0)
 *     P0.DIRSET           0x50000518   scrivi 1 nel bit n -> pin n diventa un'uscita
 * Il LED 1 (led0 in Zephyr e in Renode) e' sul pin 13 ed e' ATTIVO BASSO: si accende
 * quando il pin e' a 0.
 *
 * PASSI
 * 1. Completa la macro REG32 qui sotto e le costanti degli indirizzi.
 * 2. Leggi i due registri DEVICEID e stampa
 *        DEVICEID = 0x%08x%08x        (prima la parte alta, poi la bassa)
 *    Su una scheda vera e' un numero unico per ogni chip. In Renode potrebbe essere 0
 *    o un valore fisso: osserva e annota cosa vedi.
 * 3. Configura il pin 13 come uscita, MA prima portalo a 1 (LED spento) con OUTSET:
 *    al reset il registro OUT vale 0, e un LED attivo basso si accenderebbe nell'istante
 *    in cui il pin diventa uscita. Ordine giusto: OUTSET, poi DIRSET.
 * 4. Accendi il LED (OUTCLR, bit 13) e stampa "LED acceso".
 *    Rileggi P0.OUT e stampa "OUT bit 13 = %u" con il valore del bit 13 (0 o 1).
 * 5. k_msleep(500); spegni il LED (OUTSET), stampa "LED spento" e di nuovo
 *    "OUT bit 13 = %u".
 * 6. k_msleep(500); riaccendilo e stampa "LED acceso di nuovo". Il programma finisce con
 *    il LED acceso (la verifica controlla proprio questo nel simulatore).
 * 7. Con  .\esegui.ps1  scrivi nel monitor di Renode, prima e dopo l'accensione,
 *        sysbus.gpio0.led0 State
 *    e anche  sysbus ReadDoubleWord 0x50000504  : e' lo stesso registro che leggi dal C!
 *
 * OUTPUT ATTESO
 *     DEVICEID = 0x................     (16 cifre esadecimali, valore qualsiasi)
 *     LED acceso
 *     OUT bit 13 = 0
 *     LED spento
 *     OUT bit 13 = 1
 *     LED acceso di nuovo
 *
 * CONCETTI NUOVI
 * - Memory-mapped I/O: le periferiche si leggono e scrivono come memoria.
 * - (volatile uint32_t *)0x50000508: trasformare un numero in un puntatore a registro.
 * - volatile: obbliga il compilatore a fare davvero ogni lettura e scrittura.
 * - Registri SET/CLR: modificano solo i bit scritti a 1, senza leggi-modifica-scrivi.
 * - Attivo basso: "acceso" = livello logico 0.
 * - Suffisso UL (0x50000000UL): costante unsigned long.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\09_registri
 * Oltre all'output, la verifica chiede a Renode se il LED led0 e' acceso alla fine.
 *
 * ERRORI TIPICI
 * - Dimenticare volatile: con l'ottimizzazione, il compilatore puo' fondere o eliminare
 *   le scritture. Qui forse funziona lo stesso: "forse" non basta in un firmware.
 * - Scrivere  REG32(OUTSET) |= BIT(13);  : su un registro SET si scrive e basta (=).
 *   |= fa prima una lettura, inutile e su alcuni registri anche dannosa.
 * - Sbagliare l'indirizzo di una cifra: in Renode vedrai un avviso (con logLevel 2) di
 *   accesso a un registro non modellato; su una scheda vera, un comportamento strano o
 *   un fault.
 * - Confondere "LED acceso" con "pin a 1": su questa scheda e' il contrario.
 *
 * BONUS (facoltativo)
 * - Fai lampeggiare i 4 LED (pin 13, 14, 15, 16) in sequenza, "a mano", in un ciclo.
 * - Il registro P0.IN (0x50000510) contiene lo stato dei pin di ingresso: il pulsante 1
 *   e' sul pin 11. Leggilo in un ciclo e stampa quando cambia; premilo da Renode con
 *   sysbus.gpio0.sw0 Press  e  Release. (Il pin va prima configurato come ingresso con
 *   il buffer abilitato: registro PIN_CNF[11] a 0x5000072C, valore 0x0000000C =
 *   ingresso con pull-up. Cerca PIN_CNF nel manuale per capire i bit.)
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

/* TODO passo 1: indirizzi dei registri (completa) */
#define FICR_DEVICEID0  0x10000060UL
#define FICR_DEVICEID1  /* TODO */
#define GPIO_P0_BASE    0x50000000UL
#define GPIO_P0_OUT     (GPIO_P0_BASE + 0x504)
#define GPIO_P0_OUTSET  /* TODO */
#define GPIO_P0_OUTCLR  /* TODO */
#define GPIO_P0_DIRSET  /* TODO */

#define LED1_PIN 13

int main(void)
{
	/* TODO passi 2-6 */

	return 0;
}
