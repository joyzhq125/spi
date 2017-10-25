echo usage: build ROBASE RWBASE DEBUGMOD
echo For example: build 0x01234567 0 AXD

nmake -f sec.mak cleanExe
set ROBASE=%1
set RWBASE=%2
set BREWDEBUG=%3
nmake -f sec.mak