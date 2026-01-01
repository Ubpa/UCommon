#!/usr/bin/env pwsh
param(
    [string]$FilePath = "specs/1-shbandvector-view/spec.md"
)

$ErrorActionPreference = "Stop"

Write-Host "Reading file: $FilePath"
$content = Get-Content $FilePath -Raw -Encoding UTF8

Write-Host "Performing type renaming..."
# Order matters - replace longer names first to avoid partial replacements
$content = $content -replace 'TSHBandVectorRGBConst', 'TSHBandConstViewRGB'
$content = $content -replace 'TSHBandVectorRGB', 'TSHBandViewRGB'
$content = $content -replace 'TSHBandVectorConst', 'TSHBandConstView'
$content = $content -replace 'TSHBandVector', 'TSHBandView'

Write-Host "Writing updated content..."
$content | Set-Content $FilePath -Encoding UTF8 -NoNewline

Write-Host "Type renaming completed successfully!"

