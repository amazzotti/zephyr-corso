# 07 — Periferiche e driver (Modulo 6)

## 1. Dai registri ai driver

Nel Modulo 1 hai acceso un LED scrivendo direttamente nel registro `BSRR` della porta D.
Funziona, ma:
- il codice vale solo per quel chip (su un nRF52 o un ESP32 i registri sono completamente
  diversi, e anche tra famiglie STM32 cambiano);
- devi conoscere a memoria indirizzi e significato di ogni bit, e ricordarti di accendere
  il clock della periferica nel registro `RCC_AHB1ENR`;
- se due parti del programma usano la stessa periferica, nessuno coordina gli accessi.

Un **driver** è il codice che conosce i registri di una periferica e offre in cambio
**funzioni generiche** (`gpio_pin_set`, `uart_poll_out`, `i2c_write_read`...). Il tuo codice
chiama le funzioni; il driver le traduce in letture e scritture di registri.

```
  tuo codice      gpio_pin_set_dt(&led, 1)          (uguale su ogni chip)
       |
  API Zephyr      include/zephyr/drivers/gpio.h     (interfaccia comune)
       |
  driver          drivers/gpio/gpio_stm32.c         (specifico degli STM32)
       |
  registri        GPIOD->BSRR = (1 << 12)           (hardware)
```

## 2. Il modello dei driver di Zephyr

Ogni periferica è rappresentata da una **`struct device`**, creata in compilazione a partire
dal **devicetree**: un nodo con `status = "okay"` e un driver compatibile diventa un device.

```c
const struct device *uart = DEVICE_DT_GET(DT_CHOSEN(zephyr_console));
const struct device *sens = DEVICE_DT_GET(DT_NODELABEL(sensore));
```

- `DEVICE_DT_GET(nodo)` restituisce il puntatore al device, deciso in compilazione: non può
  fallire a runtime, ma se il nodo non esiste o è disabilitato la compilazione si ferma
  (errore con `__device_dts_ord_...`: è il segno che manca qualcosa nel devicetree o in
  `prj.conf`).
- **`device_is_ready(dev)`**: il driver è stato inizializzato bene? Va **sempre** controllato
  prima dell'uso. Le varianti `_dt` (`gpio_is_ready_dt`, `i2c_is_ready_dt`) fanno lo stesso.
- **Inizializzazione**: all'avvio Zephyr chiama la funzione di init di ogni driver, in ordine
  di livello (`PRE_KERNEL_1`, `PRE_KERNEL_2`, `POST_KERNEL`, `APPLICATION`) e di priorità.
  Per questo in `main()` trovi già tutto pronto (clock accesi, pin configurati).
- **Kconfig attiva il codice del driver**: `CONFIG_GPIO=y`, `CONFIG_I2C=y`, `CONFIG_SENSOR=y`...
  Senza l'opzione il driver non viene compilato e il device non esiste.
- Le funzioni dell'API restituiscono un `int`: **0 = tutto bene, negativo = errore** (un codice
  `errno` come `-EIO`, errore di I/O, o `-ENODEV`). Controllalo sempre.

Le strutture `_dt_spec` raccolgono in un unico oggetto tutto ciò che il devicetree dice di un
collegamento:

```c
/* GPIO: quale porta, quale pin, quali flag (es. attivo alto/basso) */
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
/* I2C: quale bus e quale indirizzo */
static const struct i2c_dt_spec sens = I2C_DT_SPEC_GET(DT_NODELABEL(sensore));
```

## 3. GPIO

**GPIO** (*General Purpose Input/Output*) = un pin che il programma può usare come uscita
(accendere un LED) o come ingresso (leggere un pulsante).

- **Uscita**: il pin è collegato a 3,3 V (livello 1, "alto") o a massa (livello 0, "basso").
- **Ingresso**: il programma legge se il pin è alto o basso. Un ingresso non collegato a
  nulla "fluttua" e legge valori casuali: per questo si usa una **resistenza di pull-up**
  (porta il pin a 1 quando nessuno lo tira giù) o di **pull-down** (lo porta a 0).

Sull'STM32F407 i pin sono raggruppati in **porte** da 16 (`PA0`..`PA15`, `PB0`..., fino a
`PK`). Ogni porta ha i suoi registri, per esempio la porta D a `0x40020C00`:

