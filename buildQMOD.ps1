$buildScript = "E:/BSMG/Android/android-ndk-r22b/build/ndk-build"
if (-not ($PSVersionTable.PSEdition -eq "Core")) {
    $buildScript += ".cmd"
}

$ArchiveName = "HitScoreVisualizer_v1.5.0.qmod"
$TempArchiveName = "HitScoreVisualizer_v1.5.0.qmod.zip"

& $buildScript NDK_PROJECT_PATH=$PSScriptRoot APP_BUILD_SCRIPT=$PSScriptRoot/Android.mk NDK_APPLICATION_MK=$PSScriptRoot/Application.mk
Compress-Archive -Path "./libs/arm64-v8a/libhitscore.so", "./libs/arm64-v8a/libbeatsaber-hook_2_2_5.so", "./mod.json" -DestinationPath $TempArchiveName -Force
Move-Item $TempArchiveName $ArchiveName -Force
Read-Host -Prompt "Press Enter to exit"