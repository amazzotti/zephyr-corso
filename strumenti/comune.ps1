# Funzioni comuni agli script del corso (esegui.ps1, verifica.ps1).
# Non si lancia da solo: viene incluso dagli altri script.

$script:Radice = Split-Path -Parent $PSScriptRoot
$script:Board  = 'nrf52840dk/nrf52840'
$script:Repl   = Join-Path $script:Radice 'renode\nrf52840dk.repl'
$script:Resc   = Join-Path $script:Radice 'renode\esercizio.resc'

# Percorso per Renode: barre normali, che Renode accetta anche su Windows.
function ConvertTo-PercorsoRenode([string]$Percorso) {
    return ($Percorso -replace '\\', '/')
}

# Trova la cartella dell'esercizio, relativa alla cartella corrente o alla radice del repo.
function Get-CartellaEsercizio([string]$Esercizio) {
    $candidati = @($Esercizio, (Join-Path $script:Radice $Esercizio))
    foreach ($c in $candidati) {
        if ((Test-Path $c) -and (Test-Path (Join-Path $c 'CMakeLists.txt'))) {
            return (Resolve-Path $c).Path
        }
    }
    throw "Non trovo l'esercizio '$Esercizio' (serve una cartella con CMakeLists.txt, es. modulo0\02_hello)."
}

# Compila con west. -Pulito ricompila da zero (serve dopo modifiche a Kconfig o overlay
# se qualcosa non torna).
function Invoke-Compilazione([string]$App, [switch]$Pulito) {
    if (-not $env:VIRTUAL_ENV -or -not $env:ZEPHYR_BASE) {
        throw "Ambiente Zephyr non attivo. Esegui prima:  . .\attiva.ps1"
    }
    $build = Join-Path $App 'build'
    $wargs = @('build', '-b', $script:Board, '-d', $build, '--no-sysbuild')
    if ($Pulito) { $wargs += @('-p', 'always') } else { $wargs += @('-p', 'auto') }
    $wargs += $App
    Write-Host "west $($wargs -join ' ')" -ForegroundColor DarkGray
    # "python -m west" invece di "west.exe": Smart App Control di Windows blocca west.exe
    # (non firmato), mentre python.exe e' firmato.
    # west scrive messaggi normali anche su stderr: con ErrorActionPreference = Stop
    # PowerShell 5.1 li tratterebbe come errori. Qui conta solo il codice di uscita.
    $prima = $ErrorActionPreference
    $ErrorActionPreference = 'Continue'
    & python -m west @wargs
    $ErrorActionPreference = $prima
    if ($LASTEXITCODE -ne 0) {
        throw "Compilazione fallita: leggi il PRIMO errore nell'output qui sopra."
    }
}

function Get-Elf([string]$App) {
    $elf = Join-Path $App 'build\zephyr\zephyr.elf'
    if (-not (Test-Path $elf)) {
        throw "Non trovo $elf. Compila prima l'esercizio (senza -NoBuild)."
    }
    return $elf
}

# Trova l'eseguibile di Renode: variabile RENODE, PATH, cartelle di installazione tipiche.
function Get-Renode {
    if ($env:RENODE -and (Test-Path $env:RENODE)) { return $env:RENODE }
    $cmd = Get-Command renode -ErrorAction SilentlyContinue
    if ($cmd) { return $cmd.Source }
    $tipici = @(
        "$env:ProgramFiles\Renode\bin\Renode.exe",
        "$env:ProgramFiles\Renode\Renode.exe",
        "${env:ProgramFiles(x86)}\Renode\bin\Renode.exe",
        "$env:LOCALAPPDATA\Programs\Renode\bin\Renode.exe"
    )
    foreach ($p in $tipici) { if (Test-Path $p) { return $p } }
    throw "Renode non trovato. Installalo (modulo0/01_installazione.md) oppure imposta `$env:RENODE con il percorso di Renode.exe."
}

# Scrive lo script Renode che carica scheda e firmware. $Extra = comandi da aggiungere in fondo.
function New-ScriptRenode([string]$App, [string]$Nome, [string[]]$Extra) {
    $elf = ConvertTo-PercorsoRenode (Get-Elf $App)
    $righe = @(
        '# File generato automaticamente da esegui.ps1 / verifica.ps1: non modificarlo.',
        "`$bin=@$elf",
        "`$repl=@$(ConvertTo-PercorsoRenode $script:Repl)",
        "include @$(ConvertTo-PercorsoRenode $script:Resc)"
    ) + $Extra
    $file = Join-Path $App "build\$Nome"
    Set-Content -Path $file -Value $righe -Encoding ascii
    return $file
}