| Registro | Offset | A cosa serve |
|---|---|---|
| `MODER` | `0x00` | 2 bit per pin: `00` ingresso, `01` uscita, `10` funzione alternativa (UART, I2C...), `11` analogico |
| `PUPDR` | `0x0C` | 2 bit per pin: nessuna resistenza, pull-up, pull-down |
| `IDR` | `0x10` | livello letto sui pin d'ingresso (1 bit per pin) |
| `ODR` | `0x14` | livello delle uscite |
| `BSRR` | `0x18` | scrittura "atomica": bit n = metti a 1 il pin n, bit n+16 = mettilo a 0 |

Prima di usare una porta va acceso il suo **clock** (registro `RCC_AHB1ENR`): sugli STM32
ogni periferica è spenta all'avvio per risparmiare energia. Il driver di Zephyr lo fa per te.

Sulla **STM32F4 Discovery**:
- i 4 LED (PD12 verde, PD13 arancione, PD14 rosso, PD15 blu) sono **attivi alti**: si
  accendono quando il pin è a 1;
- il pulsante utente blu (PA0) è **attivo alto**: premuto porta il pin a 3,3 V, a riposo
  una resistenza di pull-down sulla scheda lo tiene a 0.

Il devicetree descrive tutto questo con i flag (`GPIO_ACTIVE_HIGH`, o `GPIO_ACTIVE_LOW` su
schede dove il pulsante va a massa), e le funzioni `_dt` lavorano in **logica**:
`gpio_pin_get_dt(&btn)` restituisce **1 = premuto** in entrambi i casi. Non devi invertire
niente a mano, e il codice è lo stesso su schede diverse.

```c
gpio_pin_configure_dt(&btn, GPIO_INPUT);          /* resistenze gia' nei flag del DT */
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
numero di interrupt e una priorità; la tabella dei vettori (in flash) dice quale ISR
eseguire per ciascuno.

### EXTI: gli interrupt dei pin sull'STM32

I pin non arrivano al NVIC direttamente: passano per l'**EXTI** (*EXTernal Interrupt/event
controller*).

```
 PA0 --\
 PB0 ---+--[SYSCFG_EXTICR: scegli la porta]--> linea EXTI 0 --> NVIC (EXTI0_IRQn)
 ...   /
 PK0 -/
 PA7, PB7, ..., PE7 ------------------------> linea EXTI 7 --> NVIC (EXTI9_5_IRQn)
```

- Le linee EXTI sono **16**, una per **numero di pin**: la linea 0 serve il pin 0 di
  qualunque porta, la linea 7 il pin 7, ecc. I registri `SYSCFG_EXTICR` scelgono **quale
  porta** usa ogni linea. Conseguenza: **PA7 e PE7 non possono avere entrambi un
  interrupt**, mentre PA0 e PE7 sì (linee diverse).
- Per ogni linea si sceglie il fronte (registri `EXTI_RTSR` salita, `EXTI_FTSR` discesa) e
  la si abilita (`EXTI_IMR`). Quando scatta, il bit della linea in `EXTI_PR` resta a 1
  finché la ISR non lo azzera.
- Le linee 0-4 hanno un interrupt ciascuna nel NVIC; le linee 5-9 ne condividono uno
  (`EXTI9_5`), e così le 10-15 (`EXTI15_10`): la ISR guarda `EXTI_PR` per capire quale
  pin ha scattato.

In Zephyr non scrivi la ISR: la scrive il driver (EXTI + GPIO), e lui chiama la **tua
callback** registrata con `gpio_init_callback` + `gpio_add_callback`.

- **Fronte**: il passaggio da 0 a 1 (*salita*) o da 1 a 0 (*discesa*). Con
  `GPIO_INT_EDGE_TO_ACTIVE` l'interrupt scatta quando il pin diventa *attivo* (premuto),
  qualunque sia il livello elettrico; con `GPIO_INT_EDGE_BOTH` su entrambi i fronti.

```c
static struct gpio_callback cb_data;      /* static: il driver ne conserva l'indirizzo */

static void pulsante_premuto(const struct device *dev, struct gpio_callback *cb,
			     uint32_t pins)
{
	/* contesto di interrupt: breve, niente attese. "pins" = maschera dei pin scattati */
}

