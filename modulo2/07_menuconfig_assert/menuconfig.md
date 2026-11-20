# Esercizio 7 (Modulo 2) — Esplorare menuconfig

Compila almeno una volta l'esercizio, poi:

```powershell
west build -t menuconfig -d modulo2\07_menuconfig_assert\build
```

(oppure `-t guiconfig` per la versione grafica). Rispondi qui sotto.

1. Con `/` cerca `ASSERT`. Qual è il nome completo dell'opzione che attiva `__ASSERT`?
   In quale menu si trova (percorso tra i menu)?
2. Premi `?` sull'opzione: da cosa dipende? Qual è il suo valore predefinito?
3. Cerca `MAIN_STACK_SIZE`: valore attuale? A cosa serve secondo l'aiuto?
4. Cerca `BOOT_BANNER`: in quale menu sta?
5. Trova il menu "Device Drivers": elenca tre tipi di driver che vedi attivi.
6. Cambia un'opzione qualsiasi in menuconfig, salva, e poi cerca la stessa opzione in
   `build/zephyr/.config`: è cambiata? Ora ricompila con `-Pulito` e ricontrolla: cosa
   è successo, e perché?
