Param(
    [String]$qmodname="",
    [Parameter(Mandatory=$false)]
    [Switch] $help
)

if ($help -eq $true) {
    Write-Output "`"createqmod`" - Creates a .qmod file with your compiled libraries and mod.json."
    Write-Output "`n-- Parameters --`n"

    Write-Output "`n-- Arguments --`n"

    Write-Output "qmodName `t The file name of your qmod"

    exit
}

$mod = "./mod.json"
$modJson = Get-Content $mod -Raw | ConvertFrom-Json

if ($qmodName -eq "")
{
    $qmodName = $modJson.name
}

$filelist = @($mod)

$cover = "./" + $modJson.coverImage
if ((-not ($cover -eq "./")) -and (Test-Path $cover))
{
    $filelist += ,$cover
}

foreach ($mod in $modJson.modFiles)
{
    $path = "./build/" + $mod
    if (-not (Test-Path $path))
    {
        $path = "./extern/libs/" + $mod
    }
    $filelist += $path
}

foreach ($lib in $modJson.libraryFiles)
{
    $path = "./build/" + $lib
    if (-not (Test-Path $path))
    {
        $path = "./extern/libs/" + $lib
    }
    $filelist += $path
}

$zip = $qmodName + ".zip"
$qmod = $qmodName + ".qmod"

Compress-Archive -Path $filelist -DestinationPath $zip -Update
Move-Item $zip $qmod -Force