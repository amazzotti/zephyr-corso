# zephyr-corso

Percorso personale di apprendimento di **Zephyr RTOS** senza hardware: tutto gira su una
scheda **nRF52840 DK simulata con [Renode](https://renode.io)**.

Si parte dal setup e dal C per l'embedded, poi build system (Kconfig, devicetree), thread,
sincronizzazione, timer, periferiche (GPIO, UART, I2C, watchdog), shell, test e debug,
fino a un progetto finale: un **nodo di monitoraggio della temperatura**.

- Piano, metodo e stato di avanzamento: [CORSO.md](CORSO.md)
- Appunti di teoria: [teoria/](teoria/)
- Esercizi: `modulo0/` ... `modulo8/`

## Uso rapido

```powershell
. .\attiva.ps1                       # una volta per terminale: ambiente Zephyr
.\esegui.ps1 modulo0\02_hello        # compila e apre l'esercizio in Renode
.\verifica.ps1 modulo0\02_hello      # compila, esegue e controlla l'output da solo
```
