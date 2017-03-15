@ECHO OFF

set HEV_BIN=bin

start %HEV_BIN%\HevEn.exe "-u$HOME\My Games\HevEn" -glog.txt %*
