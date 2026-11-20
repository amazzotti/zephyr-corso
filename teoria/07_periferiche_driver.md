# 07 — Periferiche e driver (Modulo 6)

## 1. Dai registri ai driver

Nel Modulo 1 hai acceso un LED scrivendo direttamente nel registro `OUTCLR` del GPIO.
Funziona, ma:
- il codice vale solo per quel chip (su un STM32 i registri sono completamente diversi);
- devi conoscere a memoria indirizzi e significato di ogni bit;
- se due parti del programma usano la stessa periferica, nessuno coordina gli accessi.

Un **driver** è il codice che conosce i registri di una periferica e offre in cambio
**funzioni generiche** (`gpio_pin_set`, `uart_poll_out`, `i2c_write_read`...). Il tuo codice
chiama le funzioni; il driver le traduce in letture e scritture di registri.

```
  tuo codice      gpio_pin_set_dt(&led, 1)          (uguale su ogni chip)
       |
  API Zephyr      include/zephyr/drivers/gpio.h     (interfaccia comune)
       |
  driver          drivers/gpio/gpio_nrfx.c          (specifico del nRF52)
       |
  registri        GPIO->OUTCLR = (1 << 13)          (hardware)
```

## 2. Il modello dei driver di Zephyr

Ogni periferica è rappresentata da una **`struct device`**, creata in compilazione a partire
dal **devicetree**: un nodo con `status = "okay"` e un driver compatibile diventa un device.

```c
const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
const struct device *i2c  = DEVICE_DT_GET(DT_NODELABEL(i2c0));
```

- `DEVICE_DT_GET(nodo)` restituisce il puntatore al device, deciso in compilazione: non può
  fallire a runtime, ma se il nodo non esiste o è disabilitato la compilazione si ferma
  (errore con `__device_dts_ord_...`: è il segno che manca qualcosa nel devicetree o in
  `prj.conf`).
- **`device_is_ready(dev)`**: il driver è stato inizializzato bene? Va **sempre** controllato
  prima dell'uso. Le varianti `_dt` (`gpio_is_ready_dt`, `i2c_is_ready_dt`) fanno lo stesso.
- **Inizializzazione**: all'avvio Zephyr chiama la funzione di init di ogni driver, in ordine
  di livello (`PRE_KERNEL_1`, `PRE_KERNEL_2`, `POST_KERNEL`, `APPLICATION`) e di priorità.
  Per questo in `main()` trovi già tutto pronto.
- **Kconfig attiva il codice del driver**: `CONFIG_GPIO=y`, `CONFIG_I2C=y`, `CONFIG_SENSOR=y`...
  Senza l'opzione il driver non viene compilato e il device non esiste.
- Le funzioni dell'API restituiscono un `int`: **0 = tutto bene, negativo = errore** (un codice
  `errno` come `-EIO`, errore di I/O, o `-ENODEV`). Controllalo sempre.

Le strutture `_dt_spec` raccolgono in un unico oggetto tutto ciò che il devicetree dice di un
collegamento:

```c
/* GPIO: quale porta, quale pin, quali flag (es. attivo basso) */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
/* I2C: quale bus e quale indirizzo */
static const struct i2c_dt_spec sens = I2C_DT_SPEC_GET(DT_NODELABEL(bme280));
```

## 3. GPIO

**GPIO** (*General Purpose Input/Output*) = un pin che il programma può usare come uscita
(accendere un LED) o come ingresso (leggere un pulsante).

- **Uscita**: il pin è collegato a 3,3 V (livello 1, "alto") o a massa (livello 0, "basso").
- **Ingresso**: il programma legge se il pin è alto o basso. Un ingresso non collegato a
  nulla "fluttua" e legge valori casuali: per questo si usa una **resistenza di pull-up**
  (porta il pin a 1 quando nessuno lo tira giù) o di **pull-down**.
- **Attivo basso**: sulla nRF52840 DK il pulsante collega il pin a massa. A riposo il pull-up
  tiene il pin a 1, premuto vale 0. I LED si accendono quando il pin è a 0.

Il devicetree descrive tutto questo con i flag (`GPIO_ACTIVE_LOW | GPIO_PULL_UP`), e le
funzioni `_dt` lavorano in **logica**: `gpio_pin_get_dt(&btn)` restituisce **1 = premuto**,
anche se il pin è elettricamente a 0. Non devi invertire niente a mano.