gpio_pin_interrupt_configure_dt(&btn, GPIO_INT_EDGE_TO_ACTIVE);
gpio_init_callback(&cb_data, pulsante_premuto, BIT(btn.pin));
gpio_add_callback(btn.port, &cb_data);
```

Una `gpio_callback` appartiene a **una porta** (`btn.port`): per pulsanti su porte diverse
servono callback diverse; per più pin della stessa porta basta una callback con una maschera
(`BIT(7) | BIT(8)`), e l'argomento `pins` dice quale ha scattato.

Regole della callback (le stesse del Modulo 5 per il timer): niente `k_msleep`, niente
`k_mutex_lock`, niente attese; `printk` è permesso ma lento. Si fa il minimo (contare,
`k_sem_give`, `k_work_submit`, `k_msgq_put` con `K_NO_WAIT`) e il resto lo fa un thread.

Le variabili condivise tra callback e thread vanno protette: `atomic_t`, oppure
passate con semafori e code.

### 4.1 Alternativa: campionamento con un timer

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

| | Interrupt GPIO (EXTI) | Campionamento con timer |
|---|---|---|
| Latenza | immediata (µs) | fino a un periodo (10 ms): per una persona è invisibile |
| Consumo | la CPU dorme finché non premi | la CPU si sveglia 100 volte al secondo |
| Eventi brevissimi | li vede tutti | perde gli impulsi più brevi del periodo |
| Debounce | serve un meccanismo in più (sezione 5) | è quasi gratis |
| Vincoli hardware | una linea EXTI per numero di pin | nessuno |
| Molti pulsanti | una callback per porta | un solo timer li legge tutti |

Nei firmware veri si vedono entrambi: interrupt quando contano latenza o consumo
(es. risveglio da sleep), campionamento per tastiere e pulsanti di interfaccia.

## 5. Rimbalzi e debounce

Un pulsante meccanico, quando lo premi, non passa in modo pulito da 0 a 1: i contatti
**rimbalzano** per qualche millisecondo e il pin oscilla:

```
pin  0 0 0 1 0 1 1 0 1 1 1 1 1 1 1      (una sola pressione!)
            \_______/
          rimbalzi ~1-10 ms
```

Con gli interrupt sui fronti una pressione può contare 3-4 volte. Il **debounce** è la
tecnica per ignorare i rimbalzi.

**Con interrupt + lavoro ritardato (quello usato negli esercizi).**

1. a ogni fronte la callback (ri)programma un **lavoro ritardato** di ~30 ms
   (`k_work_reschedule`: se era già programmato, il conto alla rovescia riparte da zero);
2. quando il lavoro scatta, i rimbalzi sono finiti da almeno 30 ms: legge il pin
   **stabile** e lo confronta con l'ultimo stato stabile.

```c
static void pulsante_stabile(struct k_work *w)      /* gira nella work queue */
{
	static int stabile;
	int letto = gpio_pin_get_dt(&btn);

	if (letto != stabile) {
		stabile = letto;   /* cambiamento valido: 1 = pressione, 0 = rilascio */
	}
}
K_WORK_DELAYABLE_DEFINE(lavoro_debounce, pulsante_stabile);

static void fronte(const struct device *d, struct gpio_callback *cb, uint32_t pins)
{
	k_work_reschedule(&lavoro_debounce, K_MSEC(30));
}
/* interrupt su entrambi i fronti: GPIO_INT_EDGE_BOTH */
```

**Per campionamento.** Si campiona ogni 10 ms e si accetta un nuovo stato solo quando è
rimasto **uguale per N campioni consecutivi** (N = 3 → 30 ms di stabilità):

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
	stabile = candidato;
}
```

In Renode i pulsanti non rimbalzano: i rimbalzi li simuli tu con più comandi
`Press`/`Release` di seguito. Per farli arrivare "nello stesso istante" simulato, ferma il
tempo virtuale prima (`pause`) e fallo ripartire dopo (`start`), oppure usa la macro della
sezione 11.

## 6. UART

La UART trasmette un byte alla volta, bit dopo bit, su un filo (TX) e riceve sull'altro (RX).
Ogni byte è una **trama** (*frame*): un bit di start (0), 8 bit di dati, un bit di stop (1).
Le due parti devono concordare la velocità (**baud rate**, 115200 bit/s per la console).

Sulla Discovery la console è la **USART2** (TX su PA2, RX su PA3, pin in "funzione
alternativa"). Registri principali: `USART_SR` (stato: bit `RXNE` = "è arrivato un byte",
bit `TXE` = "posso trasmettere"), `USART_DR` (il byte ricevuto o da trasmettere),
`USART_BRR` (divisore per il baud rate).

