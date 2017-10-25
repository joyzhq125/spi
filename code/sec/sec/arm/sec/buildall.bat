@echo usage: build ROBASE RWBASE AXD
@echo For example: build 0x01234567 0 AXD
nmake -f sec.mak clean
set ROBASE=%1
set RWBASE=%2
set BREWDEBUG=%3
set LINTFLAG=%4
nmake -f sec.mak