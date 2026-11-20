# 03 — Come si costruisce un'applicazione Zephyr (Modulo 2)

Un'applicazione Zephyr è fatta di tre "ingredienti" che entrano nella compilazione:

| Ingrediente | File | Risponde alla domanda |
|---|---|---|
| **CMake** | `CMakeLists.txt` | *quali sorgenti* compilare e come |
| **Kconfig** | `prj.conf` (+ `Kconfig` se hai opzioni tue) | *quali funzioni del software* attivare (log, shell, dimensioni degli stack...) |
| **Devicetree** | file `.dts` della board + `app.overlay` | *com'è fatto l'hardware*: quali periferiche, a che indirizzo, quali pin |

Regola pratica: **Kconfig = software, devicetree = hardware.**

## 1. Il flusso di `west build`

```
CMakeLists.txt ──► CMake ──┬─► Kconfig:    prj.conf + default della board ─► build/zephyr/.config
                           │                                              └► autoconf.h (#define CONFIG_...)
                           ├─► Devicetree: .dts della board + app.overlay  ─► build/zephyr/zephyr.dts
                           │                                              └► devicetree_generated.h (macro DT_...)
                           └─► Ninja: compila tutti i .c con quegli header ─► build/zephyr/zephyr.elf
```

I file generati in `build/zephyr/` sono la **verità finale**: quando qualcosa non torna,
guarda lì (`.config`, `zephyr.dts`, `zephyr.map`).

Kconfig e devicetree vengono elaborati **solo alla configurazione** (la prima compilazione,
o quando cambiano `prj.conf`, `Kconfig`, `app.overlay`). Se una modifica "non si vede",
ricompila da zero: `.\esegui.ps1 <esercizio> -Pulito`.

## 2. Kconfig e `prj.conf`

Kconfig è un sistema di **opzioni** (lo stesso del kernel Linux). Ogni opzione ha un nome,
un tipo, un valore predefinito e delle dipendenze. In `prj.conf` scrivi solo quelle che vuoi
cambiare rispetto al predefinito:

```
CONFIG_LOG=y              # bool:   y oppure n
CONFIG_MAIN_STACK_SIZE=2048   # int
CONFIG_BOOT_BANNER=n
```

Regole:
- niente spazi attorno a `=`;
- commenti con `#`;
- se un'opzione dipende da un'altra spenta, il tuo valore viene **ignorato** con un
  avviso in compilazione: leggilo.

Nel codice, ogni opzione attiva diventa una macro `CONFIG_...`:

| Tipo | Nel codice |
|---|---|
| bool `y` | `#define CONFIG_LOG 1` (se `n`, la macro **non esiste**) |
| int | `#define CONFIG_MAIN_STACK_SIZE 2048` |
| string | `#define CONFIG_BOARD "nrf52840dk"` |

Per i bool si usa `IS_ENABLED(CONFIG_X)`, che vale 1 o 0 anche se la macro non esiste:

```c
if (IS_ENABLED(CONFIG_BOOT_BANNER)) {
	printk("banner attivo\n");
}
```

Il risultato finale di tutte le opzioni (anche le centinaia predefinite) è in
`build/zephyr/.config`.

## 3. Opzioni Kconfig proprie

Un file `Kconfig` nella cartella dell'applicazione aggiunge opzioni tue:

```kconfig
mainmenu "Il mio esercizio"

config CORSO_NOME
	string "Nome da salutare"
	default "Mondo"
	help
	  Nome usato nel saluto.

config CORSO_RIPETIZIONI
	int "Quante volte salutare"
	default 3
	range 1 10

config CORSO_FORMALE
	bool "Saluto formale"
	default n

source "Kconfig.zephyr"     # OBBLIGATORIO in fondo: carica tutte le opzioni di Zephyr
```

- l'indentazione è con **tab**; il testo di `help` con tab + 2 spazi;
- il nome si scrive **senza** `CONFIG_` nel file `Kconfig`, **con** `CONFIG_` in `prj.conf`
  e nel codice.

Perché non usare un semplice `#define` nel `.c`? Perché Kconfig controlla i valori (`range`),
gestisce le dipendenze e permette di cambiare configurazione senza toccare il codice (per
esempio una configurazione "debug" e una "produzione").

## 4. Logging

`printk` stampa e basta. Il **logging** aggiunge: livello di gravità, nome del modulo, ora,
filtri, colori, e può essere spento in compilazione senza toccare il codice.

