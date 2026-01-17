param(
    [Parameter(Mandatory=$true)]
    [ValidateSet("debug","debug_perf","release","release_dbginfo","release_perf")]
    [string]$BuildType,

    [Parameter(Mandatory=$true)]
    [ValidateSet("shared","static")]
    [string]$LinkType,

    [Parameter(Mandatory=$true)]
    [ValidateSet("run","cleanbuild","build")]
    [string]$Action,

    [switch]$DryRun,

    [switch]$Help
)

$ErrorActionPreference = "Stop"


# From: https://stackoverflow.com/a/12605755
function Force-Resolve-Path {
    <#
        .SYNOPSIS
        Calls Resolve-Path but works for files that don't exist.
        .REMARKS
        From http://devhawk.net/blog/2010/1/22/fixing-powershells-busted-resolve-path-cmdlet
    #>
    param (
        [string] $FileName
    )

    $FileName = Resolve-Path $FileName -ErrorAction SilentlyContinue `
                                            -ErrorVariable _frperror
    if (-not($FileName)) {
        $FileName = $_frperror[0].TargetObject
    }

    return $FileName
}


function Show-Help {
    Write-Host "Usage: .\build_script.ps1 -BuildType <BuildType> -LinkType <LinkType> -Action <Action> [-DryRun] [-Help]"
    Write-Host ""
    Write-Host "Parameters:"
    Write-Host "  -BuildType <BuildType>  : The build type. Possible values:"
    Write-Host "                             debug, debug_perf, release, release_dbginfo, release_perf"
    Write-Host "  -LinkType <LinkType>    : The type of linking. Possible values:"
    Write-Host "                             shared, static"
    Write-Host "  -Action <Action>        : The action to perform. Possible values:"
    Write-Host "                             run, cleanbuild, build"
    Write-Host "  -DryRun                 : (Optional) If set, simulates the actions without executing them."
    Write-Host "  -Help                   : (Optional) Displays this help message."
    Write-Host ""
    Write-Host "Examples:"
    Write-Host "  .\build_script.ps1 -BuildType debug -LinkType shared -Action build"
    Write-Host "  .\build_script.ps1 -BuildType release_perf -LinkType static -Action cleanbuild -DryRun"
}


if ($Help) {
    Show-Help
    return
}




$PROJECT_NAME = "hydrogen_atom_electron_cloud"
$CMAKE_ARGLIST = @(
    "-DCMAKE_C_COMPILER=clang"
    "-DCMAKE_CXX_COMPILER=clang++"
    "-DCMAKE_EXPORT_COMPILE_COMMANDS=1"
    "-DGIT_SUBMODULE=1"
    "-DCMAKE_COLOR_DIAGNOSTICS=ON"
    "-DSDLSHADERCROSS_DXC=OFF"
    "-DSDL3_DIR=dependencies/SDL3"
    "-Dspirv_cross_c_shared_DIR=${env:VULKAN_SDK}\Lib\spirv_cross_c_shared\cmake" # << This is what fixed it for me. The Problem: https://vulkan.lunarg.com/issue/view/66fef9a807baa3c85a20d08a
    "-Dshaderc_combined_DIR=${env:VULKAN_SDK}\Lib\cmake"

    # "-DCMAKE_PREFIX_PATH=${env:VULKAN_SDK_PATH};${env:VULKAN_SDK_PATH}\Lib\cmake" # This is the problem, cmake doesn't find the /cmake dir
)


$CMAKE_ROOT_BUILD_DIR = "build"
$CMAKE_INTRMD_BUILD_DIR = ""
$CLEAN_CURRENT_ROOT_BUILD_DIR = $true
$RUN_BINARY_FLAG = $false
$BUILD_BINARIES_FLAG = $false


switch ($BuildType) {
    "debug" {
        $CMAKE_ARGLIST += "-DCMAKE_BUILD_TYPE=Debug"
        $CMAKE_INTRMD_BUILD_DIR += "debug"
    }
    "debug_perf" {
        $CMAKE_ARGLIST += "-DCMAKE_BUILD_TYPE=Debug"
        $CMAKE_ARGLIST += "-DMEASURE_PERFORMANCE_TIMEOUT=1"
        $CMAKE_INTRMD_BUILD_DIR += "debug_perf"
    }
    "release" {
        $CMAKE_ARGLIST += "-DCMAKE_BUILD_TYPE=Release"
        $CMAKE_INTRMD_BUILD_DIR += "release"
    }
    "release_dbginfo" {
        $CMAKE_ARGLIST += "-DCMAKE_BUILD_TYPE=RelWithDbgInfo"
        $CMAKE_INTRMD_BUILD_DIR += "release_dbginfo"
    }
    "release_perf" {
        $CMAKE_ARGLIST += "-DCMAKE_BUILD_TYPE=Release"
        $CMAKE_ARGLIST += "-DMEASURE_PERFORMANCE_TIMEOUT=1"
        $CMAKE_INTRMD_BUILD_DIR += "release_perf"
    }
}

switch ($LinkType) {
    "shared" {
        $CMAKE_ARGLIST += "-DBUILD_SHARED_LIBS=1"
        $CMAKE_INTRMD_BUILD_DIR = Join-Path $CMAKE_INTRMD_BUILD_DIR "shared"
    }
    "static" {
        $CMAKE_ARGLIST += "-DBUILD_SHARED_LIBS=0"
        $CMAKE_INTRMD_BUILD_DIR = Join-Path $CMAKE_INTRMD_BUILD_DIR "static"
    }
}

switch ($Action) {
    "run" {
        $CLEAN_CURRENT_ROOT_BUILD_DIR = $false
        $BUILD_BINARIES_FLAG = $false
        $RUN_BINARY_FLAG = $true
    }
    "cleanbuild" {
        $CLEAN_CURRENT_ROOT_BUILD_DIR = $true
        $BUILD_BINARIES_FLAG = $true
        $RUN_BINARY_FLAG = $false
    }
    "build" {
        $CLEAN_CURRENT_ROOT_BUILD_DIR = $false
        $BUILD_BINARIES_FLAG = $true
        $RUN_BINARY_FLAG = $false
    }
}


$CMAKE_INTRMD_BUILD_DIR = $CMAKE_INTRMD_BUILD_DIR.Trim([char[]]"\/")
$CMAKE_FINAL_BUILD_DIR = Join-Path $CMAKE_ROOT_BUILD_DIR $CMAKE_INTRMD_BUILD_DIR
Write-Host "======== DRY RUN: $DryRun ========"
Write-Host "Build Type      : $BuildType"
Write-Host "Link Type       : $LinkType"
Write-Host "Action          : $Action"
Write-Host "Final Build Dir : $CMAKE_FINAL_BUILD_DIR"
Write-Host "CMake Arguments : $($CMAKE_ARGLIST -join ' ')"
Write-Host ""


function RunOrEcho {
    param (
        [string]$Description,
        [ScriptBlock]$Codeblock
    )


    if ($DryRun) {
        Write-Host "Would run: '$Description'"
    } 
    else {
        try {
            & $Codeblock
        }
        catch {
            Write-Host "Command '$Description' Failed with Code '$LASTEXITCODE'"
            Write-Host "MESSAGE`n$($_.Exception.Message)"
            exit
        }
    }
}






