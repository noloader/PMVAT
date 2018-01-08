
robocopy "%~dp0\..\pmvat\res" "%~dp0\common" pmvat.ico

robocopy "%~dp0\..\Release\x86" "%~dp0\x86" pmvat.exe
robocopy "%~dp0\..\Release\x64" "%~dp0\x64" pmvat.exe

PAUSE

exit 0