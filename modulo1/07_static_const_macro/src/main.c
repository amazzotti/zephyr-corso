/*
 * Esercizio 7 (Modulo 1) - static, const e macro.
 *
 * Teoria: teoria/02_c_embedded.md, sezione 8.
 *
 * OBIETTIVO
 * Capire tre strumenti usatissimi nel codice embedded (e in tutto Zephyr): variabili
 * static che "ricordano" il valore, costanti const, e macro del preprocessore con le
 * loro trappole.
 *
 * PASSI
 * 1. Scrivi  void conta_chiamate(void)  con dentro
 *        static int contatore = 0;
 *        contatore++;
 *        printk("chiamata %d\n", contatore);
 *    Chiamala 3 volte.
 * 2. Scrivi  void conta_sbagliato(void)  identica ma SENZA static (stampa "sbagliato %d").
 *    Chiamala 3 volte. Perche' stampa sempre 1?
 * 3. Definisci fuori da main la macro SBAGLIATA
 *        #define QUADRATO(x) x * x
 *    e quella corretta
 *        #define QUADRATO_OK(x) ((x) * (x))
 *    Stampa "QUADRATO(2 + 3) = %d" e "QUADRATO_OK(2 + 3) = %d" passando proprio 2 + 3.
 *    Scrivi su carta in cosa si trasforma QUADRATO(2 + 3) dopo la sostituzione.
 * 4. Dichiara  const int massimo = 10;  e prova a scrivere  massimo = 20;
 *    Compila, leggi l'errore ("assignment of read-only variable"), poi togli la riga.
 * 5. Dichiara  static const char *const colori[] = {"rosso", "verde", "giallo", "blu"};
 *    e stampa "colori: 4" usando ARRAY_SIZE.
 * 6. Usa le macro di Zephyr MIN e CLAMP e stampa
 *        MIN(3, 7) = 3
 *        CLAMP(15, 0, 10) = 10
 *
 * OUTPUT ATTESO
 *     chiamata 1
 *     chiamata 2
 *     chiamata 3
 *     sbagliato 1
 *     sbagliato 1
 *     sbagliato 1
 *     QUADRATO(2 + 3) = 11
 *     QUADRATO_OK(2 + 3) = 25
 *     colori: 4
 *     MIN(3, 7) = 3
 *     CLAMP(15, 0, 10) = 10
 *
 * CONCETTI NUOVI
 * - static locale: vive per tutto il programma (non sullo stack), inizializzata una volta.
 * - static globale: visibile solo in questo file .c ("privata").
 * - const: sola lettura, controllato dal compilatore. Le const globali stanno in flash.
 * - const char *const v[]: array di puntatori costanti a stringhe costanti (si legge
 *   da destra a sinistra).
 * - #define: sostituzione di TESTO fatta dal preprocessore prima della compilazione.
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo1\07_static_const_macro
 *
 * ERRORI TIPICI
 * - Mettere il ; alla fine di un #define:  #define N 10;  poi  int v[N];  diventa
 *   int v[10;];  ed esce un errore incomprensibile.
 * - Macro con argomenti con effetti collaterali: QUADRATO_OK(i++) incrementa i DUE
 *   volte (comportamento indefinito). Mai farlo.
 * - Credere che una variabile static locale si riazzeri a ogni chiamata.
 *
 * BONUS (facoltativo)
 * - Guarda cosa produce il preprocessore: aggiungi in fondo a CMakeLists.txt
 *       target_compile_options(app PRIVATE -save-temps=obj)
 *   ricompila con -Pulito e apri build/CMakeFiles/app.dir/src/main.c.i: e' il tuo
 *   codice dopo le sostituzioni (in fondo al file, dopo migliaia di righe di header).
 * - Definisci  #define STR(x) #x  e stampa STR(QUADRATO(2 + 3)): cosa vedi?
 * - Perche' MIN e' una macro e non una funzione? (pensa ai tipi: int, uint8_t, ...)
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>   /* ARRAY_SIZE, MIN, MAX, CLAMP */

/* TODO passo 3: le due macro QUADRATO e QUADRATO_OK */

/* TODO passi 1-2: conta_chiamate e conta_sbagliato */

int main(void)
{
	/* TODO passi 1-6 */

	return 0;
}