La stessa UART serve sia a `printk` (trasmissione) sia alla ricezione dei caratteri che
scrivi: TX e RX sono indipendenti, quindi puoi stampare e leggere insieme. Il device si
prende con `DEVICE_DT_GET(DT_CHOSEN(zephyr_console))`.

**Polling** (API "poll"):

```c
unsigned char c;
if (uart_poll_in(uart, &c) == 0) {  /* 0 = c'era un carattere, -1 = nessuno */
	uart_poll_out(uart, c);          /* lo rimanda indietro (eco) */
}
```

Se il programma non legge abbastanza spesso, i caratteri arrivati nel frattempo si perdono:
la USART dell'STM32F4 tiene **un solo byte** in `DR`.

**A interrupt** (API "irq", `CONFIG_UART_INTERRUPT_DRIVEN=y`): la UART chiama una callback
quando arrivano dati (interrupt `USART2` nel NVIC, scatenato da `RXNE`); la callback li legge
e li mette in un buffer.

```c
static void uart_cb(const struct device *dev, void *user_data)
{
	uint8_t c;

	uart_irq_update(dev);            /* una volta per callback: legge lo stato */
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
- Ogni dispositivo **target** ha un **indirizzo a 7 bit** (l'ADT7420 del corso: `0x48`).

Una lettura di registro:

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

Tra la scrittura del registro e la lettura non c'è uno STOP ma un **RESTART** (*repeated
start*): il controller tiene il bus per sé e cambia direzione. È la forma standard per
leggere un registro.

API Zephyr (versioni `_dt`, che prendono bus e indirizzo da una `i2c_dt_spec`):

```c
static const struct i2c_dt_spec sens = I2C_DT_SPEC_GET(DT_NODELABEL(sensore));

