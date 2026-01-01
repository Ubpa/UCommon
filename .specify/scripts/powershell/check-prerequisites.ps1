#!/usr/bin/env pwsh
<#
.SYNOPSIS
    Check prerequisites for feature development workflow.

.DESCRIPTION
    Validates feature branch environment and outputs paths to key files.

.PARAMETER Json
    Output results as JSON

.PARAMETER PathsOnly
    Output only paths (minimal JSON)

.EXAMPLE
    .\check-prerequisites.ps1 -Json -PathsOnly
#>

param(
    [switch]$Json,
    [switch]$PathsOnly
)

$ErrorActionPreference = "Stop"

try {
    # Get current branch name
    $BranchName = git rev-parse --abbrev-ref HEAD 2>&1
    if ($LASTEXITCODE -ne 0) {
        throw "Not in a git repository"
    }
    
    # Check if on feature branch (format: N-short-name)
    if ($BranchName -notmatch '^(\d+)-(.+)$') {
        throw "Not on a feature branch. Expected format: N-short-name (e.g., 1-shbandvector-view)"
    }
    
    $FeatureNumber = $Matches[1]
    $ShortName = $Matches[2]
    
    # Construct paths
    $FeatureDir = "specs/$BranchName"
    $SpecFile = "$FeatureDir/spec.md"
    $ImplPlan = "$FeatureDir/plan.md"
    $Tasks = "$FeatureDir/tasks.md"
    $ChecklistDir = "$FeatureDir/checklists"
    
    # Check if spec file exists
    if (-not (Test-Path $SpecFile)) {
        throw "Spec file not found: $SpecFile. Run /speckit.specify first."
    }
    
    # Output results
    if ($Json) {
        $result = @{
            BRANCH_NAME = $BranchName
            FEATURE_NUMBER = [int]$FeatureNumber
            SHORT_NAME = $ShortName
            FEATURE_DIR = $FeatureDir
            FEATURE_SPEC = $SpecFile
            IMPL_PLAN = $ImplPlan
            TASKS = $Tasks
            CHECKLIST_DIR = $ChecklistDir
            SPEC_EXISTS = Test-Path $SpecFile
            PLAN_EXISTS = Test-Path $ImplPlan
            TASKS_EXISTS = Test-Path $Tasks
        }
        
        if ($PathsOnly) {
            # Minimal output
            $minimal = @{
                FEATURE_DIR = $FeatureDir
                FEATURE_SPEC = $SpecFile
                IMPL_PLAN = $ImplPlan
                TASKS = $Tasks
            }
            $minimal | ConvertTo-Json -Compress
        } else {
            $result | ConvertTo-Json -Compress
        }
    } else {
        Write-Host "Branch: $BranchName"
        Write-Host "Feature Dir: $FeatureDir"
        Write-Host "Spec File: $SpecFile"
        Write-Host "Plan File: $ImplPlan"
        Write-Host "Tasks File: $Tasks"
    }
    
    exit 0
} catch {
    Write-Error "Prerequisites check failed: $_"
    exit 1
}

