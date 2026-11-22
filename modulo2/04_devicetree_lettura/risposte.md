# Esercizio 4 (Modulo 2) — Le mie risposte

Apri `build/zephyr/zephyr.dts` (dopo aver compilato) e rispondi.

1. Il nodo `chosen` ha la proprietà `zephyr,console`. A quale nodo punta? Quale altra
   proprietà `zephyr,...` c'è in `chosen` e a cosa serve secondo te?
2. Nodo `usart2`: valore di `reg`. Cosa sono i due numeri? Valore di `compatible`.
3. Nodo `led_4` (etichetta `green_led_4`): valore di `gpios`. Cosa significano le tre parti
   (`&gpiod`, `0xc`, `0x0`)? Che cosa vale `0x0` (`GPIO_ACTIVE_HIGH`) e cosa vorrebbe dire
   `GPIO_ACTIVE_LOW` (il numero `1`)?
4. Perché sulla Discovery il LED è "attivo alto"? Come sarebbe collegato un LED attivo basso?
   (Teoria, sezione 7.)
5. Il nodo `i2c1` ha `status = "okay"` o `"disabled"`? Cosa cambierebbe nel firmware se
   fosse `disabled`?
6. Trova un nodo con `status = "disabled"` e scrivi quale periferica è.
7. Nel monitor di Renode, comando `peripherals`: a quale indirizzo sono `usart2` e
   `gpioPortD`? Coincidono con i nodi `usart2` e `gpiod` del devicetree?