```c
gpio_pin_configure_dt(&btn, GPIO_INPUT);          /* pull-up gia' nei flag del DT */
gpio_pin_configure_dt(&led, GPIO_OUTPUT_INACTIVE); /* uscita, parte spento */
int premuto = gpio_pin_get_dt(&btn);              /* 1, 0 oppure <0 se errore */
gpio_pin_set_dt(&led, premuto);
```

**Polling**: leggere l'ingresso in un ciclo, ogni tot millisecondi. Semplice, ma consuma
CPU e può perdere eventi più brevi del periodo di lettura.

## 4. Interrupt

Un **interrupt** è un segnale con cui una periferica dice alla CPU "è successo qualcosa,
occupatene ora". La CPU sospende ciò che sta facendo, esegue una funzione breve,
la **ISR** (*Interrupt Service Routine*), poi riprende da dove era.

Sul Cortex-M c'è il **NVIC** (*Nested Vectored Interrupt Controller*): ogni periferica ha un
numero di interrupt e una priorità. Sul nRF52 gli interrupt dei pin passano per il **GPIOTE**
(*GPIO Tasks and Events*), che rileva i fronti.

- **Fronte**: il passaggio da 0 a 1 (*salita*) o da 1 a 0 (*discesa*). Con
  `GPIO_INT_EDGE_TO_ACTIVE` l'interrupt scatta quando il pin diventa *attivo* (premuto),
  qualunque sia il livello elettrico.
- In Zephyr non scrivi la ISR del GPIO: la scrive il driver, e lui chiama la **tua callback**
  registrata con `gpio_init_callback` + `gpio_add_callback`.

```c
static struct gpio_callback cb_data;

static void pulsante_premuto(const struct device *dev, struct gpio_callback *cb,
			     uint32_t pins)
{
	/* contesto di interrupt: breve, niente attese */
}

gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
gpio_init_callback(&cb_data, pulsante_premuto, BIT(btn.pin));
gpio_add_callback(btn.port, &cb_data);
```

Regole della callback (le stesse del Modulo 5 per il timer): niente `k_msleep`, niente
`k_mutex_lock`, niente attese; `printk` è permesso ma lento. Si fa il minimo (contare,
`k_sem_give`, `k_work_submit`, `k_msgq_put` con `K_NO_WAIT`) e il resto lo fa un thread.

Le variabili condivise tra callback e thread vanno protette: `atomic_t`, oppure
passate con semafori e code.

> **Limite di Renode.** Con Renode 1.16 e Zephyr 4.5 gli interrupt GPIO **non scattano**
> nel simulatore: la configurazione va a buon fine, ma alla pressione del pulsante la
> callback non viene mai chiamata (né sul fronte né a livello). Per questo negli esercizi
> i pulsanti si leggono **campionandoli con un timer** (qui sotto). L'API degli interrupt
> qui sopra resta quella da usare sulla scheda vera: la provi nei bonus.

### 4.1 Campionamento con un timer

Un `k_timer` periodico (es. ogni 10 ms) legge il pin nella sua funzione (contesto di
interrupt del timer: `gpio_pin_get_dt` è lecita, è solo una lettura di registro) e
riconosce il **fronte** confrontando la lettura con quella precedente:

```c
static void campiona(struct k_timer *t)
{
	static int precedente;
	int ora = gpio_pin_get_dt(&btn);     /* 1 = premuto */

	if (ora && !precedente) {            /* fronte 0 -> 1: pressione */
		k_sem_give(&sem_pressione);  /* il resto lo fa un thread */
	}
	precedente = ora;
}
K_TIMER_DEFINE(timer_pulsante, campiona, NULL);
/* in main():  k_timer_start(&timer_pulsante, K_MSEC(10), K_MSEC(10)); */
```

| | Interrupt GPIO | Campionamento con timer |
|---|---|---|
| Latenza | immediata (µs) | fino a un periodo (10 ms): per una persona è invisibile |
| Consumo | la CPU dorme finché non premi | la CPU si sveglia 100 volte al secondo |
| Eventi brevissimi | li vede tutti | perde gli impulsi più brevi del periodo |
| Debounce | serve un meccanismo in più | è quasi gratis (sezione 5) |
| Semplicità | callback, maschere, GPIOTE | un timer e un confronto |
| Molti pulsanti | una callback per porta | un solo timer li legge tutti |

Nei firmware veri si vedono entrambi: interrupt quando conta la latenza o il consumo
(es. risveglio da sleep), campionamento per tastiere e pulsanti di interfaccia.

