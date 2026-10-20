param(
    [ValidateSet('Debug', 'Release')]
    [string]$Configuration = 'Debug',
    [switch]$SkipRender
)

$ErrorActionPreference = 'Stop'
$repoRoot = Split-Path -Parent $PSScriptRoot
$sourceDir = Join-Path $repoRoot 'dx-tutorial'
$outputDir = Join-Path $repoRoot ".model-validation\tests-$Configuration"
New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio\Installer\vswhere.exe'
$vsRoot = & $vswhere -latest -products '*' -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath
if (-not $vsRoot) { throw 'Visual Studio C++ build tools are required.' }
$toolchain = Join-Path $vsRoot 'Common7\Tools\VsDevCmd.bat'
$runtime = if ($Configuration -eq 'Debug') { '/MTd /Zi /Od' } else { '/MT /O2' }
$flags = "/nologo /EHsc /std:c++20 /W4 /DNOMINMAX $runtime /I`"$sourceDir`""
$commands = @(
    '@echo off',
    "call `"$toolchain`" -no_logo -arch=x64",
    'if errorlevel 1 exit /b 1',
    "cl /nologo /TC /c /W0 $runtime `"$repoRoot\third_party\ufbx\ufbx.c`" /Foufbx.obj",
    'if errorlevel 1 exit /b 1',
    "cl $flags /c `"$sourceDir\ModelAsset.cpp`"",
    'if errorlevel 1 exit /b 1',
    "cl $flags `"$PSScriptRoot\ModelAssetTests.cpp`" ModelAsset.obj ufbx.obj /FeModelAssetTests.exe",
    'if errorlevel 1 exit /b 1'
)
if (-not $SkipRender) {
    $commands += @(
        "cl $flags /c `"$sourceDir\Mesh.cpp`" `"$sourceDir\Shader.cpp`"",
        'if errorlevel 1 exit /b 1',
        "cl $flags `"$PSScriptRoot\RenderAssets.cpp`" ModelAsset.obj Mesh.obj Shader.obj ufbx.obj /FeRenderAssets.exe /link d3d11.lib d3dcompiler.lib",
        'if errorlevel 1 exit /b 1'
    )
}
$buildScript = Join-Path $outputDir 'build-tests.cmd'
$commands | Set-Content -LiteralPath $buildScript -Encoding ascii
Push-Location $outputDir
try {
    & $env:ComSpec /d /c $buildScript
    if ($LASTEXITCODE -ne 0) { throw 'Asset test compilation failed.' }
    & (Join-Path $outputDir 'ModelAssetTests.exe') (Join-Path $sourceDir 'assets\models')
    if ($LASTEXITCODE -ne 0) { throw 'Model import tests failed.' }
    if (-not $SkipRender) {
        & (Join-Path $outputDir 'RenderAssets.exe') $sourceDir (Join-Path $outputDir 'models.bmp') --debug
        if ($LASTEXITCODE -ne 0) { throw 'Model rendering tests failed.' }
    }
}
finally { Pop-Location }
