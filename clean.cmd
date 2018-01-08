attrib -r -a -s -h "%~dp0\*.ncb"
attrib -r -a -s -h "%~dp0\*.suo"

attrib -r -a -s -h "%~dp0\pmvat\*.ncb"
attrib -r -a -s -h "%~dp0\pmvat\*.suo"

rmdir /s /q "%~dp0\Debug"
rmdir /s /q "%~dp0\Release"

rmdir /s /q "%~dp0\pmvat\Debug"
rmdir /s /q "%~dp0\pmvat\Release"

erase /f /s "%~dp0\*.ncb"
erase /f /s "%~dp0\*.suo"
erase /f /s "%~dp0\pmvat\*.user"
erase /f /s "%~dp0\pmvat\*.aps"