```c
#include <zephyr/logging/log.h>
LOG_MODULE_REGISTER(sensore, LOG_LEVEL_DBG);   /* una volta per file .c */

LOG_ERR("lettura fallita: %d", err);   /* errore */
LOG_WRN("temperatura alta: %d", t);    /* avviso */
LOG_INF("avvio completato");           /* informazione */
LOG_DBG("registro 0x%02x", r);         /* debug: dettagli per chi sviluppa */
LOG_HEXDUMP_INF(buf, sizeof(buf), "buffer");
```

Uscita: `[00:00:01.234,000] <inf> sensore: avvio completato`

- il livello in `LOG_MODULE_REGISTER` è il **massimo** che viene compilato: con
  `LOG_LEVEL_INF` le righe `LOG_DBG` spariscono **dal firmware** (non occupano flash);
- niente `\n` alla fine: lo aggiunge il logging;
- serve `CONFIG_LOG=y`;
- di default il logging è *deferred*: il messaggio viene salvato in un buffer e stampato
  poco dopo da un thread apposito, così chi logga non perde tempo. Per questo le righe di
  log possono comparire un attimo dopo una `printk` scritta più tardi.

## 5. Il devicetree: descrivere l'hardware

Il codice di un driver UART è lo stesso per tutte le UART del chip; cambiano indirizzo,
interrupt, pin, velocità. Il **devicetree** è un file di testo che descrive l'hardware della
scheda: **quali periferiche ci sono, dove sono, come sono collegate**.

```dts
/ {
	chosen {
		zephyr,console = &uart0;        /* quale UART usare per printk */
	};
	aliases {
		led0 = &led0;                   /* nomi "generici" usati dagli esempi */
	};
	leds {
		compatible = "gpio-leds";
		led0: led_0 {                   /* etichetta: nome_nodo */
			gpios = <&gpio0 13 GPIO_ACTIVE_LOW>;
			label = "Green LED 0";
		};
	};
	soc {
		uart0: uart@40002000 {          /* @indirizzo */
			compatible = "nordic,nrf-uarte";
			reg = <0x40002000 0x1000>;  /* indirizzo e dimensione dei registri */
			current-speed = <115200>;
			status = "okay";            /* "disabled" = il driver non viene compilato */
		};
	};
};
```

Termini:
- **nodo**: un elemento (`uart@40002000`), con figli tra `{ }`;
- **proprietà**: `nome = valore;` — numeri tra `< >`, stringhe tra `" "`;
- **etichetta** (*node label*): `uart0:` prima del nodo, per riferirsi a lui con `&uart0`;
- **alias** e **chosen**: nomi di comodo che puntano a un nodo;
- **compatible**: dice quale **driver** e quale *binding* (lo schema delle proprietà
  ammesse, file `.yaml` in `zephyr/dts/bindings/`) si applicano al nodo;
- **status**: solo i nodi `okay` diventano dispositivi utilizzabili.

Il devicetree finale, dopo aver unito i file della board e i tuoi overlay, è in
`build/zephyr/zephyr.dts`. Guardalo: è la mappa dell'hardware vista da Zephyr. Gli indirizzi
`reg` sono gli stessi che Renode mostra con il comando `peripherals`.

## 6. Leggere il devicetree dal codice

Il devicetree non esiste nel firmware come testo: in compilazione diventa una montagna di
macro C. Si accede con le macro `DT_...`, che restituiscono costanti **a tempo di
compilazione** (costo zero a runtime):

| Macro | Esempio | Risultato |
|---|---|---|
| `DT_NODELABEL(etichetta)` | `DT_NODELABEL(uart0)` | il nodo con etichetta `uart0` |
| `DT_ALIAS(alias)` | `DT_ALIAS(led0)` | il nodo puntato dall'alias |
| `DT_CHOSEN(nome)` | `DT_CHOSEN(zephyr_console)` | il nodo scelto (`,` e `-` diventano `_`) |
| `DT_PATH(a, b)` | `DT_PATH(zephyr_user)` | il nodo per percorso `/zephyr,user` |
| `DT_PROP(nodo, prop)` | `DT_PROP(n, current_speed)` | valore della proprietà (`-` diventa `_`) |
| `DT_REG_ADDR(nodo)` | | primo indirizzo di `reg` |
| `DT_REG_SIZE(nodo)` | | prima dimensione di `reg` |
| `DT_GPIO_PIN(nodo, gpios)` | | numero di pin |
| `DT_NODE_HAS_STATUS(nodo, okay)` | | 1 se il nodo è attivo |

