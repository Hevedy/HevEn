@ECHO OFF

set HEV_BIN=Binaries

start %HEV_BIN%\HevEn.exe "-u$HOME\My Games\HevEn" -gGameLog.txt -g %*
