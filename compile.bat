gcc -o Carbyte_rs main.c utils.c payload.c watchdog.c uac_bypass.c reverse_shell/reverse_shell.c reverse_shell/auth.c reverse_shell/tools.c -LC:/msys64/mingw64/lib -lwebsockets_static -lssl -lcrypto -lz -lws2_32 -lcrypt32 -static

pause