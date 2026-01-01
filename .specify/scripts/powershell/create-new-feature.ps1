#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Create a new feature branch and initialize spec directory structure.

.DESCRIPTION
    This script creates a new feature branch, initializes the spec directory,
    and outputs JSON with the branch name and spec file path.

.PARAMETER Number
    The feature number (e.g., 1, 2, 3)

.PARAMETER ShortName
    The short name for the feature (e.g., "user-auth", "shbandvector-view")

.PARAMETER Description
    The full feature description

.PARAMETER Json
    Output results as JSON

.EXAMPLE
    .\create-new-feature.ps1 -Number 1 -ShortName "user-auth" -Description "Add user authentication" -Json
#>

param(
    [Parameter(Mandatory=$true)]
    [int]$Number,
    
    [Parameter(Mandatory=$true)]
    [string]$ShortName,
    
    [Parameter(Mandatory=$true)]
    [string]$Description,
    
    [switch]$Json
)

$ErrorActionPreference = "Stop"

# Construct branch name
$BranchName = "$Number-$ShortName"

# Construct paths
$SpecDir = "specs/$BranchName"
$SpecFile = "$SpecDir/spec.md"
$ChecklistDir = "$SpecDir/checklists"

try {
    # Create and checkout new branch
    git checkout -b $BranchName 2>&1 | Out-Null
    
    # Create directory structure
    New-Item -ItemType Directory -Path $SpecDir -Force | Out-Null
    New-Item -ItemType Directory -Path $ChecklistDir -Force | Out-Null
    
    # Create empty spec file
    New-Item -ItemType File -Path $SpecFile -Force | Out-Null
    
    # Output JSON
    if ($Json) {
        $result = @{
            BRANCH_NAME = $BranchName
            SPEC_FILE = $SpecFile
            SPEC_DIR = $SpecDir
            CHECKLIST_DIR = $ChecklistDir
            NUMBER = $Number
            SHORT_NAME = $ShortName
            DESCRIPTION = $Description
        }
        $result | ConvertTo-Json -Compress
    } else {
        Write-Host "Branch created: $BranchName"
        Write-Host "Spec file: $SpecFile"
    }
    
    exit 0
} catch {
    Write-Error "Failed to create feature: $_"
    exit 1
}

