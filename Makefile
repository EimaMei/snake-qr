# NOTE: 'cl' is assumed to be in x86 mode.

all:
	cl /nologo /c /O1 /GS- snake.c
	link snake.obj /nologo /entry:main /nodefaultlib /subsystem:windows /align:16 kernel32.lib user32.lib gdi32.lib

qrcode:
	qrencode -r snake.exe -8 -o code.png