## 5. Rimbalzi e debounce

Un pulsante meccanico, quando lo premi, non passa in modo pulito da 1 a 0: i contatti
**rimbalzano** per qualche millisecondo e il pin oscilla:

```
pin  1 1 1 0 1 0 0 1 0 0 0 0 0 0 0      (una sola pressione!)
            \_______/
          rimbalzi ~1-10 ms
```

Con gli interrupt sui fronti, o con un campionamento molto veloce, una pressione può
contare 3-4 volte. Il **debounce** è la tecnica per ignorare i rimbalzi. Due modi comuni:

**Per campionamento (quello usato negli esercizi).** Si campiona ogni 10 ms e si accetta
un nuovo stato solo quando è rimasto **uguale per N campioni consecutivi** (N = 3 →
30 ms di stabilità):

```c
static int stabile, candidato, uguali;   /* stato accettato, ultima lettura, conteggio */

int letto = gpio_pin_get_dt(&btn);
if (letto == candidato) {
	uguali++;
} else {
	candidato = letto;
	uguali = 1;
}
if (uguali >= 3 && candidato != stabile) {
	stabile = candidato;   /* cambiamento valido: 1 = pressione, 0 = rilascio */
}
```

Variante "integratore": un contatore che sale (fino a N) quando legge 1 e scende (fino a 0)
quando legge 0; lo stato diventa 1 quando il contatore arriva a N e 0 quando arriva a 0.

**Con interrupt + lavoro ritardato (sulla scheda vera).**

1. a ogni fronte la callback (ri)programma un **lavoro ritardato** di ~30 ms
   (`k_work_reschedule`: se era già programmato, il conto alla rovescia riparte);
2. quando il lavoro scatta, i rimbalzi sono finiti: legge il pin **stabile** e decide.

In Renode i pulsanti non rimbalzano: i rimbalzi li simuli tu con più comandi
`Press`/`Release` uno dopo l'altro (macro della sezione 11). Attenzione: i comandi di una
macro arrivano a distanza di pochissimo tempo virtuale, quindi un campionamento a 10 ms
spesso non li vede nemmeno; per osservarli riduci il periodo del timer a 1 ms.

## 6. UART

La UART trasmette un byte alla volta, bit dopo bit, su un filo (TX) e riceve sull'altro (RX).
Ogni byte è una **trama** (*frame*): un bit di start (0), 8 bit di dati, un bit di stop (1).
Le due parti devono concordare la velocità (**baud rate**, 115200 bit/s per la console).

Nella scheda la UART della console (`uart0`) serve sia a `printk` (trasmissione) sia alla
ricezione dei caratteri che scrivi: TX e RX sono indipendenti, quindi puoi stampare e
leggere insieme. Il device si prende con `DEVICE_DT_GET(DT_CHOSEN(zephyr_console))`.

**Polling** (API "poll"):

```c
unsigned char c;
if (uart_poll_in(uart, &c) == 0) {  /* 0 = c'era un carattere, -1 = nessuno */
	uart_poll_out(uart, c);          /* lo rimanda indietro (eco) */
}
```

Se il programma non legge abbastanza spesso, i caratteri arrivati nel frattempo si perdono.

**A interrupt** (API "irq", `CONFIG_UART_INTERRUPT_DRIVEN=y`): la UART chiama una callback
quando arrivano dati; la callback li legge dalla FIFO hardware e li mette in un buffer.

```c
static void uart_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	if (!uart_irq_update(dev)) {
		return;
	}
	while (uart_irq_rx_ready(dev)) {
		if (uart_fifo_read(dev, &c, 1) == 1) {
			/* accumula c in un buffer di riga ... */
		}
	}
}

uart_irq_callback_user_data_set(uart, uart_cb, NULL);
uart_irq_rx_enable(uart);
```

Fine riga: il tasto Invio manda `\r` (*carriage return*), spesso seguito da `\n`
(*line feed*). Un lettore di righe robusto chiude la riga su uno dei due e ignora le righe
vuote.

Zephyr ha anche un'API **asincrona** (con DMA) che il corso non usa.

## 7. I2C

**I2C** (si legge "i-quadro-c") è un **bus**: due fili condivisi da più dispositivi.

- **SCL**: clock, generato dal **controller** (il microcontrollore);
- **SDA**: dati, in entrambe le direzioni.
- Ogni dispositivo **target** ha un **indirizzo a 7 bit** (il BME280: `0x76`).

