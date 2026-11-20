# Esercizio 1 (Modulo 0) — Installare gli strumenti

Teoria: [teoria/01_setup_toolchain.md](../teoria/01_setup_toolchain.md), sezioni 3-5.

**Obiettivo:** avere sul PC tutto il necessario per compilare un firmware Zephyr e
simularlo con Renode. Ogni passo finisce con un **controllo**: se il controllo non dà il
risultato atteso, fermati e risolvi prima di andare avanti.

Spazio su disco necessario: circa **8-10 GB**. Tempo: 1-2 ore (soprattutto download).
Riferimento ufficiale (in inglese): https://docs.zephyrproject.org/latest/develop/getting_started/index.html
(scheda "Windows").

---

## Passo 1 — Permettere gli script PowerShell

Windows di default blocca gli script `.ps1` (anche `attiva.ps1` e quelli del corso).
Apri PowerShell **normale** (non da amministratore) e scrivi:

```powershell
Set-ExecutionPolicy -Scope CurrentUser RemoteSigned
```

Vale solo per il tuo utente: permette gli script scritti sul PC, blocca quelli scaricati
non firmati.

**Controllo:** `Get-ExecutionPolicy -Scope CurrentUser` stampa `RemoteSigned`.

## Passo 2 — Strumenti di base con winget

```powershell
winget install Kitware.CMake Ninja-build.Ninja oss-winget.gperf Python.Python.3.12 Git.Git oss-winget.dtc 7zip.7zip Renode.Renode
```

(Renode si installa anche da qui: il Passo 6 serve solo a controllarlo.)

Poi **chiudi e riapri** il terminale (così vede i nuovi programmi nel PATH).

**Controllo:** ognuno di questi comandi stampa una versione, senza errori:

```powershell
git --version
cmake --version
ninja --version
python --version
dtc --version
```

> Git: GitHub Desktop ha un suo Git interno, ma gli altri programmi (west) non lo vedono.
> Serve questo Git "di sistema". Puoi continuare a usare GitHub Desktop per i commit.

## Passo 3 — Ambiente virtuale e west

```powershell
python -m venv C:\zephyrproject\.venv
C:\zephyrproject\.venv\Scripts\Activate.ps1
python -m pip install west
```

Il prompt ora inizia con `(.venv)`.

**Controllo:** `python -m west --version` stampa la versione di west.

> **Smart App Control.** Su questo PC Windows ha attivo *Smart App Control*, che blocca gli
> eseguibili non firmati: `west.exe` viene bloccato ("Un criterio di controllo
> dell'applicazione ha bloccato il file"). Per questo si scrive **`python -m west`** al posto
> di `west`: è lo stesso programma, avviato tramite `python.exe`, che è firmato.
> Dopo `. .\attiva.ps1` puoi scrivere di nuovo semplicemente `west`: lo script definisce
> una funzione che fa la sostituzione per te. Il compilatore dell'SDK e Renode invece
> partono senza problemi.

## Passo 4 — Scaricare Zephyr

Scegli l'ultima versione **stabile** di Zephyr (un tag `vX.Y.Z`, non "rc") da
https://github.com/zephyrproject-rtos/zephyr/releases e usala al posto di `vX.Y.Z`:

```powershell
python -m west init -m https://github.com/zephyrproject-rtos/zephyr --mr vX.Y.Z C:\zephyrproject
cd C:\zephyrproject
python -m west update --narrow -o=--depth=1
```

`west update` scarica tutti i moduli: ci vuole tempo. `--narrow -o=--depth=1` scarica solo
la versione corrente di ogni modulo, senza tutta la storia: molto più veloce e leggero.
Poi i pacchetti Python che servono alla compilazione e la registrazione per CMake:

```powershell
python -m pip install -r C:\zephyrproject\zephyr\scripts\requirements.txt
python -m west zephyr-export
```

(Su Windows `west packages pip --install` non installa i pacchetti, perché west li sta
usando: per questo si usa pip direttamente.)

**Controllo:** esiste il file `C:\zephyrproject\zephyr\VERSION`. Aprilo e annota la versione
qui sotto.

## Passo 5 — Zephyr SDK (la toolchain)

Installiamo solo la toolchain ARM, che è quella che serve per il nRF52840:

```powershell
$env:Path += ';C:\Program Files\7-Zip'     # l'installazione dell'SDK vuole 7z nel PATH
cd C:\zephyrproject\zephyr
python -m west sdk install -t arm-zephyr-eabi
```

L'SDK finisce in `C:\Users\<utente>\zephyr-sdk-<versione>` (qui: `zephyr-sdk-1.0.1`) e il
compilatore è in `gnu\arm-zephyr-eabi\bin`. Se l'installazione si ferma con "requires '7z'",
il PATH non comprende 7-Zip: aggiungilo come sopra e lancia
`C:\Users\<utente>\zephyr-sdk-<versione>\setup.cmd /t arm-zephyr-eabi /c`.

**Controllo:** compila un esempio ufficiale:

```powershell
cd C:\zephyrproject
python -m west build -b nrf52840dk/nrf52840 -d build-prova zephyr\samples\hello_world --no-sysbuild
```

Deve finire con una tabella `Memory region / Used Size / Region Size` e il file
`C:\zephyrproject\build-prova\zephyr\zephyr.elf` deve esistere.

## Passo 6 — Renode

Scarica l'installer per Windows dell'ultima versione da
https://github.com/renode/renode/releases e installalo.

**Controllo:** dalla cartella del repo `zephyr-corso`:

```powershell
. .\attiva.ps1
```

Deve stampare in verde la versione di west e il percorso di Renode. Se dice che Renode non
è stato trovato, cerca `Renode.exe` nella cartella di installazione e imposta:

```powershell
[Environment]::SetEnvironmentVariable('RENODE', 'C:\percorso\di\Renode.exe', 'User')
```

(poi riapri il terminale).

## Passo 7 — VS Code

Installa l'estensione **C/C++** (Microsoft). Facoltativa: **CMake Tools**.
Apri la cartella `zephyr-corso` in VS Code: userai il suo terminale integrato (PowerShell).

## Passo 8 — Prova finale

```powershell
. .\attiva.ps1
.\esegui.ps1 modulo0\02_hello
```

Si apre Renode con una finestra della UART. Lo scheletro di `02_hello` non stampa ancora
niente di tuo, ma deve comparire il banner `*** Booting Zephyr OS build ... ***`.
Chiudi Renode (scrivi `quit` nel monitor).

---

## Stato su questo PC (20/11/2026)

I passi 2-6 sono già stati eseguiti: Git, Python 3.12, CMake, Ninja, dtc, gperf, 7-Zip,
Renode 1.16.0, Zephyr **v4.5.0** in `C:\zephyrproject`, Zephyr SDK **1.0.1** (solo toolchain
ARM). Restano da fare a mano il **Passo 1** (permesso per gli script) e il **Passo 7**
(VS Code), poi la prova finale del Passo 8. Le risposte qui sotto le puoi ricavare comunque.

## Le mie risposte (scrivile qui)

1. Versione di Zephyr (file `VERSION`):
2. Versione di Renode:
3. Quanto spazio occupa `C:\zephyrproject`? (tasto destro → Proprietà)
4. Quanto è durata la prima compilazione dell'esempio? E la seconda, rilanciando lo stesso
   comando senza cambiare nulla? Perché la seconda è così veloce?
5. Nella tabella `Memory region` alla fine della compilazione: quanti byte di FLASH e di RAM
   usa `hello_world`? Che percentuale del totale?
6. Problemi incontrati e come li hai risolti:
