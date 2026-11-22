# Compila un esercizio, lo esegue in Renode senza finestre e controlla da solo l'output,
# seguendo le istruzioni del file verifica.txt dell'esercizio.
#
# Uso:
#     .\verifica.ps1 modulo0\02_hello
#     .\verifica.ps1 modulo0\02_hello -NoBuild        # non ricompila
#     .\verifica.ps1 modulo0\02_hello -Timeout 20     # attesa massima per riga (secondi)
#     .\verifica.ps1 modulo8\progetto -File modulo8\verifiche\02_acquisizione.txt
#
# Istruzioni di verifica.txt (una per riga):
#     # commento
#     attendi: REGEX              aspetta una riga della UART che contiene REGEX
#     attendi 30: REGEX           come sopra, con timeout di 30 secondi
#     mai: REGEX                  alla fine: nessuna riga deve contenere REGEX
#     scrivi: testo               scrive "testo" + invio sulla UART
#     monitor: comando            comando nel monitor di Renode
#     controlla: comando => testo comando nel monitor; la risposta deve contenere "testo"
#     pausa: 500                  aspetta 500 ms
# Le regex non distinguono maiuscole/minuscole.

param(
    [Parameter(Mandatory = $true, Position = 0)][string]$Esercizio,
    [string]$File,
    [switch]$NoBuild,
    [switch]$Pulito,
    [int]$Timeout = 10,
    [int]$PortaUart = 33450,
    [int]$PortaMonitor = 33451
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'strumenti\comune.ps1')

$Ascii = [System.Text.Encoding]::ASCII
$Ansi  = '\x1B\[[0-9;?]*[A-Za-z]'
$Prompt = '\(stm32f4\)'

# --- stato della UART -------------------------------------------------------------------
$script:Righe = New-Object System.Collections.Generic.List[string]
$script:Parziale = ''
$script:Cursore = 0          # indice della prima riga non ancora "consumata" da attendi
$script:ParzialeUsato = 0    # caratteri della riga parziale gia' consumati

function Read-Uart {
    $buf = New-Object byte[] 4096
    while ($script:UartStream.DataAvailable) {
        $n = $script:UartStream.Read($buf, 0, $buf.Length)
        if ($n -le 0) { break }
        $testo = $script:Parziale + $Ascii.GetString($buf, 0, $n)
        $pezzi = $testo -split "`n"
        for ($i = 0; $i -lt $pezzi.Count - 1; $i++) {
            $riga = ($pezzi[$i] -replace $Ansi, '') -replace "`r", ''
            $script:Righe.Add($riga)
            Write-Host "  uart | $riga" -ForegroundColor DarkGray
        }
        $script:Parziale = $pezzi[$pezzi.Count - 1]
        if ($pezzi.Count -gt 1) { $script:ParzialeUsato = 0 }
    }
}

function Wait-Riga([string]$Regex, [int]$Secondi) {
    $fine = (Get-Date).AddSeconds($Secondi)
    do {
        Read-Uart
        for ($i = $script:Cursore; $i -lt $script:Righe.Count; $i++) {
            if ($script:Righe[$i] -match $Regex) {
                $script:Cursore = $i + 1
                return $true
            }
        }
        # Anche la riga non ancora terminata (es. il prompt della shell "uart:~$ ").
        $p = ($script:Parziale -replace $Ansi, '')
        if ($p.Length -gt $script:ParzialeUsato) {
            $resto = $p.Substring($script:ParzialeUsato)
            if ($resto -match $Regex) {
                $script:Cursore = $script:Righe.Count
                $script:ParzialeUsato = $p.Length
                return $true
            }
        }
        Start-Sleep -Milliseconds 50
    } while ((Get-Date) -lt $fine)
    return $false
}

function Write-Uart([string]$Testo) {
    foreach ($c in ($Testo + "`r`n").ToCharArray()) {
        $b = $Ascii.GetBytes([string]$c)
        $script:UartStream.Write($b, 0, $b.Length)
        $script:UartStream.Flush()
        Start-Sleep -Milliseconds 5    # lento come una persona veloce: la UART non perde caratteri
    }
}

# --- monitor di Renode --------------------------------------------------------------------
function Invoke-Monitor([string]$Comando, [int]$Secondi = 5) {
    $buf = New-Object byte[] 4096
    $b = $Ascii.GetBytes($Comando + "`n")
    $script:MonStream.Write($b, 0, $b.Length)
    $script:MonStream.Flush()
    $risposta = ''
    $fine = (Get-Date).AddSeconds($Secondi)
    while ((Get-Date) -lt $fine) {
        while ($script:MonStream.DataAvailable) {
            $n = $script:MonStream.Read($buf, 0, $buf.Length)
            $risposta += $Ascii.GetString($buf, 0, $n)
        }
        $pulita = ($risposta -replace $Ansi, '') -replace '[^\x09\x0A\x0D\x20-\x7E]', ''
        if ($pulita -match $Prompt) { return $pulita }
        Start-Sleep -Milliseconds 50
    }
    return (($risposta -replace $Ansi, '') -replace '[^\x09\x0A\x0D\x20-\x7E]', '')
}

function Read-MonitorTutto {
    $buf = New-Object byte[] 4096
    while ($script:MonStream.DataAvailable) { [void]$script:MonStream.Read($buf, 0, $buf.Length) }
}

function Connect-Porta([int]$Porta, [int]$Secondi) {
    $fine = (Get-Date).AddSeconds($Secondi)
    while ((Get-Date) -lt $fine) {
        if ($script:Processo.HasExited) { throw "Renode si e' chiuso durante l'avvio (codice $($script:Processo.ExitCode))." }
        try {
            $c = New-Object System.Net.Sockets.TcpClient
            $c.Connect('127.0.0.1', $Porta)
            return $c
        } catch {
            Start-Sleep -Milliseconds 300
        }
    }
    throw "Renode non risponde sulla porta $Porta dopo $Secondi secondi."
}

# --- programma principale ---------------------------------------------------------------
$ok = 0; $ko = 0
$mai = New-Object System.Collections.Generic.List[string]
$script:Processo = $null
$uart = $null; $mon = $null

try {
    $app = Get-CartellaEsercizio $Esercizio
    if ($File) {
        $verifica = if (Test-Path $File) { (Resolve-Path $File).Path } else { Join-Path $script:Radice $File }
    } else {
        $verifica = Join-Path $app 'verifica.txt'
    }
    if (-not (Test-Path $verifica)) { throw "Non trovo il file di verifica $verifica" }
    $istruzioni = Get-Content $verifica -Encoding UTF8

    if (-not $NoBuild) { Invoke-Compilazione $app -Pulito:$Pulito }

    $resc = New-ScriptRenode $app 'verifica.resc' @(
        "emulation CreateServerSocketTerminal $PortaUart `"uartsock`" false",
        "connector Connect $script:UartConsole uartsock"
    )

    $renode = Get-Renode
    Write-Host "Avvio Renode senza finestre..." -ForegroundColor Cyan
    $script:Processo = Start-Process -FilePath $renode `
        -ArgumentList @('--disable-gui', '-P', "$PortaMonitor", "`"$resc`"") `
        -WorkingDirectory $app -WindowStyle Hidden -PassThru

    $mon = Connect-Porta $PortaMonitor 60
    $script:MonStream = $mon.GetStream()
    [void](Invoke-Monitor '' 15)          # aspetta il prompt: lo script .resc e' stato eseguito
    $uart = Connect-Porta $PortaUart 15
    $script:UartStream = $uart.GetStream()
    Read-MonitorTutto
    [void](Invoke-Monitor 'start')
    Write-Host "Emulazione avviata. Controlli da $verifica" -ForegroundColor Cyan

    foreach ($riga in $istruzioni) {
        $r = $riga.Trim()
        if ($r -eq '' -or $r.StartsWith('#')) { continue }

        if ($r -match '^attendi(\s+(\d+))?:\s*(.+)$') {
            $sec = if ($Matches[2]) { [int]$Matches[2] } else { $Timeout }
            $re = $Matches[3]
            if (Wait-Riga $re $sec) {
                Write-Host "[OK]   attendi: $re" -ForegroundColor Green; $ok++
            } else {
                Write-Host "[NO]   attendi: $re   (nessuna riga in $sec s)" -ForegroundColor Red; $ko++
                break   # le istruzioni successive dipendono di solito da questa
            }
        } elseif ($r -match '^mai:\s*(.+)$') {
            $mai.Add($Matches[1])
        } elseif ($r -match '^scrivi:\s?(.*)$') {
            Write-Host "       scrivi: $($Matches[1])" -ForegroundColor Cyan
            Write-Uart $Matches[1]
        } elseif ($r -match '^monitor:\s*(.+)$') {
            Write-Host "       monitor: $($Matches[1])" -ForegroundColor Cyan
            [void](Invoke-Monitor $Matches[1])
        } elseif ($r -match '^controlla:\s*(.+?)\s*=>\s*(.+)$') {
            $cmd = $Matches[1]; $atteso = $Matches[2]
            $risp = Invoke-Monitor $cmd
            # Togli l'eco del comando e il prompt, resta la risposta.
            $solo = (($risp -replace [regex]::Escape($cmd), '') -replace $Prompt, '').Trim()
            if ($solo -match [regex]::Escape($atteso)) {
                Write-Host "[OK]   controlla: $cmd => $atteso" -ForegroundColor Green; $ok++
            } else {
                Write-Host "[NO]   controlla: $cmd => $atteso   (risposta: '$solo')" -ForegroundColor Red; $ko++
            }
        } elseif ($r -match '^pausa:\s*(\d+)$') {
            Start-Sleep -Milliseconds ([int]$Matches[1])
            Read-Uart
        } else {
            Write-Host "[??]   istruzione non riconosciuta: $r" -ForegroundColor Yellow
        }
    }

    Read-Uart
    foreach ($re in $mai) {
        $tutte = @($script:Righe) + @($script:Parziale)
        $trovata = $tutte | Where-Object { $_ -match $re } | Select-Object -First 1
        if ($trovata) {
            Write-Host "[NO]   mai: $re   (trovata: '$trovata')" -ForegroundColor Red; $ko++
        } else {
            Write-Host "[OK]   mai: $re" -ForegroundColor Green; $ok++
        }
    }
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    $ko++
} finally {
    if ($mon) {
        try { [void](Invoke-Monitor 'quit' 2) } catch {}
        $mon.Close()
    }
    if ($uart) { $uart.Close() }
    if ($script:Processo -and -not $script:Processo.HasExited) {
        if (-not $script:Processo.WaitForExit(3000)) { Stop-Process -Id $script:Processo.Id -Force }
    }
}

Write-Host ''
if ($ko -eq 0 -and $ok -gt 0) {
    Write-Host "TUTTO OK ($ok controlli superati)" -ForegroundColor Green
    exit 0
} else {
    Write-Host "DA SISTEMARE: $ok superati, $ko non superati" -ForegroundColor Red
    exit 1
}