Una transazione:

```
START | indirizzo + W | ACK | registro | ACK | RESTART | indirizzo + R | ACK | dato | NACK | STOP
        controller --------------------------->           <------------- target
```

- **START/STOP**: condizioni speciali che aprono e chiudono la transazione;
- **ACK**: dopo ogni byte chi riceve tira giù SDA per dire "ricevuto". Se nessuno risponde
  all'indirizzo arriva un **NACK** e la funzione Zephyr restituisce un errore (`-EIO`): è il
  primo controllo da fare quando un sensore "non risponde";
- **registri del dispositivo**: quasi tutti i sensori I2C si usano così: *scrivi il numero
  del registro, poi leggi* uno o più byte (lettura "a raffica", *burst*: il sensore avanza da
  solo al registro successivo).

Nel diagramma, tra la scrittura del registro e la lettura non c'è uno STOP ma un
**RESTART** (*repeated start*): il controller tiene il bus per sé e cambia direzione.
È la forma standard per leggere un registro. Si può fare anche con **due transazioni
separate**, la prima chiusa da uno STOP:

```
START | indirizzo + W | ACK | registro | ACK | STOP
START | indirizzo + R | ACK | dato | ... | NACK | STOP
```

Il BME280 (e quasi tutti i sensori) accetta entrambe le forme: il "puntatore al registro"
resta memorizzato nel sensore tra le due transazioni.

API Zephyr di base (bus + indirizzo):

```c
const struct device *bus = DEVICE_DT_GET(DT_NODELABEL(i2c0));
uint8_t reg = 0xD0, id;
i2c_write(bus, &reg, 1, 0x76);         /* transazione 1: "parto dal registro 0xD0" */
i2c_read(bus, &id, 1, 0x76);           /* transazione 2: leggo 1 byte */
uint8_t cmd[2] = {0xF4, 0x27};
i2c_write(bus, cmd, 2, 0x76);          /* scrive 0x27 nel registro 0xF4 */
```

Sulla scheda vera si usano di solito le funzioni con il repeated start, spesso nella
versione `_dt` che prende bus e indirizzo da una `i2c_dt_spec` del devicetree:

```c
i2c_write_read(bus, 0x76, &reg, 1, &id, 1);         /* scrivi + leggi, con RESTART */
i2c_reg_read_byte_dt(&sens, 0xD0, &id);             /* legge un registro */
i2c_burst_read_dt(&sens, 0x88, buf, sizeof(buf));   /* legge registri consecutivi */
```

> **Limite di Renode: niente repeated start.** Il modello I2C del nRF52840 nel simulatore
> gestisce bene le transazioni chiuse da STOP, ma **non** il repeated start:
> `i2c_write_read`, `i2c_reg_read_byte`, `i2c_burst_read` (e le versioni `_dt`) restituiscono
> 0 ("tutto bene") ma con **dati sbagliati**. Nel corso quindi si legge sempre con
> `i2c_write` + `i2c_read`. Per lo stesso motivo il driver ufficiale di Zephyr per il
> BME280 non si inizializza in Renode, e il driver lo scriviamo noi (Modulo 6,
> esercizio 7). Sulla scheda vera tutte e due le forme funzionano.

Nel devicetree un dispositivo I2C si descrive come nodo **figlio** del bus, con l'indirizzo
in `reg` (così fanno i driver ufficiali; nel corso non serve, perché il nostro driver usa
direttamente bus e indirizzo):

```dts
&i2c0 {
	status = "okay";
	bme280: bme280@76 {
		compatible = "bosch,bme280";
		reg = <0x76>;
	};
};
```

Nella scheda simulata il bus `i2c0` di Zephyr è la periferica `twi0` di Renode
(TWI = *Two Wire Interface*, il nome che Nordic dà all'I2C).

## 8. Il BME280: registri e calibrazione

Il BME280 (Bosch) misura temperatura, pressione e umidità. Registri principali:

| Registro | Nome | Contenuto |
|---|---|---|
| `0xD0` | `id` | identificativo del chip: `0x60` |
| `0xE0` | `reset` | scrivere `0xB6` resetta il sensore |
| `0x88`-`0xA1` | `calib00..25` | coefficienti di calibrazione (scritti in fabbrica) |
| `0xF2` | `ctrl_hum` | sovracampionamento dell'umidità |
| `0xF3` | `status` | bit 3 = misura in corso |
| `0xF4` | `ctrl_meas` | sovracampionamento di temperatura e pressione + modo |
| `0xF7`-`0xFE` | dati | pressione (3 byte), temperatura (3 byte), umidità (2 byte) |

Registri usati nel corso (solo temperatura):

| Registri | Contenuto |
|---|---|
| `0xD0` | ID, deve valere `0x60` |
| `0x88`, `0x89` | `dig_T1` (byte basso, byte alto), **senza segno** (`uint16_t`) |
| `0x8A`, `0x8B` | `dig_T2`, **con segno** (`int16_t`) |
| `0x8C`, `0x8D` | `dig_T3`, **con segno** (`int16_t`) |
| `0xF4` | `ctrl_meas`: scrivere `0x27` avvia le misure continue |
| `0xFA`, `0xFB`, `0xFC` | `temp_msb`, `temp_lsb`, `temp_xlsb`: temperatura grezza |

- I coefficienti a 16 bit sono **little-endian**: prima il byte basso, poi quello alto.
  `dig_T1 = buf[0] | (buf[1] << 8)`; per `dig_T2` e `dig_T3` lo stesso, ma salvati in un
  `int16_t` (così `0x8000` vale −32768 e non 32768).
- `ctrl_meas` = `osrs_t` (3 bit) | `osrs_p` (3 bit) | `mode` (2 bit).
  `0x27` = `001 001 11`: temperatura ×1, pressione ×1, modo **normal** (misure continue).
  `0x25` = `001 001 01`: modo *forced* (una misura, poi il sensore torna a riposo).
- La temperatura grezza è un numero a **20 bit**:
  `adc_T = (msb << 12) | (lsb << 4) | (xlsb >> 4)` (di `xlsb` contano solo i 4 bit alti).

**Formula di compensazione** (datasheet BME280, sezione 4.2.3, versione intera a 32 bit):
trasforma `adc_T` in centesimi di grado usando i coefficienti di calibrazione, che sono
diversi per ogni esemplare di sensore (misurati in fabbrica).

```c
int32_t var1, var2, t_fine, T;
var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) *
          ((adc_T >> 4) - ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
t_fine = var1 + var2;          /* serve anche alle formule di pressione e umidita' */
T = (t_fine * 5 + 128) >> 8;   /* centesimi di grado: 2250 = 22.50 C */
```

- Tutto in interi: `>> 11` è una divisione per 2^11 molto più veloce. Nessun `float`.
- I cast `(int32_t)` servono perché `dig_T1` è senza segno: senza, la sottrazione verrebbe
  fatta senza segno e il risultato sarebbe sbagliato.
- Esempio con i valori del sensore simulato da Renode: `dig_T1 = 32768`,
  `dig_T2 = -32768`, `dig_T3 = 0`, registri `0xFA-0xFC` = `71 f0 d0` →
  `adc_T = 466701`, `var1 = 115184`, `var2 = 0`, `T = 2250` → **22.50 °C**.

Conoscere registri e formula è tutto ciò che serve per scrivere un driver: è quello che fai
nell'esercizio 7.

## 9. La sensor API

Su una scheda vera non si scrive un driver per ogni sensore: Zephyr ne ha già centinaia, e
per i sensori offre un'API unica, uguale per tutti i modelli. Nel corso non la usiamo,
perché il driver ufficiale del BME280 non funziona in Renode (riquadro nella sezione 7), ma
è importante sapere com'è fatta:

```c
const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(bme280));
struct sensor_value t;

sensor_sample_fetch(dev);                               /* 1. il sensore misura TUTTO */
sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &t);  /* 2. leggi un canale */
```

- **fetch** fa la comunicazione con il sensore (lenta); **get** prende un valore già
  misurato (veloce). Un fetch, più get.
- Canali del BME280: `SENSOR_CHAN_AMBIENT_TEMP` (°C), `SENSOR_CHAN_PRESS` (kPa),
  `SENSOR_CHAN_HUMIDITY` (%).
- **`struct sensor_value`** = due interi: `val1` (parte intera) e `val2` (milionesimi).
  23,5 °C → `val1 = 23`, `val2 = 500000`. −0,25 °C → `val1 = 0`, `val2 = -250000`.
  Si usano interi perché molti microcontrollori non hanno l'unità per i numeri con la
  virgola (il Cortex-M4F sì, ma Zephyr è pensato per tutti).
- `sensor_value_to_double(&t)` converte in `double`. Per stampare un `double` con
  `printk("%f")` serve `CONFIG_CBPRINTF_FP_SUPPORT=y` (il firmware cresce di qualche KB).
- Il driver si attiva da solo quando nel devicetree c'è un nodo `compatible = "bosch,bme280"`
  con `status = "okay"` e in `prj.conf` c'è `CONFIG_SENSOR=y`.
- Il vantaggio: se domani cambi sensore (per esempio un SHT3x al posto del BME280) cambi il
  nodo nel devicetree, e il codice che chiama `sensor_sample_fetch`/`sensor_channel_get`
  resta identico. È la stessa idea della tua interfaccia `bme280_corso.h`, in grande.

## 10. Watchdog

Il **watchdog** ("cane da guardia") è un timer hardware che, se nessuno lo "nutre" (*feed*)
entro un certo tempo, **resetta il microcontrollore**. Serve a uscire da blocchi che il
software da solo non vede: un thread bloccato per sempre, un ciclo infinito per un bug,
un'interferenza elettrica.

```c
const struct device *wdt = DEVICE_DT_GET(DT_ALIAS(watchdog0));
struct wdt_timeout_cfg cfg = {
	.window.min = 0,
	.window.max = 2000,           /* ms: nutrimi almeno ogni 2 s */
	.callback = NULL,
	.flags = WDT_FLAG_RESET_SOC,  /* allo scadere: reset di tutto il chip */
};
int canale = wdt_install_timeout(wdt, &cfg);
wdt_setup(wdt, WDT_OPT_PAUSE_HALTED_BY_DBG);
/* ... poi, periodicamente ... */
wdt_feed(wdt, canale);
```

- Una volta avviato, il watchdog **non si può fermare** (fino al reset): è voluto.
- `WDT_OPT_PAUSE_HALTED_BY_DBG`: si ferma quando il debugger blocca la CPU (altrimenti
  ogni breakpoint causerebbe un reset).
- `WDT_OPT_PAUSE_IN_SLEEP`: si ferma quando la CPU dorme. **Attenzione**: in un RTOS, quando
  tutti i thread sono bloccati la CPU dorme; con questa opzione un blocco "totale" non
  verrebbe mai rilevato.
- Chi nutre il watchdog? Mai un timer (continuerebbe a nutrire anche con i thread bloccati).
  Lo deve nutrire il codice che si vuole sorvegliare, oppure un supervisore che nutre solo
  se **tutti** i thread hanno dato segni di vita di recente.
- `CONFIG_WDT_DISABLE_AT_BOOT`: alcuni bootloader lasciano il watchdog acceso; questa opzione
  prova a spegnerlo all'avvio. Sul nRF52 non si può spegnere: non la usiamo.
- Dopo il reset il firmware riparte da capo: in Renode vedi un nuovo banner
  `*** Booting Zephyr OS ***`. La causa del reset si legge con l'API `hwinfo`
  (`hwinfo_get_reset_cause`).

## 11. Le periferiche in Renode

Comandi utili nel monitor per questo modulo:

| Comando | Effetto |
|---|---|
| `sysbus.gpio0.sw0 Press` / `Release` | preme / rilascia il pulsante 0 |
| `sysbus.gpio0.led0 State` | `True` se il LED 0 è acceso |
| `sysbus.twi0.bme280` | mostra proprietà e metodi del sensore simulato |
| `sysbus.twi0.bme280 <Proprietà>` | legge una proprietà; `sysbus.twi0.bme280 <Proprietà> 25.5` la imposta |
| `sysbus.twi0.bme280 Temperature 35.5` | imposta la temperatura (°C); all'avvio vale 22.5 (lo imposta `renode/esercizio.resc`) |
| `sysbus.twi0.bme280 Humidity 60` | imposta l'umidità relativa (%); all'avvio vale 45. C'è anche `Pressure` |
| `logLevel 0 sysbus.twi0` | mostra ogni accesso al bus I2C (utile per il debug) |
| `sysbus.wdt` | stato del watchdog simulato |

Una **macro** esegue più comandi di seguito senza che tu debba scriverli uno a uno (utile per
simulare rimbalzi):

```
macro rimbalzo
"""
sysbus.gpio0.sw0 Press
sysbus.gpio0.sw0 Release
sysbus.gpio0.sw0 Press
sysbus.gpio0.sw0 Release
sysbus.gpio0.sw0 Press
"""
runMacro $rimbalzo
```
