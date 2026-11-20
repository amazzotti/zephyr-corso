# Esercizio 4 (Modulo 2) — Le mie risposte

Apri `build/zephyr/zephyr.dts` (dopo aver compilato) e rispondi.

1. Il nodo `chosen` ha la proprietà `zephyr,console`. A quale nodo punta? Quale altra
   proprietà `zephyr,...` c'è in `chosen` e a cosa serve secondo te?
2. Nodo `uart0`: valore di `reg`. Cosa sono i due numeri? Valore di `compatible`.
3. Nodo `led_0`: valore di `gpios`. Cosa significano le tre parti (`&gpio0`, `13`,
   `GPIO_ACTIVE_LOW` oppure il numero `1`)?
4. Perché il LED è "attivo basso"? (Teoria, sezione 7.)
5. Il nodo `i2c0` ha `status = "okay"` o `"disabled"`? Cosa cambierebbe nel firmware se
   fosse `disabled`?
6. Trova un nodo con `status = "disabled"` e scrivi quale periferica è.
7. Nel monitor di Renode, comando `peripherals`: a quale indirizzo è `twi0`? Coincide
   con `i2c0` del devicetree?
