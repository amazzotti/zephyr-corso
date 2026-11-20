# Compila un esercizio e lo apre in Renode, con la finestra della UART (la console).
#
# Uso:
#     .\esegui.ps1 modulo0\02_hello            # compila (solo cio' che e' cambiato) e avvia
#     .\esegui.ps1 modulo0\02_hello -Pulito    # ricompila tutto da zero
#     .\esegui.ps1 modulo0\02_hello -NoBuild   # avvia senza ricompilare
#     .\esegui.ps1 modulo0\02_hello -Pausa     # carica ma non avvia: scrivi 'start' nel monitor
#
# Nella finestra del monitor di Renode puoi scrivere comandi (vedi modulo0/03_esplora_renode.md).
# Per ricominciare dall'inizio:  machine Reset   (poi start)  -  per uscire:  quit

param(
    [Parameter(Mandatory = $true, Position = 0)][string]$Esercizio,
    [switch]$Pulito,
    [switch]$NoBuild,
    [switch]$Pausa
)

$ErrorActionPreference = 'Stop'
. (Join-Path $PSScriptRoot 'strumenti\comune.ps1')

try {
    $app = Get-CartellaEsercizio $Esercizio
    if (-not $NoBuild) { Invoke-Compilazione $app -Pulito:$Pulito }

    $extra = @('showAnalyzer sysbus.uart0')
    if (-not $Pausa) { $extra += 'start' }
    $resc = New-ScriptRenode $app 'esegui.resc' $extra

    $renode = Get-Renode
    Write-Host "Avvio Renode con $resc" -ForegroundColor Cyan
    Start-Process -FilePath $renode -ArgumentList "`"$resc`"" -WorkingDirectory $app | Out-Null
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Red
    exit 1
}