# --- Simulate Execution ---

if (-not (Test-Path $CMAKE_ROOT_BUILD_DIR)) {
    $mkdir_p_option_equiv = {
        New-Item -ItemType Directory -Path $CMAKE_ROOT_BUILD_DIR | Out-Null
    }
    Write-Host "Testing Path $CMAKE_ROOT_BUILD_DIR -> " Test-Path -IsValid $CMAKE_ROOT_BUILD_DIR
    RunOrEcho -Description "Create directory '$CMAKE_ROOT_BUILD_DIR'" -Codeblock $mkdir_p_option_equiv
}

if ($CLEAN_CURRENT_ROOT_BUILD_DIR -and (Test-Path $CMAKE_FINAL_BUILD_DIR)) {
    $remove_build_dir_recurse = { 
        Remove-Item -Recurse -Force $CMAKE_FINAL_BUILD_DIR 
    }
    RunOrEcho -Description "Remove build directory '$CMAKE_FINAL_BUILD_DIR'" -Codeblock $remove_build_dir_recurse
}

if ($BUILD_BINARIES_FLAG) {
    $cmakeCmd = "cmake -S . -B '$CMAKE_FINAL_BUILD_DIR' -G 'Ninja'"
    $cmakeCmd = -join($cmakeCmd, " ", $CMAKE_ARGLIST)
    
    $mkdir_p_option_equiv = {
        # New-Item -ItemType Directory -Path $CMAKE_FINAL_BUILD_DIR -Force | Out-Null 
        New-Item -ItemType Directory -Path $CMAKE_FINAL_BUILD_DIR -Force
    }

    $cmakePrepareBuild = {
        cmake -S "." -B $CMAKE_FINAL_BUILD_DIR -G "Ninja" $CMAKE_ARGLIST
    }

    $build_with_ninja = {
        ninja $PROJECT_NAME
    }


    RunOrEcho -Description "Create Final build directory '$CMAKE_FINAL_BUILD_DIR'" -Codeblock $mkdir_p_option_equiv
    RunOrEcho -Description $cmakeCmd -Codeblock $cmakePrepareBuild
    if(-not $DryRun) {
        Push-Location $CMAKE_FINAL_BUILD_DIR
    }

    if (Test-Path "compile_commands.json") {
        $target = Force-Resolve-Path "..\..\compile_commands.json"
        RunOrEcho -Description "Copy compile_commands.json to build root" -Codeblock {
            Copy-Item "compile_commands.json" $target -Force
        }
    }

    RunOrEcho -Description "ninja $PROJECT_NAME" -Codeblock $build_with_ninja

    if(-not $DryRun) {
        Pop-Location
    }
}

if ($RUN_BINARY_FLAG) {
    $ninja_run_binary_cmake_rule = {
        ninja run_binary
    }
    Push-Location $CMAKE_FINAL_BUILD_DIR
    RunOrEcho -Description "ninja run_binary" -Codeblock $ninja_run_binary_cmake_rule
    Pop-Location
}
