# Attiva l'ambiente Zephyr in questo terminale PowerShell.
#
# Uso (dalla cartella del repo, una volta per ogni terminale aperto):
#     . .\attiva.ps1
#     . .\attiva.ps1 -Workspace D:\zp     # se il workspace e' altrove
#
# Cosa fa:
#   1. attiva l'ambiente virtuale Python del workspace (dove c'e' west);
#   2. imposta ZEPHYR_BASE, cioe' dove si trovano i sorgenti di Zephyr;
#   3. controlla che west e Renode si trovino.

param(
    [string]$Workspace = $(if ($env:ZEPHYR_WORKSPACE) { $env:ZEPHYR_WORKSPACE } else { 'C:\zephyrproject' })
)

$attivatore = Join-Path $Workspace '.venv\Scripts\Activate.ps1'
if (-not (Test-Path $attivatore)) {
    Write-Host "Non trovo $attivatore" -ForegroundColor Red
    Write-Host "Il workspace Zephyr non e' in $Workspace? Vedi modulo0/01_installazione.md" -ForegroundColor Red
    return
}

. $attivatore
$env:ZEPHYR_BASE = Join-Path $Workspace 'zephyr'

# Smart App Control di Windows blocca west.exe (non firmato). Questa funzione fa si' che
# scrivendo "west ..." nel terminale parta "python -m west ...", che funziona.
function global:west { & python -m west @args }

Write-Host "Workspace:   $Workspace"
Write-Host "ZEPHYR_BASE: $env:ZEPHYR_BASE"

$versione = & python -m west --version 2>$null
if ($LASTEXITCODE -eq 0) {
    Write-Host "west:        $versione" -ForegroundColor Green
} else {
    Write-Host "west non trovato nell'ambiente virtuale: python -m pip install west" -ForegroundColor Red
}

. (Join-Path $PSScriptRoot 'strumenti\comune.ps1')
try {
    $r = Get-Renode
    Write-Host "Renode:      $r" -ForegroundColor Green
} catch {
    Write-Host $_.Exception.Message -ForegroundColor Yellow
}