Attenzione ai nomi: nel devicetree si scrive `current-speed`, `zephyr,console`; nelle macro
`current_speed`, `zephyr_console` (minuscolo, con `_`).

Se sbagli un nome, l'errore di compilazione è lunghissimo e parla di `DT_N_S_..._P_...`:
cerca nel messaggio il nome della proprietà o del nodo e confrontalo con `zephyr.dts`.

## 7. GPIO: il primo LED

**GPIO** (*General Purpose Input/Output*): pin del chip che il software può portare a 0 o 1
(uscita) o leggere (ingresso). Nell'nRF52840 la porta `gpio0` ha 32 pin, comandati da
registri (`OUT`, `OUTSET`, `OUTCLR`, `DIR`, `IN`...) a partire da `0x50000500`.

Zephyr nasconde i registri dietro un'API uguale per tutti i chip. La struttura
`gpio_dt_spec` raccoglie in un colpo dal devicetree: dispositivo della porta, numero di pin e
flag (es. *attivo basso*):

```c
#include <zephyr/drivers/gpio.h>

static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

if (!gpio_is_ready_dt(&led)) {                    /* il driver si e' avviato? */
	return 0;
}
gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE); /* uscita, inizialmente spento */
gpio_pin_set_dt(&led, 1);                          /* 1 = ATTIVO (acceso) */
gpio_pin_toggle_dt(&led);                          /* inverte */
```

**Attivo basso** (`GPIO_ACTIVE_LOW`): sulla DK il LED si accende quando il pin è a **0**
(il LED è collegato tra l'alimentazione e il pin). Con le funzioni `_dt` lavori in valori
*logici* (1 = acceso): l'inversione la fa Zephyr leggendo il flag dal devicetree.

Le funzioni restituiscono un `int`: 0 = tutto bene, negativo = codice di errore (es.
`-EINVAL`, `-ENODEV`). È la convenzione di tutto Zephyr.

## 8. Overlay: modificare il devicetree senza toccare la board

Il file `app.overlay` nella cartella dell'applicazione viene **aggiunto** al devicetree della
board. Serve per: attivare/disattivare periferiche, aggiungere dispositivi esterni (un
sensore su I2C), cambiare proprietà, aggiungere alias.

```dts
/ {
	aliases {
		allarme = &led2;            /* nuovo alias: nel codice DT_ALIAS(allarme) */
	};

	zephyr,user {                   /* nodo "libero" per i parametri della tua app */
		soglia = <30>;
		nome-nodo = "nodo-01";
	};
};

&uart0 {                            /* modificare un nodo esistente tramite etichetta */
	current-speed = <9600>;
};
```

Il nodo `zephyr,user` accetta proprietà qualsiasi: è il posto giusto per i parametri
"hardware" della tua applicazione (quale pin, quale soglia di fabbrica).

## 9. menuconfig e `__ASSERT`

**menuconfig** è un'interfaccia a menu per esplorare e cambiare le opzioni Kconfig:

```powershell
west build -t menuconfig -d modulo2\07_menuconfig_assert\build
```

(alternativa grafica: `-t guiconfig`). Si naviga con le frecce, `/` cerca un'opzione,
`?` mostra l'aiuto con dipendenze e posizione del file Kconfig. Le modifiche fatte lì
valgono **solo per quella cartella build**: al prossimo `-Pulito` si perdono. Quando hai
trovato l'opzione giusta, scrivila in `prj.conf`.

**Asserzioni**: `__ASSERT(condizione, "messaggio", ...)` controlla una condizione che *deve*
essere vera; se è falsa stampa file, riga e messaggio e ferma il sistema (errore fatale).
Si attivano con `CONFIG_ASSERT=y`: in produzione si possono spegnere e il codice dei
controlli sparisce. Servono a scoprire presto gli errori di programmazione, non a gestire
errori "normali" (un sensore che non risponde si gestisce con il codice di ritorno).

## 10. Riassunto: dove si mette cosa

| Voglio... | Dove |
|---|---|
| aggiungere un file `.c` | `CMakeLists.txt` → `target_sources(app PRIVATE src/nuovo.c)` |
| attivare log, shell, assert... | `prj.conf` |
| un parametro mio del software (periodo, nome) | `Kconfig` + `prj.conf` |
| dire su quale pin è un LED, abilitare una periferica, aggiungere un sensore | `app.overlay` |
| sapere che valore ha preso un'opzione | `build/zephyr/.config` |
| sapere com'è fatto l'hardware | `build/zephyr/zephyr.dts` |
| sapere dove è finito un simbolo in memoria | `build/zephyr/zephyr.map` |
