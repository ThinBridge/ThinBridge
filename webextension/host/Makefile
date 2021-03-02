CFLAGS=/O2 /W2 /nologo /D_CRT_SECURE_NO_WARNINGS
CC=cl
LIBS=Advapi32.lib User32.lib Shell32.lib
DESTDIR=C:\ThinBridge\

all: ThinBridgeTalk.exe DummyTalker.exe

install: all reg
	!if not exist "$(DESTDIR)" mkdir "$(DESTDIR)"
	COPY ThinBridgeTalk.exe $(DESTDIR)
	COPY edge.json $(DESTDIR)

reg:
	reg add "HKLM\Software\Microsoft\Edge\NativeMessagingHosts\com.clear_code.thinbridge" /f /ve /t REG_SZ /d "$(DESTDIR)\edge.json"

clean:
	DEL *.exe
	DEL *.obj

ThinBridgeTalk.exe: main.c
	$(CC) $(CFLAGS) /Fe"ThinBridgeTalk.exe" main.c $(LIBS)

DummyTalker.exe: DummyTalker.c
	$(CC) $(CFLAGS) /Fe"DummyTalker.exe" DummyTalker.c