uint8_t reg = 0x0B, id;
i2c_write_read_dt(&sens, &reg, 1, &id, 1);          /* scrivi + leggi, con RESTART */
i2c_reg_read_byte_dt(&sens, 0x0B, &id);             /* lo stesso, per un registro */
i2c_burst_read_dt(&sens, 0x00, buf, 2);             /* registri consecutivi */
uint8_t cmd[2] = {0x03, 0x80};
i2c_write_dt(&sens, cmd, 2);                        /* scrive 0x80 nel registro 0x03 */
```

Nel devicetree un dispositivo I2C si descrive come nodo **figlio** del bus, con l'indirizzo
in `reg`. Sulla scheda vera il sensore starebbe sul bus `i2c1` della Discovery (PB6 = SCL,
PB9 = SDA):

```dts
&i2c1 {
	sensore: adt7420@48 {
		compatible = "adi,adt7420";
		reg = <0x48>;
	};
};
```

### 7.1 Il sensore emulato del corso

Il modello Renode della periferica I2C dell'STM32F4 non è affidabile (dopo la prima
transazione i dati si disallineano). Per questo il sensore lo simula **Zephyr stesso**
(`comune/sensore_sim.overlay`): un controller I2C **emulato** (`zephyr,i2c-emul-controller`)
con un ADT7420 emulato all'indirizzo `0x48`.

```
 tuo codice / driver adt7420                 tuo codice / driver adt7420
          |  i2c_write_read_dt                         |
 [controller I2C emulato]  <- nel corso      [I2C1 dell'STM32] <- scheda vera
          |                                            |  SCL / SDA
 [emulatore ADT7420: registri in RAM]         [ADT7420 vero]
```

- Per il codice degli esercizi non cambia nulla: stesse funzioni, stessi registri, stessi
  valori di ritorno. Sulla scheda vera basterebbe spostare il nodo `sensore` sotto `&i2c1`.
- Quello che **non** vedi è la periferica I2C a livello di registri dell'STM32 (`I2C_CR1`,
  `I2C_SR1`...): la gestisce il driver, ed è quello che Renode non simula bene.
- La temperatura del sensore emulato si cambia dalla console con `sim temp <centesimi>`
  (codice in `comune/sensore_sim.c`): parte da 22,50 °C.

## 8. L'ADT7420: registri e conversione

L'ADT7420 (Analog Devices) è un sensore di temperatura digitale, preciso (±0,25 °C).
Registri principali:

| Registro | Nome | Contenuto |
|---|---|---|
| `0x00` | Temperature MSB | byte alto della temperatura |
| `0x01` | Temperature LSB | byte basso della temperatura |
| `0x02` | Status | bit 7 `RDY` (0 = nuova misura pronta), bit 4-6 soglie superate |
| `0x03` | Configuration | bit 7 risoluzione (0 = 13 bit, 1 = 16 bit), bit 6-5 modo (00 continuo, 01 una misura, 10 una al secondo, 11 spento) |
| `0x04`-`0x0A` | soglie | T_HIGH, T_LOW, T_CRIT, isteresi |
| `0x0B` | ID | identificativo del chip: `0xCB` |
| `0x2F` | Reset | scriverci resetta il sensore |

**Formato della temperatura a 13 bit** (quello predefinito, usato dal driver di Zephyr):

```
      MSB (0x00)              LSB (0x01)
 b15 b14 ... b9 b8     b7 b6 b5 b4 b3 | b2 b1 b0
 [segno e 12 bit di temperatura      ] [non usati]
```

- Il sensore manda **prima il byte alto** (*big-endian*): `valore = (msb << 8) | lsb`.
- Il valore è **con segno** (complemento a due): va messo in un `int16_t`.
- I 13 bit utili sono i bit 15..3: `grezzo = valore >> 3` (lo shift di un numero negativo
  conserva il segno con GCC).
- Ogni unità vale **0,0625 °C** = 1/16 °C. In centesimi senza `float`:
  `centesimi = grezzo * 25 / 4` (perché 100/16 = 25/4; prima moltiplica, poi dividi).

| Temperatura | `grezzo` | registri `0x00`, `0x01` |
|---|---|---|
| 22,50 °C | 360 | `0x0B`, `0x40` |
| 35,50 °C | 568 | `0x11`, `0xC0` |
| −5,25 °C | −84 | `0xFD`, `0x60` |

Con la risoluzione a 16 bit (bit 7 di Configuration a 1) tutti i 16 bit sono temperatura e
ogni unità vale 1/128 °C.

Conoscere registri e formato è tutto ciò che serve per scrivere un driver: è quello che fai
nell'esercizio 6.

## 9. La sensor API

Su una scheda vera non si scrive un driver per ogni sensore: Zephyr ne ha già centinaia, e
per i sensori offre un'API unica, uguale per tutti i modelli:

```c
const struct device *dev = DEVICE_DT_GET(DT_NODELABEL(sensore));
struct sensor_value t;

sensor_sample_fetch(dev);                               /* 1. il sensore misura */
sensor_channel_get(dev, SENSOR_CHAN_AMBIENT_TEMP, &t);  /* 2. leggi un canale */
```

- **fetch** fa la comunicazione con il sensore (lenta); **get** prende un valore già
  misurato (veloce). Un fetch, più get.
- Canali comuni: `SENSOR_CHAN_AMBIENT_TEMP` (°C), `SENSOR_CHAN_PRESS` (kPa),
  `SENSOR_CHAN_HUMIDITY` (%), `SENSOR_CHAN_ACCEL_XYZ` (m/s²)... L'ADT7420 ha solo la
  temperatura.
- **`struct sensor_value`** = due interi: `val1` (parte intera) e `val2` (milionesimi).
  22,5 °C → `val1 = 22`, `val2 = 500000`. **Negativi**: −5,25 °C → `val1 = -5`,
  `val2 = -250000`; −0,25 °C → `val1 = 0`, `val2 = -250000`. Il segno sta in entrambi i
  campi (o solo in `val2` se la parte intera è 0): per stamparlo converti prima in centesimi
  (`val1 * 100 + val2 / 10000`) e gestisci il segno a parte.
- Si usano interi perché molti microcontrollori non hanno l'unità per i numeri con la
  virgola (il Cortex-M4F sì, ma Zephyr è pensato per tutti).
- `sensor_value_to_double(&t)` converte in `double`. Per stampare un `double` con
  `printk("%f")` serve `CONFIG_CBPRINTF_FP_SUPPORT=y` (il firmware cresce di qualche KB).
- Il driver si attiva da solo quando nel devicetree c'è un nodo con un `compatible`
  conosciuto (qui `"adi,adt7420"`) e `status = "okay"`, e in `prj.conf` c'è
  `CONFIG_SENSOR=y`. All'avvio il driver legge l'ID e scrive la configurazione.
- Il vantaggio: se domani cambi sensore (per esempio un BME280 o un SHT3x) cambi il nodo nel
  devicetree, e il codice che chiama `sensor_sample_fetch`/`sensor_channel_get` resta
  identico.

## 10. Watchdog

Il **watchdog** ("cane da guardia") è un timer hardware che, se nessuno lo "nutre" (*feed*)
entro un certo tempo, **resetta il microcontrollore**. Serve a uscire da blocchi che il
software da solo non vede: un thread bloccato per sempre, un ciclo infinito per un bug,
un'interferenza elettrica.

L'STM32F4 ha l'**IWDG** (*Independent WatchDoG*, indirizzo `0x40003000`): conta
all'indietro con un oscillatore suo, l'**LSI** (~32 kHz), indipendente dal clock della CPU.
Registri: `IWDG_KR` (chiavi: `0xCCCC` avvia, `0xAAAA` nutre, `0x5555` sblocca la
scrittura degli altri), `IWDG_PR` (prescaler), `IWDG_RLR` (valore di ricarica, 12 bit).
Timeout = (4 · 2^PR) · (RLR + 1) / 32000 secondi: da 0,125 ms a circa 32 s.

La scheda non attiva l'IWDG nel devicetree: gli esercizi lo attivano con un overlay.

```dts
&iwdg {
	status = "okay";
};
/ {
	aliases {
		watchdog0 = &iwdg;
	};
};
```

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

- Una volta avviato, l'IWDG **non si può fermare** (fino al reset): è voluto.
- `WDT_OPT_PAUSE_HALTED_BY_DBG`: si ferma quando il debugger blocca la CPU (altrimenti
  ogni breakpoint causerebbe un reset).
- `WDT_OPT_PAUSE_IN_SLEEP`: si fermerebbe quando la CPU dorme. Sull'STM32 il driver la
  **rifiuta** (`-ENOTSUP`). Ed è giusto diffidarne: in un RTOS, quando tutti i thread sono
  bloccati la CPU dorme, e con questa opzione un blocco "totale" non verrebbe mai rilevato.
- Chi nutre il watchdog? Mai un timer (continuerebbe a nutrire anche con i thread bloccati).
  Lo deve nutrire il codice che si vuole sorvegliare, oppure un supervisore che nutre solo
  se **tutti** i thread hanno dato segni di vita di recente.
- Dopo il reset il firmware riparte da capo: in Renode vedi un nuovo banner
  `*** Booting Zephyr OS ***`. La causa del reset si legge con l'API `hwinfo`
  (`hwinfo_get_reset_cause`, sull'STM32 dal registro `RCC_CSR`).

## 11. Le periferiche in Renode

Comandi utili nel monitor per questo modulo:

| Comando | Effetto |
|---|---|
| `sysbus.gpioPortA.sw0 Press` / `Release` | preme / rilascia il pulsante utente (PA0) |
| `sysbus.gpioPortA.sw0 Pressed` | `True` se è premuto |
| `sysbus.gpioPortE.sw1` ... `sw3` | i pulsanti extra su PE7, PE8, PE9 (stessi comandi) |
| `sysbus.gpioPortD.led0 State` | `True` se il LED verde (PD12) è acceso; `led1`..`led3` per gli altri |
| `sysbus ReadDoubleWord 0x40020010` | registro `IDR` della porta A (bit 0 = PA0) |
| `pause` / `start` | ferma / fa ripartire il tempo virtuale (per simulare i rimbalzi) |
| `sysbus.iwdg` | stato del watchdog simulato |
| `sysbus ReadDoubleWord 0x40003008` | registro `IWDG_RLR` (ricarica del watchdog) |

Il sensore di temperatura non è in Renode: si comanda dalla **console** (la finestra della
UART), non dal monitor:

| Comando nella console | Effetto |
|---|---|
| `sim temp 3550` | imposta la temperatura simulata a 35,50 °C (centesimi di grado) |
| `sim temp` | mostra la temperatura impostata |

Una **macro** esegue più comandi di seguito senza che tu debba scriverli uno a uno (utile per
simulare rimbalzi):

```
macro rimbalzo
"""
pause
sysbus.gpioPortA.sw0 Press
sysbus.gpioPortA.sw0 Release
sysbus.gpioPortA.sw0 Press
start
"""
runMacro $rimbalzo
```
