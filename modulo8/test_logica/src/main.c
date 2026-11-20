/*
 * Progetto finale (Modulo 8), passo 7 - Test della logica
 *
 * Teoria: teoria/08_strumenti_debug_test.md, sezione 3; teoria/09_progettazione.md,
 * sezioni 6 e 9. Consegna completa: modulo8/07_test_relazione.md, parte A.
 *
 * OBIETTIVO
 * Testare con ztest i file "puri" del progetto, compilati direttamente da
 * modulo8/progetto/src: filtro.c, allarme.c e (se c'e') formato.c.
 *
 * PASSI
 * 1. Esegui lo scheletro: .\verifica.ps1 modulo8\test_logica
 *    Passa un solo test (test_sanita). Il CMakeLists aggiunge filtro.c, allarme.c e
 *    formato.c solo se esistono, e definisce HA_FILTRO, HA_ALLARME, HA_FORMATO.
 * 2. Dentro i blocchi #ifdef qui sotto scrivi le suite "filtro", "allarme", "formato"
 *    con i test elencati nella tabella della consegna (almeno 5 + 6 + 6).
 *    Tutte le temperature in centesimi: 30.00 C = 3000.
 * 3. Per ogni test scrivi PRIMA cosa ti aspetti (a mano, nel nome o in un commento), POI
 *    il codice. Se un test fallisce, chiediti chi sbaglia: il test o il codice?
 *
 * OUTPUT ATTESO
 *     SUITE PASS - 100.00% [filtro]: ...
 *     SUITE PASS - 100.00% [allarme]: ...
 *     PROJECT EXECUTION SUCCESSFUL
 *
 * CONCETTI NUOVI
 * - Testare la logica separata dall'hardware: gli stessi file girano nel firmware e qui.
 * - Casi limite: esattamente sulla soglia, esattamente su soglia - isteresi, negativi,
 *   buffer del filtro che "gira".
 *
 * COME VERIFICARE DA SOLO
 *     .\verifica.ps1 modulo8\test_logica
 *
 * ERRORI TIPICI
 * - Test che dipendono l'uno dall'altro (struct globale non reinizializzata).
 * - Media "a mano" sbagliata: con interi la divisione tronca (-7 / 2 = -3, non -4).
 * - Dimenticare che il CMakeLists vede i nuovi file solo alla compilazione successiva:
 *   se hai appena creato formato.c e HA_FORMATO non risulta, usa -Pulito.
 *
 * BONUS (facoltativo)
 * - Un test "sequenza": 20 temperature realistiche (salita, picco, discesa) passate
 *   prima al filtro e poi all'allarme; controlla gli eventi nell'ordine giusto.
 *
 * Scrivi il tuo codice qui sotto.
 */

#include <zephyr/ztest.h>

ZTEST(sanita, test_sanita)
{
	zassert_equal(30 * 100, 3000, "30.00 C in centesimi deve valere 3000");
}

ZTEST_SUITE(sanita, NULL, NULL, NULL, NULL, NULL);

#ifdef HA_FILTRO
#include "filtro.h"

/* TODO: suite "filtro". */

#endif

#ifdef HA_ALLARME
#include "allarme.h"

/* TODO: suite "allarme". */

#endif

#ifdef HA_FORMATO
#include "formato.h"

/* TODO: suite "formato". */

#endif
