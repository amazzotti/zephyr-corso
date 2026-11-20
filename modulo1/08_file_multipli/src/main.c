/*
 * Esercizio 8 (Modulo 1) - Un programma su piu' file.
 *
 * Teoria: teoria/02_c_embedded.md, sezioni 8 e 9.
 *
 * OBIETTIVO
 * Dividere il codice in un modulo riutilizzabile (statistiche.h + statistiche.c) e
 * usarlo da main.c, come si fa in ogni progetto vero. E capire la differenza tra errori
 * del compilatore e errori del linker.
 *
 * PASSI
 * 1. Leggi statistiche.h (gia' completo): e' l'"interfaccia" del modulo.
 *    Nota l'header guard (#ifndef / #define / #endif) e la parola extern.
 * 2. In statistiche.c definisci  int stat_chiamate = 0;
 * 3. In statistiche.c scrivi la funzione privata  static void registra_chiamata(void)
 *    e completa le quattro funzioni pubbliche (ognuna chiama registra_chiamata()).
 * 4. In main.c, sui dati
 *        static const int32_t dati[] = {12, -3, 45, 7, 0, 22};
 *    chiama le quattro funzioni e stampa i risultati e poi stat_chiamate.
 *    La media va stampata in virgola fissa (138 -> "13.8").
 *    COMPILA SUBITO, PRIMA di modificare CMakeLists.txt: otterrai l'errore
 *        undefined reference to `stat_somma'
 *    Non e' un errore del compilatore (main.c compila!) ma del LINKER: la funzione e'
 *    dichiarata, ma nessun file compilato la definisce.
 * 5. Ora aggiungi src/statistiche.c in CMakeLists.txt (riga target_sources) e ricompila
 *    con  .\esegui.ps1 modulo1\08_file_multipli -Pulito
 *
 * OUTPUT ATTESO
 *     somma = 83
 *     min = -3
 *     max = 45
 *     media = 13.8
 *     chiamate = 4
 *
 * CONCETTI NUOVI
 * - Header (.h): dichiarazioni. Sorgente (.c): definizioni.
 * - #include "file.h" (file del progetto) contro #include <file.h> (librerie).
 * - Header guard: evita doppie inclusioni.
 * - extern: "questa variabile e' definita altrove".
 * - static su una funzione: visibile solo nel suo file.
 * - Compilazione separata: ogni .c -> .o; poi il linker unisce i .o.
 * - target_sources in CMakeLists.txt: l'elenco dei file da compilare.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\08_file_multipli
 *
 * ERRORI TIPICI
 * - Definire la variabile nell'header (int stat_chiamate = 0; in statistiche.h): se due
 *   .c lo includono, il linker trova DUE definizioni ("multiple definition").
 * - Dimenticare l'header guard: errori di "redefinition" quando un header e' incluso
 *   due volte (per esempio tramite un altro header).
 * - Chiamare registra_chiamata() da main.c: errore, e' static (privata di statistiche.c).
 * - Firma diversa tra .h e .c (es. int invece di int32_t): "conflicting types".
 *
 * BONUS (facoltativo)
 * - Aggiungi al modulo  int32_t stat_conta_sopra(const int32_t *v, size_t n, int32_t soglia)
 *   (quanti elementi superano la soglia): prima la dichiarazione nell'.h, poi il codice.
 * - Guarda in build/zephyr/zephyr.map dove sono finite stat_somma e registra_chiamata.
 *   (Se registra_chiamata non c'e', il compilatore l'ha "inlinata": copiata dentro chi
 *   la chiama.)
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>
#include "statistiche.h"

int main(void)
{
	/* TODO passo 4 */

	return 0;
}
