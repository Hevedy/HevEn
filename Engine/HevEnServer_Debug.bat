@ECHO OFF

set HEV_BIN=Binaries

start %HEV_BIN%\HevEn_Debug.exe "-u$HOME\My Games\HevEn" -gDServerLog.txt -d %*
